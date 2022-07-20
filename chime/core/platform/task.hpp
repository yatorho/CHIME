// Copyright by 2022.6 chime
// author: yatorho

#ifndef CHIME_CORE_PLATFORM_TASK_HPP_
#define CHIME_CORE_PLATFORM_TASK_HPP_

namespace chime {
namespace platform {

/// \brief Interface of basic unit for thread pool's parallelization scheduling
class Task {
 public:
  virtual void Run() = 0;
  virtual ~Task() {}
};

}  // namespace platform
}  // namespace chime

#endif  // CHIME_CORE_PLATFORM_TASK_HPP_
