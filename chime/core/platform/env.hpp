// Copyright by 2022.6 chime
// author: yatorho

#ifndef CHIME_CORE_PLATFORM_ENV_HPP_
#define CHIME_CORE_PLATFORM_ENV_HPP_

#include <condition_variable>
#include <functional>
#include <mutex>

#include "chime/core/platform/env_time.h"
#include "chime/core/platform/macros.h"

namespace chime {
namespace platform {

namespace concurrent {
#if defined(__linux__)

#include "pthread.h"

typedef pthread_mutex_t chime_mutex_type;
#define chime_init_mutex(mutex) pthread_mutex_init(&(mutex), NULL)
#define chime_delete_mutex(mutex) pthread_mutex_destroy(&(mutex), NULL)
#define chime_lock(mutex) pthread_mutex_lock(&(mutex))
#define chime_unlock(mutex) pthread_mutex_unlock(&(mutex))

#elif defined(__WIN32__)

#include "windows.h"

typedef CRITICAL_SECTION chime_mutex_type
#define chime_init_mutex(mutex) InitializeCriticalSection(&(mutex))
#define chime_delete_mutex(mutex) DeleteCriticalSection(&(mutex))
#define chime_lock(mutex) EnterCriticalSection(&(mutex))
#define chime_unlock(mutex) LeaveCriticalSection(&(mutex))
#endif

typedef std::mutex chime_mutex;
typedef std::unique_lock<std::mutex> chime_unique_lock;
typedef std::condition_variable chime_condition;
}  // namespace concurrent

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

  virtual uint64_t now_nanos() const { return EnvTime::now_nanos(); }

  virtual uint64_t now_micros() const { return EnvTime::now_micros(); }

  virtual uint64_t now_seconds() const { return EnvTime::now_seconds(); }

 private:
  CHIME_DISALLOW_COPY_AND_ASSIGN(Env);
};

class Thread {
 public:
  Thread() {}

  /// Blocks until the thread of control stops running.
  virtual ~Thread();

 private:
  CHIME_DISALLOW_COPY_AND_ASSIGN(Thread);
};

}  // namespace platform
}  // namespace chime
#endif  // CHIME_CORE_PLATFORM_ENV_HPP_
