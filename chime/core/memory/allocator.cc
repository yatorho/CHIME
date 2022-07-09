// Copyright by 2022.7 chime
// author: yatorho

#include "chime/core/memory/allocator.h"

#include "chime/core/memory/allocator_registry.h"

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
    // cpu_alloc = new
  }
  return cpu_alloc;
}

Allocator::~Allocator() {}

}  // namespace memory
}  // namespace chime
