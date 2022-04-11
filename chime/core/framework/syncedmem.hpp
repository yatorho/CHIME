// Copyright by 2022.4 chime
// author: yatorho

#ifndef CHIME_CORE_FRAMEWORK_SYNCEDMEM_HPP_
#define CHIME_CORE_FRAMEWORK_SYNCEDMEM_HPP_

#include "common.hpp"
#include <cstdint>

namespace chime {

inline void chime_malloc_host(void **ptr, mems_t size) {
  *ptr = malloc(static_cast<mems_t>(size));
  DCHECK(*ptr) << "Host allocation of size " << size << " failed. ";
}

inline void chime_free_host(void *ptr) { free(ptr); }

class SyncedMemory {
public:
  enum SyncedHead { UNINITIALIZED, HEAD_AT_CPU, HEAD_AT_GPU, SYNCED };

  explicit SyncedMemory(mems_t size = 0ul);
  
  ~SyncedMemory();

  inline mems_t size() { return _size; }

  inline SyncedHead head() { return _head; }

  inline bool own_cpu_data() { return _own_cpu_data; }

  inline bool own_gpu_data() { return _own_gpu_data; }

  const void *cpu_mem();
  
  void *mutable_cpu_mem();

  void set_cpu_mem(void *ptr);

private:

  void host_malloc(bool init_set = true);

  void *_cpu_ptr;
  mems_t _size;
  SyncedHead _head;
  bool _own_cpu_data;
  bool _own_gpu_data;

  DISABLE_COPY_AND_ASSIGN(SyncedMemory);
};

} // namespace chime

#endif // CHIME_CORE_FRAMEWORK_SYNCEDMEM_HPP_
