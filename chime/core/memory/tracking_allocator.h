// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

#ifndef CHIME_CORE_MEMORY_TRACKING_ALLOCATOR_H_
#define CHIME_CORE_MEMORY_TRACKING_ALLOCATOR_H_

#include <cstdint>
#include <tuple>
#include <unordered_map>

#include "chime/core/memory/allocator.h"
#include "chime/core/platform/mutex.h"
#include "chime/core/platform/thread_annotations.h"
#include "chime/core/util/optional.hpp"

namespace chime {
namespace memory {

///
struct AllocRecord {
  AllocRecord(int64_t a_bytes, int64_t a_micros)
      : alloc_bytes(a_bytes), alloc_micros(a_micros) {}

  AllocRecord() : alloc_bytes(0), alloc_micros(0) {}

  int64_t alloc_bytes;
  int64_t alloc_micros;
};

class TrackingAllocator : public Allocator {
 public:
  explicit TrackingAllocator(Allocator *allocator, bool track_ids);

  std::string Name() override { return _allocator->Name(); }

  void *AllocateRaw(size_t alignment, size_t num_bytes) override {
    return AllocateRaw(alignment, num_bytes, AllocationAttributes());
  }
  void *AllocateRaw(size_t alignment, size_t num_bytes,
                     const AllocationAttributes &attributes) override;
  void DeallocateRaw(void *ptr) override;

  bool TracksAllocationSizes() const override;
  size_t RequestedSize(const void *ptr) const override;
  size_t AllocatedSize(const void *ptr) const override;
  int64_t AllocationID(const void *ptr) const override;
  util::Optional<AllocatorStats> GetStats() override;
  bool ClearStats() override;

  std::tuple<size_t, size_t, size_t> GetSizes() const;

  AllocatorMemoryType GetMemoryType() const override {
    return _allocator->GetMemoryType();
  }

  /// After get_records_and_unref is called, the only further calls allowed
  /// on this wrapper are calls to DeallocateRow with pointers that
  /// were allocated by this wrapper and have not yet been
  /// deallocated. After this call completes and all allocated pointers
  /// have been deallocated the wrapper will delete itself.
  std::vector<AllocRecord> GetRecordsAndUnref();
  /// Returns a copy of allocations records collected so far.
  std::vector<AllocRecord> GetCurrentRecords() const;

 protected:
  ~TrackingAllocator() override {};

 private:
  bool un_ref() CHIME_EXCLUSIVE_LOCKS_REQUIRED(_mutex);
  Allocator *_allocator;  // not owned
  mutable mutex _mutex;

  /// The current number of calls to `AllocateRow()` that have not yet been
  /// matched by a corresponding call to `DeallocateRow()`.
  int64_t _ref CHIME_GUARDED_BY(_mutex);
  /// the current number of outstanding bytes that have been allocated
  /// by this wrapper, or 0 if the underlying allocator does not track
  /// allocation sizes.
  size_t _allocated CHIME_GUARDED_BY(_mutex);
  /// the maximum number of outstanding bytes that have been allocated
  /// by this wrapper, or 0 if the underlying allocator does not track
  /// allocation sizes.
  size_t _high_watermark CHIME_GUARDED_BY(_mutex);
  /// the total number of bytes that have been allocated by this
  /// wrapper if the underlying allocator tracks allocation sizes,
  /// otherwise the total number of bytes that have been requested by
  /// this allocator.
  size_t _total_bytes CHIME_GUARDED_BY(_mutex);

  std::vector<AllocRecord> _allocations CHIME_GUARDED_BY(_mutex);

  const bool _track_sizes_locally;

  struct Chunk {
    size_t requested_size;
    size_t allocated_size;
    int64_t allocation_id;
  };

  std::unordered_map<const void *, Chunk> _in_use CHIME_GUARDED_BY(_mutex);

  int64_t _next_id CHIME_GUARDED_BY(_mutex);
};

}  // namespace memory
}  // namespace chime

#endif  // CHIME_CORE_MEMORY_TRACKING_ALLOCATOR_H_
