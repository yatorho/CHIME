// Copyright by 2022.4 chime
// author: yatorho

#ifndef CHIME_CORE_FRAMEWORK_SYNCEDMEM_HPP_
#define CHIME_CORE_FRAMEWORK_SYNCEDMEM_HPP_

#include <cstdint>

#include "chime/core/framework/common.hpp"
#include "chime/core/memory/mem_optimizer.h"

#if MemoryOptimizationOption == ChimeMemoryPool
#define chime_malloc_host(ptr, size, mp) _chime_malloc_host(ptr, size, mp)
#define chime_malloc_device(ptr, size, mp) _chime_malloc_
#define chime_free_host(ptr, mp) _chime_free_host(ptr, mp)
#elif MemoryOptimizationOption == ChimeAllocator
#define chime_cpu_malloc(ptr, size, mp)
#elif MemoryOptimizationOption == NoOptimization
#define chime_cpu_malloc(ptr, size, kwargs) _chime_malloc_host(ptr, size)
#define chime_free_host(ptr) _chime_free_host(ptr)
#endif

namespace chime {

class SyncedMemory {
 public:
  typedef memory::MemoryOptimizer MemOper;
  enum SyncedHead {
    UNINITIALIZED = 0,
    HEAD_AT_HOST = 1,
    HEAD_AT_DEVICE = 2,
    SYNCED = 3
  };

  inline void check_is_legality() { DCHECK(_size); };

  explicit SyncedMemory(MemOper &mo, mems_t size = 0ul);

  ~SyncedMemory();

  inline mems_t size() const { return _size; }

  inline SyncedHead head() const { return _head; }

  inline bool own_host_mem() const { return _own_host_mem; }

  inline bool own_device_mem() const { return _own_device_mem; }

  const void *host_mem();

  const void *device_mem(DeviceSupported dname = GRAPHICS_PROCESSING_UNIT);

  void *mutable_host_mem();

  void *mutable_device_mem(DeviceSupported dname = GRAPHICS_PROCESSING_UNIT);

  void set_host_mem(void *ptr);

  void host_mem_cpy(SyncedMemory &sm);

  void device_mem_cpy(SyncedMemory &sm,
                      DeviceSupported dname = GRAPHICS_PROCESSING_UNIT);

 private:
  void _to_host(bool init_set_zero = true);

  void _to_device(bool init_set_zero = true,
                  DeviceSupported dname = GRAPHICS_PROCESSING_UNIT);

  SyncedHead _head;

  void *_host_ptr;
  void *_device_ptr;
  mems_t _size;
  bool _own_host_mem;
  bool _own_device_mem;

  MemOper &_mem_opti;
  DISABLE_COPY_AND_ASSIGN(SyncedMemory);
};

}  // namespace chime

#endif  // CHIME_CORE_FRAMEWORK_SYNCEDMEM_HPP_
