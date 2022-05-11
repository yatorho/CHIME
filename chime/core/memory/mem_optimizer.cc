// Copyright by 2022.4 chime
// author: yatorho

#include "chime/core/memory/mem_optimizer.h"

#include <cstdlib>

namespace chime {
namespace memory {

static bool _use_default_allocator = true;

void use_default_allocator() { _use_default_allocator = true; }
void unuse_default_allocator() { _use_default_allocator = false; }

bool is_use_default_allocator() { return _use_default_allocator; }

MemoryOptimizer::~MemoryOptimizer() {}

void MemoryOptimizer::malloc(void **ptr, mems_t size, MallocType type) {
  NOT_IMPLEMENTED;
}

void MemoryOptimizer::free(void *ptr, FreeType type) { 
  NOT_IMPLEMENTED; }

void DefaultAllocator::malloc(void **ptr, mems_t size, MallocType type) {
  switch (type) {
    case MemoryOptimizer::MALLOC_FROM_HOST_MEMORY: {
      *ptr = std::malloc(size);
      DCHECK(*ptr);
      break;
    }
    default: NOT_IMPLEMENTED;
  }
}

void DefaultAllocator::free(void *ptr, FreeType type) {
  switch (type) {
    case MemoryOptimizer::FREE_FROM_HOST_MEMORY: {
      std::free(ptr);
      break;
    }
    default: NOT_IMPLEMENTED;
  }
}
void DefaultAllocator::memcpy(void *dst, const void *src, mems_t size,
                              MemoryOptimizer::CopyType type) {
  switch (type) {
    case MemoryOptimizer::COPY_FROM_HOST_MEMORY: {
      std::memcpy(dst, src, size);
      break;
    }
    default: NOT_IMPLEMENTED;
  }
}

void MemoryOptimizer::memcpy(void *dst, const void *src, mems_t size,
                             MemoryOptimizer::CopyType type) {
  NOT_IMPLEMENTED;
}

DefaultAllocator default_allocator;

} // namespace memory
} // namespace chime