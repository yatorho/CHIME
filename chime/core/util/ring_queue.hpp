// Copyright by 2022.6 chime
// author: yatorho

#ifndef CHIME_CORE_UTIL_RING_QUEUE_HPP_
#define CHIME_CORE_UTIL_RING_QUEUE_HPP_

#include <atomic>
#include <cstdint>

#include "chime/core/util/container_wrapper.hpp"
#include "chime/core/util/fixed_queue.hpp"

namespace chime {
namespace concurrent {

template <typename Tp, bool strict = false,
          typename Container =
              typename ContainerWrapper<Tp, FixedQueue<Tp>>::container_type>
class RingQueue {
  typedef Container container_type;
  typedef typename ContainerWrapper<Tp, container_type>::size_type size_type;
  typedef typename ContainerWrapper<Tp, container_type>::reference reference;
  typedef typename ContainerWrapper<Tp, container_type>::const_reference
      const_reference;
  typedef typename ContainerWrapper<Tp, container_type>::pointer pointer;
  typedef typename ContainerWrapper<Tp, container_type>::const_pointer
      const_pointer;

 public:
  RingQueue(size_type size) : _capacity(size) {}

  bool empty() const { return _queue.empty(); }

  bool full() const { return _queue.size() == _capacity; }

  size_type capacity() const { return _capacity; }

  bool add_element(const Tp &element) {
    return full() ? false : (_queue.push(element), true);
  }

  bool get_element(Tp *ptr) {
    return empty() ? false : (ptr = &_queue.front(), _queue.pop(), true);
  }

 private:
  uint64_t _capacity;

  container_type _queue;

  /// Disabled copy and assignment
  RingQueue(const RingQueue &);
  RingQueue &operator=(const RingQueue &);
};

}  // namespace concurrent
}  // namespace chime

#endif  // CHIME_CORE_UTIL_RING_QUEUE_HPP_
