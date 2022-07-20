// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

#include "chime/core/memory/allocator.h"

namespace chime {
namespace memory {

class CPUAllocator;
class SubCPUAllocator;

static bool cpu_allocator_collect_full_stats = false;

void enable_cpu_allocator_stats() { cpu_allocator_collect_full_stats = true; }

void disable_cpu_allocator_stats() { cpu_allocator_collect_full_stats = false; }

bool cpu_allocator_stats_enables() { return cpu_allocator_collect_full_stats; }

class CPUAllocator : public Allocator {};

}  // namespace memory
}  // namespace chime
