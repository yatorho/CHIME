// Copyright by 2022.4 chime
// author: yatorho

#include "chime/core/memory/memory_pool.hpp"

namespace chime {

ChimeMemoryPool::ChimeMemoryPool(mems_t size) : _pool_size(size) {}

mems_t ChimeMemoryPool::pool_size() const {
  return _pool_size;
}

ChimeMemoryPool::~ChimeMemoryPool() {
  if (_head) free(_head);
}

} // namespace chime