// Copyright by 2022.4 chime
// author: yatorho

#include "chime/core/memory/mem_optimizer.h"

namespace chime {
namespace memory {

MemoryOptimizer::~MemoryOptimizer() {}

void MemoryOptimizer::malloc(void **ptr, mems_t size, MallocType type) {
  NOT_IMPLEMENTED;
}

void MemoryOptimizer::free(void *ptr, FreeType type) { NOT_IMPLEMENTED; }

} // namespace memory
} // namespace chime