// Copyright by 2022.6 chime
// author: yatorho

#ifndef CHIME_CORE_PLATFORM_ENV_HPP_
#define CHIME_CORE_PLATFORM_ENV_HPP_

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

class Env {
 public:
  Env();
  ~Env() = default;

  /// \brief Returns a default environment for the current operating system.
  static Env *Default() { return &_default_instance; }

 private:
  static Env _default_instance;
};

}  // namespace platform
}  // namespace chime
#endif  // CHIME_CORE_PLATFORM_ENV_HPP_
