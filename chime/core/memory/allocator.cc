// Copyright by 2022.7 chime
// author: yatorho

#include "chime/core/memory/allocator.h"

#include "chime/core/memory/allocator_registry.h"
#include "chime/core/memory/tracking_allocator.h"

namespace chime {
namespace memory {

constexpr size_t Allocator::ALLOCATOR_ALIGNMENT;

static bool cpu_allocator_collect_full_stats = false;

void enable_cpu_allocator_full_stats() {
  cpu_allocator_collect_full_stats = true;
}

bool cpu_allocator_full_stats_enabled() {
  return cpu_allocator_collect_full_stats;
}

Allocator *cpu_allocator_base() {
  static Allocator *cpu_alloc =
      AllocatorFactoryRegistry::singleton()->get_allocator();
  if (cpu_allocator_collect_full_stats &&
      !cpu_alloc->tracks_allocation_sizes()) {
    cpu_alloc = new TrackingAllocator(cpu_alloc, true);
  }
  return cpu_alloc;
}

Allocator *cpu_allocator(int numa_node) {
  static ProcessStateInterface *ps =
      AllocatorFactoryRegistry::singleton()->process_state();
  if (ps) {
    return ps->get_cpu_allocator(numa_node);
  } else {
    return cpu_allocator_base();
  }
}

Allocator::~Allocator() {}

SubAllocator::SubAllocator(const std::vector<Visitor> &alloc_visitors,
                           const std::vector<Visitor> &free_visitors)
    : _alloc_visitors(alloc_visitors), _free_visitors(free_visitors) {}

void SubAllocator::visit_alloc(void *ptr, int64_t index, size_t size) {
  for (auto &visitor : _alloc_visitors) {
    visitor(ptr, index, size);
  }
}

void SubAllocator::visit_free(void *ptr, int64_t index, size_t size) {
  /// Although we don't guarantee any order of visitor application, strive
  /// to apply free visitors in reverse order of alloc visitors.
  for (int i = _free_visitors.size() - 1; i >= 0; --i) {
    _free_visitors[i](ptr, index, size);
  }
}

}  // namespace memory
}  // namespace chime
