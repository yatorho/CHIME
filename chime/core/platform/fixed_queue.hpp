// Copyright by 2022.6 chime
// author: yatorho

#ifndef CHIME_CORE_PLATFORM_FIXED_QUEUE_HPP_
#define CHIME_CORE_PLATFORM_FIXED_QUEUE_HPP_

#include <queue>

namespace chime {
namespace platform {

template <typename Tp>
struct QueueWrapper {
  typedef std::queue<Tp *> QueueContainer;
};

}  // namespace platform
}  // namespace chime

#endif  // CHIME_CORE_PLATFORM_FIXED_QUEUE_HPP_
