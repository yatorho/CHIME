// Copyright by 2022.6 chime
// author: yatorho

#ifndef CHIME_CORE_UTIL_PLATFORM_FIXED_QUEUE_HPP_
#define CHIME_CORE_UTIL_PLATFORM_FIXED_QUEUE_HPP_

#include <mutex>
#include <queue>

#include "chime/core/util/container_wrapper.hpp"

namespace chime {
namespace concurrent {
using util::ContainerWrapper;
/**
 * \brief Fixed-size queue for concurrent access.
 */
template <typename Tp, bool strict = false,
          typename Container =
              typename ContainerWrapper<Tp, std::queue<Tp>>::container_type>
class FixedQueue {
  typedef Container container_type;
  typedef std::mutex mutex_type;
  typedef std::unique_lock<std::mutex> unique_lock;
  typedef typename ContainerWrapper<Tp, container_type>::value_type value_type;
  typedef typename ContainerWrapper<Tp, container_type>::size_type size_type;
  typedef typename ContainerWrapper<Tp, container_type>::reference reference;
  typedef typename ContainerWrapper<Tp, container_type>::const_reference
      const_reference;
  typedef typename ContainerWrapper<Tp, container_type>::pointer pointer;
  typedef typename ContainerWrapper<Tp, container_type>::const_pointer
      const_pointer;

 public:
  bool empty() const { return _queue.empty(); }

  reference front() {
    return !strict ? _queue.front() : (unique_lock(_mutex), _queue.front());
  }

  reference back() {
    return !strict ? _queue.back() : (unique_lock(_mutex), _queue.back());
  }

  const_reference front() const { return _queue.front(); }
  const_reference back() const { return _queue.back(); }

  size_type size() const { return _queue.size(); }

  bool strcit() const { return strict; }

  template <typename... Args>
  void emplace(Args &&...args) {
    unique_lock lock(_mutex);
    _queue.emplace(std::forward<Args>(args)...);
  }

  void pop() {
    unique_lock lock(_mutex);
    _queue.pop();
  }

  void push(const value_type &value) {
    unique_lock lock(_mutex);
    _queue.push(value);
  }

  void push(value_type &&value) {
    unique_lock lock(_mutex);
    _queue.push(std::move(value));
  }

  void swap(FixedQueue<Tp> &other) {
    unique_lock lock(_mutex);
    _queue.swap(other._queue);
  }

  FixedQueue() {}

 private:
  container_type _queue;
  mutex_type _mutex;

  // Disable copy and assign.
  FixedQueue(const FixedQueue &);
  FixedQueue &operator=(const FixedQueue &);
};
}  // namespace concurrent
}  // namespace chime

#endif  // CHIME_CORE_UTIL_PLATFORM_FIXED_QUEUE_HPP_
