// Copyright by 2022.4 chime
// author: yatorho

#ifndef CHIME_CORE_MEMORY_CHIME_MP_HPP_
#define CHIME_CORE_MEMORY_CHIME_MP_HPP_

#include "chime/core/framework/common.hpp"

#if defined(__WIN32__)

typedef CRITICAL_SECTION MUTEXTYPE
#define INITMUTEX(hMutex) InitializeCriticalSection(&(hMutex))
#define DELMUTEX(hMutex) DeleteCriticalSection(&(hMutex))
#define LOCK(hMutex) EnterCriticalSection(&(hMutex))
#define UNLOCK(hMutex) LeaveCriticalSection(&(hMutex))

#elif defined(__linux__)

#include <pthread.h>

typedef pthread_mutex_t MUTEXTYPE;
#define INITMUTEX(hMutex) pthread_mutex_init(&(hMutex), NULL)
#define DELMUTEX(hMutex) pthread_mutex_destroy(&(hMutex))
#define LOCK(hMutex) pthread_mutex_lock(&(hMutex))
#define UNLOCK(hMutex) pthread_mutex_unlock(&(hMutex))

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

  static utens_t blocks_count;
  inline MemoryBlock() { blocks_count++; }

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

  float32 memory_usage() const;

  inline bool check_cpu_memory_block() const {
    return _cpu_memory_block ? true : false;
  }

  inline bool check_cpu_head() const { return _cpu_head ? true : false; }

  inline bool check_cpu_free_mb_list() const {
    return _cpu_free_mb_list ? true : false;
  }

  inline bool check_gpu_memory_block() const {
    return _gpu_memory_block ? true : false;
  }

  inline bool check_gpu_head() const { return _gpu_head ? true : false; }

  inline bool check_gpu_free_mb_lists() const {
    return _gpu_free_mb_list ? true : false;
  }

  void allocate(void **ptr, mems_t size, MallocDeviceType malloc_type);

  void free(void *ptr, FreeDeviceType free_type);

  void init();

 private:
  ChimeMemoryPool() = delete;

  void destroy();

  mems_t _pool_size;

  mb_ptr _cpu_memory_block;
  mb_ptr _cpu_free_mb_list;
  void *_cpu_head;

  mb_ptr _gpu_memory_block;
  mb_ptr _gpu_free_mb_list;
  void *_gpu_head;

  PoolType _p_type;

  PoolStatus _p_status;

  DISABLE_COPY_AND_ASSIGN(ChimeMemoryPool);

}; // class ChimeMemoryPool
} // namespace memory
} // namespace chime
#endif // CHIME_CORE_MEMORY_CHIME_MP_HPP_
