// Copyright by 2022.6 chime
// author: yatorho

#ifndef CHIME_CORE_PLATFORM_RING_BUFFER_HPP_
#define CHIME_CORE_PLATFORM_RING_BUFFER_HPP_

#include <mutex>
#include <queue>

#include "chime/core/framework/common.hpp"
#include "chime/core/platform/fixed_queue.hpp"

namespace chime {
namespace platform {

/**
 * \brief Ring buffer for concurrent access.
 */
template <typename Tp,
          typename Container = typename QueueWrapper<Tp>::QueueContainer>
class RingBuffer {
  typedef Container container_type;
  typedef std::mutex mutex_type;
  typedef std::unique_lock<std::mutex> unique_lock;

 public:
  bool is_empty() const {
    return _queue.size() == 0;
  }

  bool is_full() const {
    return _queue.size() == _capacity;
  }

  uint64_t capacity() const { return _capacity; }

  bool add_element(Tp *element) {
    unique_lock lock(_mutex);
    return is_full() ? false : (_queue.push(element), true);
  }

  bool get_element(Tp **element) {
    unique_lock lock(_mutex);
    return is_empty() ? false
                      : (*element = _queue.front(), _queue.pop(), true);
  }

  RingBuffer(uint64_t capacity) : _capacity(capacity) { }

 private:
  uint64_t _capacity;
  container_type _queue;
  mutex_type _mutex;

  DISABLE_COPY_AND_ASSIGN(RingBuffer);
};  // class RingBuffer

}  // namespace platform
}  // namespace chime

#endif  // CHIME_CORE_PLATFROM_RING_BUFFER_HPP_
