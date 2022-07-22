// Copyright by 2022.7 chime
// author: yatorho

#include "chime/core/memory/allocator.h"

#include "chime/core/memory/allocator_registry.h"
#include "chime/core/memory/tracking_allocator.h"

namespace chime {
namespace memory {

constexpr size_t Allocator::ALLOCATOR_ALIGNMENT;

static bool cpu_allocator_tracks_allocation_sizes = false;

void EnableCPUAllocatorTracksAllocationSizes() {
  cpu_allocator_tracks_allocation_sizes = true;
}

void DisableCPUAllocatorTracksAllocationSizes() {
  cpu_allocator_tracks_allocation_sizes = false;
}

bool CPUAllocatorTracksAllocationSizesEnabled() {
  return cpu_allocator_tracks_allocation_sizes;
}

Allocator *CPUAllocatorBase() {
  static Allocator *cpu_alloc =
      AllocatorFactoryRegistry::Singleton()->GetAllocator();
  if (cpu_allocator_tracks_allocation_sizes &&
      !cpu_alloc->TracksAllocationSizes()) {
    cpu_alloc = new TrackingAllocator(cpu_alloc, true);
  }
  return cpu_alloc;
}

Allocator *CPUAllocator(int numa_node) {
  static ProcessStateInterface *ps =
      AllocatorFactoryRegistry::Singleton()->ProcessState();
  if (ps) {
    return ps->GetCPUAllocator(numa_node);
  } else {
    return CPUAllocatorBase();
  }
}

Allocator::~Allocator() {}

SubAllocator::SubAllocator(const std::vector<Visitor> &alloc_visitors,
                           const std::vector<Visitor> &free_visitors)
    : _alloc_visitors(alloc_visitors), _free_visitors(free_visitors) {}

void SubAllocator::VisitAlloc(void *ptr, int64_t index, size_t size) {
  for (auto &visitor : _alloc_visitors) {
    visitor(ptr, index, size);
  }
}

void SubAllocator::VisitFree(void *ptr, int64_t index, size_t size) {
  /// Although we don't guarantee any order of visitor application, strive
  /// to apply free visitors in reverse order of alloc visitors.
  for (int i = _free_visitors.size() - 1; i >= 0; --i) {
    _free_visitors[i](ptr, index, size);
  }
}

}  // namespace memory
}  // namespace chime
