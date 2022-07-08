// Copyright by 2022.7 chime
// author: yatorho

#include "chime/core/memory/mem.h"

namespace chime {
namespace port {

// Aligned allocation/deallocation. `minimum_alignment` must be a power of 2
// Don't require `size` to be a multiple of `alignment`.
void *AlignedMalloc(size_t size, size_t alignment) {
  if (size % alignment != 0) size += alignment - (size % alignment);
  void *ptr = nullptr;
  if (posix_memalign(&ptr, alignment, size) != 0) return nullptr;
  return ptr;
}

void AlignedFree(void *ptr) { free(ptr); }

void *Malloc(size_t size) { return malloc(size); }

void *Realloc(void *ptr, size_t size) { return realloc(ptr, size); }

void Free(void *ptr) { free(ptr); }

}  // namespace port
}  // namespace chime
