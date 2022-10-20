// Copyright by 2022.6 chime
// author: yatorho

#ifndef CHIME_CORE_MEMORY_ALLOCATOR_H_
#define CHIME_CORE_MEMORY_ALLOCATOR_H_

#include <stdlib.h>

#include <cstdint>
#include <functional>

#include "chime/core/platform/types.h"
#include "chime/core/platform/logging.hpp"
#include "chime/core/platform/macros.h"
#include "chime/core/platform/numa.h"
#include "chime/core/util/optional.hpp"

namespace chime {
namespace memory {

/// Attributes for a single memory allocation call. Different calls to the same
/// allocator could potentially have different attributes.
struct AllocationAttributes {
  AllocationAttributes() = default;

  AllocationAttributes(bool retry_on_failure, bool allocation_will_be_logged,
                       std::function<uint64()> *freed_by_func)
      : retry_on_failure(retry_on_failure),
        allocation_will_be_logged(allocation_will_be_logged),
        freed_by_func(freed_by_func) {}

  /// If the first attempt to allocate the memory fails, the allocation should
  /// wait and retry (with a timeout).
  ///
  /// This is usually set to true, but we may set it to false in cases where a
  /// failure has only performance impact (e.g. optional scratch space
  /// allocation).
  bool retry_on_failure = false;

  /// If true, the allocation will be logged.
  bool allocation_will_be_logged = false;

  std::function<uint64()> *freed_by_func = nullptr;
  CHIME_DISALLOW_COPY_AND_ASSIGN(AllocationAttributes);
};

struct AllocatorStats {
  int64_t num_allocs;
  int64_t bytes_in_use;
  int64_t peak_bytes_in_use;
  int64_t largest_alloc_size;

  /// The upper limit of bytes of user allocatable device memory, if such a
  /// limit is known.
  util::Optional<int64_t> bytes_limit;

  int64_t bytes_reserved;
  int64_t peak_bytes_reserved;

  util::Optional<int64_t> bytes_reserved_limit;

  int64_t largest_free_block_bytes;  // largest free block's size in heap.

  AllocatorStats()
      : num_allocs(0),
        bytes_in_use(0),
        peak_bytes_in_use(0),
        largest_alloc_size(0),
        bytes_reserved(0),
        peak_bytes_reserved(0),
        largest_free_block_bytes(0) {}

  std::string DebugString() const;
};

enum class AllocatorMemoryType {
  UNKNOWN = 0,
  DEVICE = 1,
  HOST_PAGEABLE = 2,
  HOST_PINNED = 3,
};

class Allocator {
 public:
  /// Align to 64 bytes boundary.
  static constexpr size_t ALLOCATOR_ALIGNMENT = 64;

  virtual ~Allocator();

  virtual std::string Name() = 0;

  /// Return an uninitialized block of memory that is "num_bytes" bytes
  /// in size.  The returned pointer is guaranteed to be aligned to a
  /// multiple of "alignment" bytes.
  /// REQUIRES: "alignment" is a power of 2.
  virtual void *AllocateRaw(size_t alignment, size_t num_btyes) = 0;

  /// Return an uninitialized block of memory that is "num_bytes" bytes
  /// in size with specified allocation attributes.  The returned pointer is
  /// guaranteed to be aligned to a multiple of "alignment" bytes.
  /// REQUIRES: "alignment" is a power of 2.
  virtual void *AllocateRaw(size_t alignment, size_t num_btyes,
                             const AllocationAttributes &allocation_attr) {
    /// The default behavior is to use the implementation without any allocation
    /// attributes.
    return AllocateRaw(alignment, num_btyes);
  }

  /// Deallocate the memory block previously returned by AllocateRaw.
  /// REQUIRES: "ptr" was returned by a call to AllocateRaw.
  virtual void DeallocateRaw(void *ptr) = 0;

  /// Returns true if this allocator tracks the size of allocations.
  ///
  virtual bool TracksAllocationSizes() const { return false; }

  /// Returns the user-requested size of the data allocated at
  /// 'ptr'. Note that the actual buffer allocated might be larger
  /// than requested, but this function returns the size requested by
  /// the user.
  ///
  /// REQUIRES: TracksAllocationSizes() is true.
  ///
  /// REQUIRES: 'ptr!=nullptr' and points to a buffer previously
  /// allocated by this allocator.
  virtual size_t RequestedSize(const void *ptr) const {
    CHECK(false) << "allocator does not track allocation sizes";
    return size_t(0);
  }

  /// Returns the really allocated size buffer pointed by 'ptr' if known,
  /// otherwise returns RequestedSize(ptr). `AllocatedSize(ptr)` is guaranteed
  /// to be >= `RequestedSize(ptr)`.
  ///
  /// REQUIRES: TracksAllocationSizes() is
  /// true. REQUIRES: 'ptr!=nullptr' and points to a buffer previously allocated
  /// by this allocator.
  virtual size_t AllocatedSize(const void *ptr) const {
    return RequestedSize(ptr);
  }

  /// Returns the allocated size of the buffer at 'ptr' if known,
  /// otherwise returns 0. This method can be called when
  /// TracksAllocationSizes() is false, but can be extremely slow.
  ///
  /// REQUIRES: 'ptr!=nullptr' and points to a buffer previously
  /// allocated by this allocator.
  virtual size_t AllocatedSizeSlow(const void *ptr) const {
    if (TracksAllocationSizes()) return AllocatedSize(ptr);
    return 0;
  }

  virtual int64_t AllocationID(const void *ptr) const { return 0; }

  virtual util::Optional<AllocatorStats> GetStats() { return util::nullopt; }

  virtual bool ClearStats() CHIME_MUST_USE_RESULT { return false; }

  virtual AllocatorMemoryType GetMemoryType() const {
    return AllocatorMemoryType::UNKNOWN;
  }
};

class AllocatorWrapper : public Allocator {
 public:
  explicit AllocatorWrapper(Allocator *allocator) : _wrapped(allocator) {}

  ~AllocatorWrapper() override {}

  std::string Name() override { return _wrapped->Name(); }

  Allocator *Wrappe() const { return _wrapped; }

  void *AllocateRaw(size_t alignment, size_t num_btyes) override {
    return _wrapped->AllocateRaw(alignment, num_btyes);
  }

  void *AllocateRaw(size_t alignment, size_t num_btyes,
                     const AllocationAttributes &allocation_attr) override {
    return _wrapped->AllocateRaw(alignment, num_btyes, allocation_attr);
  }

  void DeallocateRaw(void *ptr) override { _wrapped->DeallocateRaw(ptr); }

  bool TracksAllocationSizes() const override {
    return _wrapped->TracksAllocationSizes();
  }

  size_t RequestedSize(const void *ptr) const override {
    return _wrapped->RequestedSize(ptr);
  }

  size_t AllocatedSize(const void *ptr) const override {
    return _wrapped->AllocatedSize(ptr);
  }

  size_t AllocatedSizeSlow(const void *ptr) const override {
    return _wrapped->AllocatedSizeSlow(ptr);
  }

  int64_t AllocationID(const void *ptr) const override {
    return _wrapped->AllocationID(ptr);
  }

  AllocatorMemoryType GetMemoryType() const override {
    return _wrapped->GetMemoryType();
  }

 private:
  Allocator *_wrapped;
};

/// Returns a trivial implementation of Allocator, which is a process singleton.
/// Access through this function is only intended for use by restricted parts
/// of the infrastructure.
Allocator *CPUAllocatorBase();

/// If available, calls ProcessState::get_cpu_allocator(numa_node).
/// If not, falls back to CPUAllocatorBase().
/// Intended for use in contexts where ProcessState is not visible at
/// compile time. Where ProcessState is visible, it's preferable to
/// call it directly.
Allocator *CPUAllocator(int numa_node = port::NUMA_NO_AFFINITY);

void EnableCPUAllocatorStats();

void DisableCPUAllocatorStats();

bool CPUAllocatorStatsEnabled();

void EnableCPUAllocatorTracksAllocationSizes();

void DisableCPUAllocatorTracksAllocationSizes();

bool CPUAllocatorTracksAllocationSizesEnabled();

/// An object that does the underlying suballoc/free of memory for a
/// higher-level allocator.  The expectation is that the higher-level allocator
/// is doing some kind of cache or pool management so that it will call
/// SubAllocator::Alloc and Free relatively infrequently, compared to the number
/// of times its own AllocateRaw and Free methods are called.
class SubAllocator {
 public:
  /// Visitor gets called with a pointer to a memory area and its
  /// size in bytes. The index value will be numa_node for a CPU
  /// allocator and GPU id for a GPU allocator.
  typedef std::function<void(void *, int64_t index, size_t)> Visitor;

  SubAllocator(const std::vector<Visitor> &alloc_visitors,
               const std::vector<Visitor> &free_visitors);

  virtual ~SubAllocator() {}

  /// Allocates at least `num_bytes`. Returns actual number of bytes allocated
  /// in `bytes_allocated`. The caller can safely use the full `bytes_reverved`
  /// sized buffer following the returned pointer.
  virtual void *Alloc(size_t alignment, size_t num_bytes,
                      size_t *bytes_reserved) = 0;
  virtual void Free(void *ptr, size_t num_bytes) = 0;

  virtual AllocatorMemoryType GetMemoryType() const {
    return AllocatorMemoryType::UNKNOWN;
  }

 protected:
  /// Implementation of `Alloc()` method must call this on newly allocated
  /// value.
  void VisitAlloc(void *ptr, int64_t index, size_t num_bytes);

  /// Implementation of `Free()` method must call this on value to be freed
  /// immediately before deallocation.
  void VisitFree(void *ptr, int64_t index, size_t num_bytes);

  const std::vector<Visitor> _alloc_visitors;
  const std::vector<Visitor> _free_visitors;
};

}  // namespace memory
}  // namespace chime

#endif  // CHIME_CORE_MEMORY_ALLOCATOR_H_
