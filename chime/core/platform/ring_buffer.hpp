// Copyright by 2022.6 chime
// author: yatorho

#ifndef CHIME_CORE_PLATFORM_RING_BUFFER_HPP_
#define CHIME_CORE_PLATFORM_RING_BUFFER_HPP_

#include <queue>

#include "chime/core/framework/common.hpp"
#include "chime/core/platform/fixed_queue.hpp"

#if defined(__linux__)

#include "pthread.h"

typedef pthread_mutex_t mutex_type;
#define init_mutex(mutex) pthread_mutex_init(&(mutex), NULL)
#define delete_mutex(mutex) pthread_mutex_destroy(&(mutex), NULL)
#define lock(mutex) pthread_mutex_lock(&(mutex))
#define unlock(mutex) pthread_mutex_unlock(&(mutex))

#elif defined(__WIN32__)

#include "windows.h"

typedef CRITICAL_SECTION mutex_type
#define init_mutex(mutex) InitializeCriticalSection(&(mutex))
#define delete_mutex(mutex) DeleteCriticalSection(&(mutex))
#define lock(mutex) EnterCriticalSection(&(mutex))
#define unlock(mutex) LeaveCriticalSection(&(mutex))
#endif

namespace chime {
namespace platform {
/**
 * \brief
 */
template <typename Tp,
          typename Container = typename QueueWrapper<Tp>::QueueContainer>
class RingBuffer {
  typedef Container container_type;

 public:
  bool is_empty() const {
    if (_queue.size() == 0) return true;
    return false;
  }

  bool is_full() const {
    if (_queue.size() == _capacity) return true;
    return false;
  }

  uint64_t capacity() const { return _capacity; }

  bool add_element(Tp *element) {
    lock(_mutex);
    if (is_full()) {
      unlock(_mutex);
      return false;
    } else {
      _queue.push(element);
      unlock(_mutex);
      return true;
    }
  }

  bool get_element(Tp **element) {
    lock(_mutex);
    if (is_empty()) {
      unlock(_mutex);
      return false;
    } else {
      *element = _queue.front();
      _queue.pop();
      unlock(_mutex);
      return true;
    }
  }

  RingBuffer(uint64_t capacity) : _capacity(capacity) { init_mutex(_mutex); }

 private:
  uint64_t _capacity;

  container_type _queue;

  mutex_type _mutex;

  DISABLE_COPY_AND_ASSIGN(RingBuffer);
};  // class RingBuffer

}  // namespace platform
}  // namespace chime

#endif  // CHIME_CORE_PLATFROM_RING_BUFFER_HPP_
