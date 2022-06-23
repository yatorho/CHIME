// Copyright by 2022.6 chime
// author: yatorho

#ifndef CHIME_CORE_PLATFORM_BUFFER_QUEUE_HPP_
#define CHIME_CORE_PLATFORM_BUFFER_QUEUE_HPP_

#include <mutex>

#include "chime/core/framework/common.hpp"
#include "chime/core/platform/fixed_queue.hpp"

namespace chime {
namespace platform {

template <typename Tp,
          typename Container = typename QueueWrapper<Tp>::QueueContainer>
class Buffer {
  typedef Container container_type;
  typedef std::mutex mutex_type;
  typedef std::unique_lock<std::mutex> unique_lock;

 public:
  Buffer() {}

  bool is_empty() const {
    return _queue.size() == 0;
  }

  void add_element(Tp *element) {
    unique_lock lock(_mutex);
    _queue.push(element);
  }

  bool get_element(Tp **element) {
    unique_lock lock(_mutex);
    return is_empty() ? false
                      : (*element = _queue.front(), _queue.pop(), true);
  }

  size_t size() const {
    return _queue.size();
  } 

 private:
  container_type _queue;
  mutex_type _mutex;

  DISABLE_COPY_AND_ASSIGN(Buffer);
};
}  // namespace platform
}  // namespace chime

#endif  // CHIME_CORE_PLATFORM_BUFFER_QUEUE_HPP_
