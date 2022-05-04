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

DefaultAllocator default_allocator();

MemoryOptimizer::~MemoryOptimizer() {}

void MemoryOptimizer::malloc(void **ptr, mems_t size, MallocType type) {
  NOT_IMPLEMENTED;
}

void MemoryOptimizer::free(void *ptr, FreeType type) { NOT_IMPLEMENTED; }

void DefaultAllocator::malloc(void **ptr, mems_t size, MallocType type) {
  *ptr = std::malloc(size);
  DCHECK(*ptr);
}

void DefaultAllocator::free(void *ptr, FreeType type) { std::free(ptr); }

} // namespace memory
} // namespace chime