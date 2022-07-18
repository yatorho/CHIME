// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

#ifndef CHIME_CORE_PLATFORM_NUMA_H_
#define CHIME_CORE_PLATFORM_NUMA_H_

#include <stdlib.h>

namespace chime {
namespace port {

/// Returns true if NUMA is supported on this platform.
bool NUMAEnabled();

/// Returns the number of NUMA nodes present with respect to CPU operations.
/// Typically this will be the number of sockets where some RAM has greater
/// affinity with one socket than another.
int NUMANumNodes();

static const int NUMA_NO_AFFINITY = -1;

void NUMASetThreadNodeAffinity(int node);

int NUMAGetThreadNodeAffinity();

/// Like AlignedMalloc, but allocates memory with affinity to the specified NUMA
/// node.
///
/// Notes:
///  1. node must be >= 0 and < NUMANumNodes.
///  1. minimum_alignment must a factor of system page size, the memory
///     returned will be page-aligned.
///  2. This function is likely significantly slower than AlignedMalloc
///     and should not be used for lots of small allocations.
void *NUMAMalloc(int node, size_t size, int minimum_alignment);

/// Memory allocated by NUMAMalloc must be freed via NUMAFree.
void NUMAFree(void *ptr, size_t size);

/// Returns NUMA node affinity of memory address, kNUMANoAffinity if none.
int NUMAGetMemAffinity(const void *ptr);

}  // namespace port
}  // namespace chime

#endif  // CHIME_CORE_PLATFORM_NUMA_H_
