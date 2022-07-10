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
      _track_sizes_locally(track_ids && !allocator->tracks_allocation_sizes()),
      _next_id(int64_t(0)) {}

void *TrackingAllocator::allocate_row(size_t alignment, size_t num_bytes,
                                      const AllocationAttributes &attributes) {
  void *ptr = _allocator->allocate_row(alignment, num_bytes, attributes);
  if (ptr == nullptr) return nullptr;
  if (_allocator->tracks_allocation_sizes()) {
    size_t allocated_bytes = _allocator->allocated_size(ptr);
    mutex_lock lock(_mutex);
    _allocated += allocated_bytes;
    _high_watermark = std::max(_high_watermark, _allocated);
    _total_bytes += allocated_bytes;
    _allocations.emplace_back(allocated_bytes,
                              platform::Env::Default()->now_micros());
    ++_ref;
  } else if (_track_sizes_locally) {
    // Call the underlying allocator to try to get the allocated size
    // whenever possible, even when it might be slow. If this fails,
    // use the requested size as an approximation.
    size_t allocated_bytes = _allocator->allocated_size_slow(ptr);
    allocated_bytes = std::max(num_bytes, allocated_bytes);
    mutex_lock lock(_mutex);
    _next_id += 1;
    Chunk chunk = {num_bytes, allocated_bytes, _next_id};
    _in_use.emplace(std::make_pair(ptr, chunk));
    _allocated += allocated_bytes;
    _high_watermark = std::max(_high_watermark, _allocated);
    _total_bytes += allocated_bytes;
    _allocations.emplace_back(allocated_bytes,
                              platform::Env::Default()->now_micros());
    ++_ref;
  } else {
    mutex_lock lock(_mutex);
    _total_bytes += num_bytes;
    _allocations.emplace_back(num_bytes,
                              platform::Env::Default()->now_micros());
    ++_ref;
  }
  return ptr;
}

void TrackingAllocator::deallocate_row(void *ptr) {
  if (ptr == nullptr) return;

  bool should_delete;

  if (_allocator->tracks_allocation_sizes()) {
    size_t allocated_bytes = _allocator->allocated_size(ptr);
    mutex_lock lock(_mutex);
    CHECK_GE(_allocated, allocated_bytes);
    _allocated -= allocated_bytes;
    _allocations.emplace_back(-allocated_bytes,
                              platform::Env::Default()->now_micros());
    should_delete = un_ref();
  } else if (_track_sizes_locally) {
    mutex_lock lock(_mutex);
    auto itr = _in_use.find(ptr);
    if (itr != _in_use.end()) {
      size_t allocated_bytes = itr->second.allocated_size;
      _in_use.erase(itr);
      _allocated -= allocated_bytes;
      _allocations.emplace_back(-allocated_bytes,
                                platform::Env::Default()->now_micros());
      should_delete = un_ref();
    }
  } else {
    mutex_lock lock(_mutex);
    should_delete = un_ref();
  }
  _allocator->deallocate_row(ptr);
  if (should_delete) delete this;
}

bool TrackingAllocator::tracks_allocation_sizes() const {
  return _allocator->tracks_allocation_sizes() || _track_sizes_locally;
}

size_t TrackingAllocator::requested_size(const void *ptr) const {
  if (_track_sizes_locally) {
    mutex_lock lock(_mutex);
    auto itr = _in_use.find(ptr);
    if (itr != _in_use.end()) {
      return itr->second.requested_size;
    }
    return 0;
  }
  return _allocator->requested_size(ptr);
}

size_t TrackingAllocator::allocated_size(const void *ptr) const {
  if (_track_sizes_locally) {
    mutex_lock lock(_mutex);
    auto itr = _in_use.find(ptr);
    if (itr != _in_use.end()) {
      return itr->second.allocated_size;
    }
    return 0;
  }
  return _allocator->allocated_size(ptr);
}

int64_t TrackingAllocator::allocation_id(const void *ptr) const {
  if (_track_sizes_locally) {
    mutex_lock lock(_mutex);
    auto itr = _in_use.find(ptr);
    if (itr != _in_use.end()) {
      return itr->second.allocation_id;
    }
    return 0;
  }
  return _allocator->allocation_id(ptr);
}

util::Optional<AllocatorStats> TrackingAllocator::get_stats() {
  return _allocator->get_stats();
}

bool TrackingAllocator::clear_stats() { return _allocator->clear_stats(); }

std::tuple<size_t, size_t, size_t> TrackingAllocator::get_sizes() const {
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

std::vector<AllocRecord> TrackingAllocator::get_records_and_unref() {
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
std::vector<AllocRecord> TrackingAllocator::get_current_records() const {
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
