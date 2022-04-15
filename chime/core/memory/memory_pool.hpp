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

struct MemoryBlock {
  enum BlockStatus { OCCOPYIED, FREE };

  BlockStatus block_status;
  mems_t size;
  BlockStatus *rear;
  BlockStatus *next;
}; // struct MemoryBlock

typedef MemoryBlock *MP_PTR;

class ChimeMemoryPool {
 public:
  ChimeMemoryPool() = delete;

  explicit ChimeMemoryPool(mems_t size);

  ~ChimeMemoryPool();

  mems_t pool_size() const;

 private:
  mems_t _pool_size;

  MP_PTR _free_ptr;
  MP_PTR _next_head;
  MP_PTR _head;

  DISABLE_COPY_AND_ASSIGN(ChimeMemoryPool);
}; // class ChimeMemoryPool

} // namespace chime
#endif // CHIME_CORE_MEMORY_CHIME_MP_HPP_
