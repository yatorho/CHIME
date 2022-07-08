// Copyright by 2022.7 chime
// author: yatorho

#include "chime/core/memory/allocator.h"

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



Allocator::~Allocator() {}

}  // namespace memory
}  // namespace chime
