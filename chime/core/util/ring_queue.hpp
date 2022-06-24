// Copyright by 2022.6 chime
// author: yatorho

#ifndef CHIME_CORE_UTIL_RING_QUEUE_HPP_
#define CHIME_CORE_UTIL_RING_QUEUE_HPP_

#include <cstdint>
#include <mutex>
#include <queue>

#include "chime/core/util/container_wrapper.hpp"

namespace chime {
namespace concurrent {

using util::ContainerWrapper;

template <typename Tp, bool strict = false,
          typename Container =
              typename ContainerWrapper<Tp, std::queue<Tp>>::container_type>
class RingQueue {
  typedef Container container_type;
  typedef typename ContainerWrapper<Tp, container_type>::size_type size_type;
  typedef typename ContainerWrapper<Tp, container_type>::reference reference;
  typedef typename ContainerWrapper<Tp, container_type>::const_reference
      const_reference;
  typedef typename ContainerWrapper<Tp, container_type>::pointer pointer;
  typedef typename ContainerWrapper<Tp, container_type>::const_pointer
      const_pointer;
  typedef std::mutex mutex_type;
  typedef std::unique_lock<std::mutex> unique_lock;

 public:
  RingQueue(size_type size) : _capacity(size) {}

  bool empty() const { return _queue.empty(); }

  bool full() const { return _queue.size() == _capacity; }

  size_type size() const { return _queue.size(); }

  size_type capacity() const { return _capacity; }

  bool add_element(const Tp &element) {
    unique_lock lock(_mutex);
    return full() ? false : (_queue.push(element), true);
  }

  bool get_element(Tp *ptr) {
    unique_lock lock(_mutex);
    return empty() ? false : (*ptr = _queue.front(), _queue.pop(), true);
  }

 private:
  uint64_t _capacity;

  container_type _queue;
  mutex_type _mutex;

  /// Disabled copy and assignment
  RingQueue(const RingQueue &);
  RingQueue &operator=(const RingQueue &);
};

}  // namespace concurrent
}  // namespace chime

#endif  // CHIME_CORE_UTIL_RING_QUEUE_HPP_
