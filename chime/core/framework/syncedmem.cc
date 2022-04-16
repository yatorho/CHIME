// Copyright by 2022.4 chime
// author: yatorho

#include "chime/core/framework/syncedmem.hpp"

namespace chime {

SyncedMemory::SyncedMemory(mems_t size)
    : _cpu_ptr(nullptr),
      _size(size),
      _head(UNINITIALIZED),
      _own_cpu_data(false),
      _own_gpu_data(false) {}

SyncedMemory::~SyncedMemory() {
  if (_cpu_ptr) { chime_free_host(_cpu_ptr); }
}

const void *SyncedMemory::cpu_mem() {
  host_malloc(true);
  return const_cast<const void *>(_cpu_ptr);
}

void *SyncedMemory::mutable_cpu_mem() {
  host_malloc(true);
  return _cpu_ptr;
}

void SyncedMemory::set_cpu_mem(void *ptr) {
  DCHECK(ptr);
  if (_own_cpu_data) chime_free_host(_cpu_ptr);
  _cpu_ptr = static_cast<void *>(ptr);
  _head = HEAD_AT_CPU;
  _own_cpu_data = false;
}

void SyncedMemory::host_malloc(bool init_set) {
  switch (_head) {
    case UNINITIALIZED:
      chime_malloc_host(&_cpu_ptr, _size);
      if (init_set) std::memset(_cpu_ptr, 0, _size);
      _head = HEAD_AT_CPU;
      _own_cpu_data = true;
    case HEAD_AT_CPU:
    case HEAD_AT_GPU:
    case SYNCED: break;
  }
}

} // namespace chime
