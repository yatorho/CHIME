// Copyright by 2022.6 chime
// author: yatorho

#ifndef CHIME_CORE_PLATFORM_THREADPOOL_H_
#define CHIME_CORE_PLATFORM_THREADPOOL_H_

#include "chime/core/framework/common.hpp"

namespace chime {
namespace platform {

class ThreadPool;

/**
 * \brief
 */
class Task {
 public:
  virtual void run() = 0;
  virtual ~Task();
};  // class Task

/**
 * \brief
 */
class ThreadPool {
 protected:
  ThreadPool();

 private:
  static ThreadPool _thread_pool;

  DISABLE_COPY_AND_ASSIGN(ThreadPool);
};  // class ThreadPool

}  // namespace platform
}  // namespace chime

#endif  // CHIME_CORE_PLATFORM_THREADPOOL_H_
