// Copyright by 2022.4 chime
// author: yatorho

#ifndef CHIME_CORE_MEMORY_CHIME_MP_HPP_
#define CHIME_CORE_MEMORY_CHIME_MP_HPP_

#include "chime/core/framework/common.hpp"

#if defined(__WIN32__)

typedef CRITICAL_SECTION MUTEXTYPE
#define init_mutex(hMutex) InitializeCriticalSection(&(hMutex))
#define delete_mutex(hMutex) DeleteCriticalSection(&(hMutex))
#define lock(hMutex) EnterCriticalSection(&(hMutex))
#define unlock(hMutex) LeaveCriticalSection(&(hMutex))

#elif defined(__linux__)

#include <pthread.h>

typedef pthread_mutex_t MUTEXTYPE;
#define init_mutex(hMutex) pthread_mutex_init(&(hMutex), NULL)
#define delete_mutex(hMutex) pthread_mutex_destroy(&(hMutex))
#define lock(hMutex) pthread_mutex_lock(&(hMutex))
#define unlock(hMutex) pthread_mutex_unlock(&(hMutex))

#endif // defined(__WIN32__)

  namespace chime {
} // namespace chime

namespace chime {
namespace memory {

typedef enum {
  CPU_MEMORY_TYPE = 0,
  GPU_MEMORY_TYPE = 1,
  CPU_AND_GPU_MEMORY_TYPE = 2
} PoolType;

typedef struct MemoryBlock {
  enum BlockStatus { OCCUPIED = 0, FREE = 1 };

  BlockStatus block_status;
  mems_t size;
  MemoryBlock *front;
  MemoryBlock *rear;
  void *memory;
} MemoryBlock; // struct MemoryBlock

typedef MemoryBlock *mb_ptr;

class ChimeMemoryPool {
 public:
  typedef enum {
    MALLOC_FROM_CPU_MEMORY = 0,
    MALLOC_FROM_GPU_MEMORY = 1
  } MallocDeviceType;

  typedef enum {
    FREE_FROM_CPU_MEMORY = 0,
    FREE_FROM_GPU_MEMORY = 1
  } FreeDeviceType;

  typedef enum {
    UNINITIALIZED = 0,
    WORKING = 1,
    READY_TO_BE_FREED = 2
  } PoolStatus;

  explicit ChimeMemoryPool(PoolType pType, mems_t size);

  ~ChimeMemoryPool();

  mems_t pool_size() const;

  PoolType pool_type() const;

  PoolStatus pool_status() const;

  float32 memory_usage() const;

  inline bool check_cpu_memory_block() const {
    return _cpu_memory_block ? true : false;
  }

  inline bool check_cpu_head() const { return _cpu_head ? true : false; }

  inline bool check_gpu_head() const { return _gpu_head ? true : false; }

  inline const mb_ptr cpu_memory_block() const { return _cpu_memory_block; }

  inline const void *cpu_head() const { return _cpu_head; }

  inline const mb_ptr cpu_next_free() const { return _cpu_next_free; }

  inline const mb_ptr cpu_next_malloc() const { return _cpu_next_malloc; }

  void malloc(void **ptr, mems_t size, MallocDeviceType malloc_type);

  void malloc(void **ptr, mems_t size);

  void free(void *ptr, FreeDeviceType free_type);

  void free(void *ptr);

  void init();

  void destroy();

 private:
  ChimeMemoryPool() = delete;

  mems_t _pool_size;

  mb_ptr _cpu_memory_block;
  void *_cpu_head;
  mb_ptr _cpu_next_free;
  mb_ptr _cpu_next_malloc;

  mb_ptr _gpu_memory_block;
  void *_gpu_head;
  mb_ptr _gpu_next_free;
  mb_ptr _gpu_next_malloc;

  PoolType _p_type;

  PoolStatus _p_status;

  MUTEXTYPE _mutex;

  DISABLE_COPY_AND_ASSIGN(ChimeMemoryPool);

}; // class ChimeMemoryPool
} // namespace memory
} // namespace chime
#endif // CHIME_CORE_MEMORY_CHIME_MP_HPP_
