/* Copyright by 2022 The Chime Developers. All rights reserved.
 *
 * License under Apache License, Version 2.0.
 * See the LICENSE file at the project root for license information.
 */

#ifndef CHIME_CORE_RUNTIME_BFC_ALLOCATOR_H_
#define CHIME_CORE_RUNTIME_BFC_ALLOCATOR_H_

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <memory>
#include <sstream>
#include <vector>

#include "chime/core/framework/types.hpp"
#include "chime/core/memory/allocator.h"
#include "chime/core/platform/macros.h"
#include "chime/core/platform/mutex.h"
#include "chime/core/platform/thread_annotations.h"
#include "chime/core/util/optional.hpp"
  
namespace chime {
namespace memory {

class BFCAllocator : public Allocator {
 public:
  struct Options {
    bool allow_growth = true;
    bool allow_retry_on_failure = true;
  };

  BFCAllocator(std::unique_ptr<SubAllocator> sub_allocator, size_t total_memory,
               const string &name, const Options &opts);

  ~BFCAllocator() override;

  string Name() override;

  void *AllocateRaw(size_t alignment, size_t num_bytes) override {
    return AllocateRaw(alignment, num_bytes, AllocationAttributes());
  }

  void *AllocateRaw(size_t alignment, size_t num_bytes,
                    const AllocationAttributes &allocation_attr) override;

  void DeallocateRaw(void *ptr) override;

  bool TracksAllocationSizes() const override { return true; }

  size_t RequestedSize(const void *ptr) const override;

  size_t AllocatedSize(const void *ptr) const override;

  int64_t AllocationID(const void *ptr) const override;

  util::Optional<AllocatorStats> GetStats() override;

  bool ClearStats() override;

  AllocatorMemoryType GetMemoryType() const override;

 private:
  struct Bin;
  struct Chunk;

  void *AllocateRawInternal(size_t alignment, size_t num_bytes,
                            bool dump_log_on_failure,
                            uint64 freed_before_count);

  void *AllocateRawInternalWithRetry(
      size_t alignment, size_t num_bytes,
      const AllocationAttributes &allocation_attr);

  void DeallocateRawInternal(void *ptr);

  int64_t LargestFreeChunk() CHIME_EXCLUSIVE_LOCKS_REQUIRED(_mu);

  void AddTraceMe(const std::string &name, const void *ptr)
      CHIME_EXCLUSIVE_LOCKS_REQUIRED(_mu);

  void AddTraceMe(const std::string &name, const void *chunk_ptr,
                  int64_t req_size, int64_t alloc_size)
      CHIME_EXCLUSIVE_LOCKS_REQUIRED(_mu);

  typedef int64_t ChunkHandle;
  static constexpr ChunkHandle INVALID_CHUNK_HANDLE = SIZE_MAX;

  typedef int BinNum;
  static constexpr BinNum INVALID_BIN_NUM = -1;
  static constexpr BinNum NUM_BINS = 21;

  struct Chunk {
    size_t size = 0;
    size_t req_size = 0;
    int64_t allocation_id = -1;
    void *ptr = nullptr;

    ChunkHandle prev = INVALID_CHUNK_HANDLE;
    ChunkHandle next = INVALID_CHUNK_HANDLE;

    BinNum bin_num = INVALID_BIN_NUM;

    uint64_t freed_at_count = 0;

    bool is_use() const { return allocation_id != -1; }

    std::string DebugString() const {
      std::stringstream ss;
      ss << "Chunk " << ptr << " | size: " << size
         << " | req_size: " << req_size << " | alloc_id: " << allocation_id
         << " | bin_num: " << bin_num;
      if (prev != INVALID_CHUNK_HANDLE) {
        ss << " | prev: " << prev;
      }
      if (next != INVALID_CHUNK_HANDLE) {
        ss << " | next: " << next;
      }
      return ss.str();
    }
  };

  struct Bin {
    size_t bin_size =
        0;  // bin_size is the size of the smallest chunk in this bin.

    class ChunkComparator {
     public:
      explicit ChunkComparator(BFCAllocator *allocator)
          : _allocator(allocator) {}
      bool operator()(const ChunkHandle ha, const ChunkHandle hb) const {
        const Chunk *a = _allocator->ChunkFromHandle(ha);
        const Chunk *b = _allocator->ChunkFromHandle(hb);
        if (a->size != b->size) {
          return a->size < b->size;
        }
        return a->ptr < b->ptr;
      }

     private:
      BFCAllocator *_allocator;  // not owned
    };

    typedef std::set<ChunkHandle, ChunkComparator> FreeChunkSet;

    FreeChunkSet free_chunks;
    Bin(BFCAllocator *allocator, size_t bs)
        : bin_size(bs), free_chunks(ChunkComparator(allocator)) {}
  };

  static constexpr size_t MIN_ALLOCATION_BITS = 8;
  static constexpr size_t MIN_ALLOCATION_SIZE = 1 << MIN_ALLOCATION_BITS;

  class AllocationRegion {
   public:
    AllocationRegion(void *ptr, size_t mem_size)
        : _ptr(ptr),
          _memory_size(mem_size),
          _end_ptr(
              static_cast<void *>(static_cast<char *>(_ptr) + _memory_size)) {
      DCHECK_EQ(0, mem_size % MIN_ALLOCATION_SIZE);
      const size_t n_handles =
          (_memory_size + MIN_ALLOCATION_SIZE - 1) / MIN_ALLOCATION_SIZE;
      _handles.resize(n_handles, INVALID_CHUNK_HANDLE);
    }

    AllocationRegion() = default;
    AllocationRegion(AllocationRegion &&other) { Swap(&other); }
    AllocationRegion &operator=(AllocationRegion &&other) {
      Swap(&other);
      return *this;
    }

    void *ptr() const { return _ptr; }
    size_t memory_size() const { return _memory_size; }
    void *end_ptr() const { return _end_ptr; }

    void Extend(size_t size) {
      _memory_size += size;
      DCHECK_EQ(0, _memory_size % MIN_ALLOCATION_SIZE);
      _end_ptr = static_cast<void *>(static_cast<char *>(_ptr) + _memory_size);
      const size_t n_handles =
          (_memory_size + MIN_ALLOCATION_SIZE - 1) / MIN_ALLOCATION_SIZE;
      _handles.resize(n_handles, INVALID_CHUNK_HANDLE);
    }

    ChunkHandle GetHandle(const void *ptr) const {
      return _handles[IndexFor(ptr)];
    }

    void SetHandle(const void *ptr, ChunkHandle h) {
      _handles[IndexFor(ptr)] = h;
    }

    void Erase(const void *ptr) {
      _handles[IndexFor(ptr)] = INVALID_CHUNK_HANDLE;
    }

   private:
    void Swap(AllocationRegion *other) {
      std::swap(_ptr, other->_ptr);
      std::swap(_memory_size, other->_memory_size);
      std::swap(_end_ptr, other->_end_ptr);
      std::swap(_handles, other->_handles);
    }

    size_t IndexFor(const void *ptr) const {
      std::uintptr_t p_int = reinterpret_cast<std::uintptr_t>(ptr);
      std::uintptr_t base_int = reinterpret_cast<std::uintptr_t>(_ptr);
      DCHECK_GE(p_int, base_int);
      DCHECK_LT(p_int, base_int + _memory_size);
      return static_cast<size_t>(((p_int - base_int) >> MIN_ALLOCATION_BITS));
    }

    void *_ptr = nullptr;
    size_t _memory_size = 0;
    void *_end_ptr = nullptr;
    std::vector<ChunkHandle> _handles;

    CHIME_DISALLOW_COPY_AND_ASSIGN(AllocationRegion);
  };

  class RegionManager {
   public:
    RegionManager() = default;
    ~RegionManager() = default;

    void AddAllocationRegion(void *ptr, size_t mem_size) {
      auto entry =
          std::upper_bound(_regions.begin(), _regions.end(), ptr, &Comparator);
      _regions.insert(entry, AllocationRegion(ptr, mem_size));
    }

    AllocationRegion *AddOrExtendAllocationRegion(void *ptr, size_t mem_size) {
      auto entry =
          std::upper_bound(_regions.begin(), _regions.end(), ptr, &Comparator);
      if (entry != _regions.begin()) {
        auto preceding_region = entry - 1;
        if (preceding_region->end_ptr() == ptr) {
          preceding_region->Extend(mem_size);
          return &(*preceding_region);
        }
      }
      _regions.insert(entry, AllocationRegion(ptr, mem_size));
      return nullptr;
    }

    std::vector<AllocationRegion>::iterator RemoveAllocationRegion(
        std::vector<AllocationRegion>::iterator it) {
      return _regions.erase(it);
    }

    ChunkHandle GetHandle(const void *ptr) const {
      return RegionFor(ptr)->GetHandle(ptr);
    }

    void SetHandle(const void *ptr, ChunkHandle h) {
      MutableRegionFor(ptr)->SetHandle(ptr, h);
    }

    const std::vector<AllocationRegion> &Regions() const { return _regions; }

   private:
    static bool Comparator(const void *ptr, const AllocationRegion &other) {
      return ptr < other.ptr();
    }

    AllocationRegion *MutableRegionFor(const void *p) {
      return const_cast<AllocationRegion *>(RegionFor(p));
    }

    const AllocationRegion *RegionFor(const void *p) const {
      auto entry =
          std::upper_bound(_regions.begin(), _regions.end(), p, &Comparator);

      if (entry != _regions.end()) {
        return &(*entry);
      }
      LOG(FATAL) << "Could not find region for pointer " << p;
      return nullptr;
    }

    std::vector<AllocationRegion> _regions;
  };

  static size_t RoundedBytes(size_t bytes);

  bool Extend(size_t alignment, size_t rounded_bytes)
      CHIME_EXCLUSIVE_LOCKS_REQUIRED(_mu);

  bool DeallocateFreeRegions(size_t rounded_bytes);

  void DeallocateRegions(const std::set<void *> &region_ptrs)
      CHIME_EXCLUSIVE_LOCKS_REQUIRED(_mu);

  void *FindChunkPtr(BinNum bin_num, size_t rounded_bytes, size_t num_btyes,
                     uint64_t freed_before) CHIME_EXCLUSIVE_LOCKS_REQUIRED(_mu);

  void SplitChunk(ChunkHandle h, size_t num_bytes)
      CHIME_EXCLUSIVE_LOCKS_REQUIRED(_mu);

  void Merge(ChunkHandle h1, ChunkHandle h2)
      CHIME_EXCLUSIVE_LOCKS_REQUIRED(_mu);

  void InsertFreeChunkIntoBin(ChunkHandle h)
      CHIME_EXCLUSIVE_LOCKS_REQUIRED(_mu);

  void RemoveFreeChunkIterFromBin(Bin::FreeChunkSet *free_chunks,
                                  const Bin::FreeChunkSet::iterator &c)
      CHIME_EXCLUSIVE_LOCKS_REQUIRED(_mu);

  void RemoveFreeChunkFromBin(ChunkHandle h)
      CHIME_EXCLUSIVE_LOCKS_REQUIRED(_mu);

  void MaybeRemoveFreeChunkFromBin(ChunkHandle h)
      CHIME_EXCLUSIVE_LOCKS_REQUIRED(_mu);

  void DeleteChunk(ChunkHandle h) CHIME_EXCLUSIVE_LOCKS_REQUIRED(_mu);

  ChunkHandle AllocateChunk() CHIME_EXCLUSIVE_LOCKS_REQUIRED(_mu);
  void DeallocateChunk(ChunkHandle h) CHIME_EXCLUSIVE_LOCKS_REQUIRED(_mu);

  Chunk *ChunkFromHandle(ChunkHandle h) CHIME_EXCLUSIVE_LOCKS_REQUIRED(_mu);
  const Chunk *ChunkFromHandle(ChunkHandle h) const
      CHIME_EXCLUSIVE_LOCKS_REQUIRED(_mu);

  void MarkFree(ChunkHandle h) CHIME_EXCLUSIVE_LOCKS_REQUIRED(_mu);

  ChunkHandle TryToCoalesce(ChunkHandle h, bool ignore_freed_at)
      CHIME_EXCLUSIVE_LOCKS_REQUIRED(_mu);

  /// Returns floor(log2(n))
  inline int Log2FloorNonZero(uint64_t n) const {
#ifdef __GNUC__
    return 63 ^ __builtin_clzll(n);
#else
    return Log2FloorNonZeroSlow(n);
#endif
  }

  inline int Log2FloorNonZeroSlow(uint64_t n) const {
    int r = 0;
    while (n > 0) {
      r++;
      n >>= 1;
    }
    return r - 1;
  }

 private:
  Bin *BinFromIndex(BinNum index) {
    return reinterpret_cast<Bin *>(_bins_space[sizeof(Bin) * index]);
  }

  size_t BinNumToSize(BinNum index) const {
    return static_cast<size_t>(256) << index;
  }

  BinNum BinNumForSize(size_t bytes) {
    uint64_t v = std::max<size_t>(bytes, 256) >> MIN_ALLOCATION_BITS;
    int b = std::min(NUM_BINS - 1, Log2FloorNonZero(v));
    return b;
  }

  mutable mutex _mu;

  size_t _memory_limit = 0;
  char _bins_space[sizeof(Bin) * NUM_BINS];
  const Options _opts;

  size_t _curr_region_allocation_bytes;
  size_t _total_region_allocated_bytes = 0;

  bool _start_backpedal = false;

  const bool _coalesce_regions;

  std::unique_ptr<SubAllocator> _sub_allocator;
  string _name;

  RegionManager _region_manager CHIME_GUARDED_BY(_mu);

  std::vector<Chunk> _chunks CHIME_GUARDED_BY(_mu);

  ChunkHandle _free_chunks_list CHIME_GUARDED_BY(_mu);

  int64_t _next_allocation_id CHIME_GUARDED_BY(_mu);

  AllocatorStats _stats CHIME_GUARDED_BY(_mu);

  CHIME_DISALLOW_COPY_AND_ASSIGN(BFCAllocator);
};

}  // namespace memory

}  // namespace chime

#endif  // CHIME_CORE_RUNTIME_BFC_ALLOCATOR_H_
