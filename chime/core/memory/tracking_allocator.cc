// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

#include "chime/core/memory/tracking_allocator.h"

#include <vector>

#include "chime/core/platform/env.hpp"
#include "chime/core/platform/mutex.h"
#include "chime/core/util/optional.hpp"

namespace chime {
namespace memory {

TrackingAllocator::TrackingAllocator(Allocator *allocator, bool track_ids)
    : _allocator(allocator),
      _ref(1),
      _allocated(size_t(0)),
      _high_watermark(size_t(0)),
      _total_bytes(size_t(0)),
      _track_sizes_locally(track_ids && !allocator->TracksAllocationSizes()),
      _next_id(int64_t(0)) {}

void *TrackingAllocator::AllocateRow(size_t alignment, size_t num_bytes,
                                      const AllocationAttributes &attributes) {
  void *ptr = _allocator->AllocateRow(alignment, num_bytes, attributes);
  if (ptr == nullptr) return nullptr;
  if (_allocator->TracksAllocationSizes()) {
    size_t allocated_bytes = _allocator->AllocatedSize(ptr);
    mutex_lock lock(_mutex);
    _allocated += allocated_bytes;
    _high_watermark = std::max(_high_watermark, _allocated);
    _total_bytes += allocated_bytes;
    _allocations.emplace_back(allocated_bytes,
                              platform::Env::Default()->NowMicros());
    ++_ref;
  } else if (_track_sizes_locally) {
    // Call the underlying allocator to try to get the allocated size
    // whenever possible, even when it might be slow. If this fails,
    // use the requested size as an approximation.
    size_t allocated_bytes = _allocator->AllocatedSizeSlow(ptr);
    allocated_bytes = std::max(num_bytes, allocated_bytes);
    mutex_lock lock(_mutex);
    _next_id += 1;
    Chunk chunk = {num_bytes, allocated_bytes, _next_id};
    _in_use.emplace(std::make_pair(ptr, chunk));
    _allocated += allocated_bytes;
    _high_watermark = std::max(_high_watermark, _allocated);
    _total_bytes += allocated_bytes;
    _allocations.emplace_back(allocated_bytes,
                              platform::Env::Default()->NowMicros());
    ++_ref;
  } else {
    mutex_lock lock(_mutex);
    _total_bytes += num_bytes;
    _allocations.emplace_back(num_bytes,
                              platform::Env::Default()->NowMicros());
    ++_ref;
  }
  return ptr;
}

void TrackingAllocator::DeallocateRow(void *ptr) {
  if (ptr == nullptr) return;

  bool should_delete;

  if (_allocator->TracksAllocationSizes()) {
    size_t allocated_bytes = _allocator->AllocatedSize(ptr);
    mutex_lock lock(_mutex);
    CHECK_GE(_allocated, allocated_bytes);
    _allocated -= allocated_bytes;
    _allocations.emplace_back(-allocated_bytes,
                              platform::Env::Default()->NowMicros());
    should_delete = un_ref();
  } else if (_track_sizes_locally) {
    mutex_lock lock(_mutex);
    auto itr = _in_use.find(ptr);
    if (itr != _in_use.end()) {
      size_t allocated_bytes = itr->second.allocated_size;
      _in_use.erase(itr);
      _allocated -= allocated_bytes;
      _allocations.emplace_back(-allocated_bytes,
                                platform::Env::Default()->NowMicros());
      should_delete = un_ref();
    }
  } else {
    mutex_lock lock(_mutex);
    should_delete = un_ref();
  }
  _allocator->DeallocateRow(ptr);
  if (should_delete) delete this;
}

bool TrackingAllocator::TracksAllocationSizes() const {
  return _allocator->TracksAllocationSizes() || _track_sizes_locally;
}

size_t TrackingAllocator::RequestedSize(const void *ptr) const {
  if (_track_sizes_locally) {
    mutex_lock lock(_mutex);
    auto itr = _in_use.find(ptr);
    if (itr != _in_use.end()) {
      return itr->second.requested_size;
    }
    return 0;
  }
  return _allocator->RequestedSize(ptr);
}

size_t TrackingAllocator::AllocatedSize(const void *ptr) const {
  if (_track_sizes_locally) {
    mutex_lock lock(_mutex);
    auto itr = _in_use.find(ptr);
    if (itr != _in_use.end()) {
      return itr->second.allocated_size;
    }
    return 0;
  }
  return _allocator->AllocatedSize(ptr);
}

int64_t TrackingAllocator::AllocationID(const void *ptr) const {
  if (_track_sizes_locally) {
    mutex_lock lock(_mutex);
    auto itr = _in_use.find(ptr);
    if (itr != _in_use.end()) {
      return itr->second.allocation_id;
    }
    return 0;
  }
  return _allocator->AllocationID(ptr);
}

util::Optional<AllocatorStats> TrackingAllocator::GetStats() {
  return _allocator->GetStats();
}

bool TrackingAllocator::ClearStats() { return _allocator->ClearStats(); }

std::tuple<size_t, size_t, size_t> TrackingAllocator::GetSizes() const {
  size_t high_watermark;
  size_t total_bytes;
  size_t allocated_bytes;
  {
    mutex_lock lock(_mutex);
    high_watermark = _high_watermark;
    total_bytes = _total_bytes;
    allocated_bytes = _allocated;
  }
  return std::make_tuple(high_watermark, total_bytes, allocated_bytes);
}

bool TrackingAllocator::un_ref() {
  CHECK_GE(_ref, 1);
  --_ref;
  return _ref == 0;
}

std::vector<AllocRecord> TrackingAllocator::GetRecordsAndUnref() {
  bool should_delete;
  std::vector<AllocRecord> records;
  {
    mutex_lock lock(_mutex);
    records.swap(_allocations);
    should_delete = un_ref();
  }
  if (should_delete) delete this;
  return records;
}
std::vector<AllocRecord> TrackingAllocator::GetCurrentRecords() const {
  std::vector<AllocRecord> records;
  {
    mutex_lock lock(_mutex);
    for (const AllocRecord &record : _allocations) {
      records.push_back(record);
    }
  }
  return records;
}

}  // namespace memory
}  // namespace chime
