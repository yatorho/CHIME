// Copyright by 2022.7 chime
// author: yatorho

#ifndef CHIME_CORE_MEMORY_MEM_H_
#define CHIME_CORE_MEMORY_MEM_H_

#include <stdlib.h>

namespace chime {
namespace port {

/// Aligned allocation/deallocation. `minimum_alignment` must be a power of 2
/// and a multiple of sizeof(void*).
/// Don't require size to be a multiple of alignment.
/// If size is not a multiple of alignment, we will allocate the next highest
/// multiple of alignment.
/// NOTE: The extra allocated memory shouldn't be used.
void *AlignedMalloc(size_t size, size_t alignment);
void AlignedFree(void *ptr);

void *Malloc(size_t size);
void *Realloc(void *ptr, size_t size);
void Free(void *ptr);

}  // namespace port
}  // namespace chime

#endif  // CHIME_CORE_MEMORY_MEM_H_
