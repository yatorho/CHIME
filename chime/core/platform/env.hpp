// Copyright by 2022.6 chime
// author: yatorho

#ifndef CHIME_CORE_PLATFORM_ENV_HPP_
#define CHIME_CORE_PLATFORM_ENV_HPP_

#include <cstddef>
#include <functional>

#include "chime/core/framework/common.hpp"
namespace chime {
namespace platform {

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

class Env;
class Thread;

struct ThreadOptions {
  /// Thread stack size to use (in bytes).
  size_t stack_size = 0;  // 0: use system default value
  /// Guard area size to use near thread stacks to use (in bytes)
  size_t guard_size = 0;  // 0: use system default value
};

class Env {
 public:
  Env();
  virtual ~Env() = default;

  /// \brief Returns a default environment for the current operating system.
  static Env *Default();

  virtual Thread *start_thread(const ThreadOptions &thread_options,
                               const std::string &name,
                               std::function<void()> fn) = 0;

  virtual int64_t get_current_thread_id() = 0;

 private:
  DISABLE_COPY_AND_ASSIGN(Env);
};

class Thread {
 public:
  Thread() {}

  /// Blocks until the thread of control stops running.
  virtual ~Thread();

 private:
  DISABLE_COPY_AND_ASSIGN(Thread);
};

}  // namespace platform
}  // namespace chime
#endif  // CHIME_CORE_PLATFORM_ENV_HPP_