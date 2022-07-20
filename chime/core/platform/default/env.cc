// Copyright by 2022.6 chime
// author: yatorho

#include "chime/core/platform/env.hpp"

#include <thread>

#include "chime/core/platform/logging.hpp"

namespace chime {
namespace platform {

class StdThread : public Thread {
 public:
  /// thread_options is ignored.
  StdThread(const ThreadOptions &thread_options, const std::string &name,
            std::function<void()> fn)
      : _thread(fn) {}
  ~StdThread() override { _thread.join(); }

 private:
  std::thread _thread;
};

class PosixEnv : public Env {
 public:
  PosixEnv() {}

  ~PosixEnv() override { LOG(FATAL) << "Env::Default() must not be destroyed"; }

  int64_t GetCurrentThreadID() override {
    static thread_local int64_t current_thread_id =
        GetCurrentThreadIDInternal();
    return current_thread_id;
  }

  Thread *StartThread(const ThreadOptions &thread_options,
                       const std::string &name,
                       std::function<void()> fn) override {
    return new StdThread(thread_options, name, fn);
  }

 private:
  int64_t GetCurrentThreadIDInternal() {
#ifdef __APPLE__
    uint64_t tid64;
    pthread_threadid_np(nullptr, &tid64);
    return static_cast<int32>(tid64);
#elif defined(__FreeBSD__)
    return pthread_getthreadid_np();
#elif defined(__NR_gettid)
    return static_cast<int32>(syscall(__NR_gettid));
#else
    return std::hash<std::thread::id>()(std::this_thread::get_id());
#endif
  }
};

Env *Env::Default() {
  static Env *default_env = new PosixEnv;
  return default_env;
}

}  // namespace platform
}  // namespace chime
