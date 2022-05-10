// Copyright by 2022.4 chime
// author: yatorho

#include "chime/core/framework/syncedmem.hpp"

#include "chime/core/memory/mem_optimizer.h"

namespace chime {

SyncedMemory::SyncedMemory(MemOper &mo, mems_t size)
    : _head(UNINITIALIZED),
      _host_ptr(nullptr),
      _device_ptr(nullptr),
      _size(size),
      _own_host_mem(false),
      _own_device_mem(false),
      _mem_opti(mo) {}

SyncedMemory::SyncedMemory(MemOper &&mo, mems_t size)
    : _head(UNINITIALIZED),
      _host_ptr(nullptr),
      _device_ptr(nullptr),
      _size(size),
      _own_host_mem(false),
      _own_device_mem(false),
      _mem_opti(mo) {}

SyncedMemory::~SyncedMemory() {
  if (_host_ptr) _mem_opti.free(_host_ptr, MemOper::FREE_FROM_HOST_MEMORY);
  if (_device_ptr) _mem_opti.free(_host_ptr, MemOper::FREE_FROM_DEVICE0_MEMORY);
}

const void *SyncedMemory::host_mem() {
  _to_host(true);
  return const_cast<const void *>(_host_ptr);
}

void *SyncedMemory::mutable_host_mem() {
  _to_host(true);
  return _host_ptr;
}

void SyncedMemory::set_host_mem(void *ptr) {
  DCHECK(ptr);
  if (_own_host_mem) _mem_opti.free(_host_ptr, MemOper::FREE_FROM_HOST_MEMORY);
  _host_ptr = static_cast<void *>(ptr);
  _head = HEAD_AT_HOST;
  _own_host_mem = false;
}

void SyncedMemory::_to_host(bool init_set_zero) {
  switch (_head) {
    case UNINITIALIZED:
      _mem_opti.malloc(&_host_ptr, _size, MemOper::MALLOC_FROM_HOST_MEMORY);
      if (init_set_zero) std::memset(_host_ptr, 0, _size);
      _head = HEAD_AT_HOST;
      _own_host_mem = true;
    case HEAD_AT_HOST:
    case HEAD_AT_DEVICE:
    case SYNCED: break;
  }
}

bool SyncedMemory::host_mem_cpy(SyncedMemory &sm) {
  if (!own_host_mem()) return false;
  _mem_opti.memcpy(mutable_host_mem(), sm.host_mem(), _size,
                   MemOper::COPY_FROM_HOST_MEMORY);
  return true;
}
bool SyncedMemory::device_mem_cpy(SyncedMemory &sm, DeviceSupported dname) {
  if (!own_device_mem()) return false;
  NOT_IMPLEMENTED;
}

} // namespace chime
