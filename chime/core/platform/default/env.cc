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

  void SleepForMicroseconds(int64_t micros) override {
    while (micros > 0) {
      timespec sleep_time;
      sleep_time.tv_sec = 0;
      sleep_time.tv_nsec = 0;

      if (micros >= 1e6) {
        sleep_time.tv_sec =
            std::min<int64_t>(micros / 1e6, std::numeric_limits<time_t>::max());
        micros -= static_cast<int64_t>(sleep_time.tv_sec) * 1e6;
      }
      if (micros < 1e6) {
        sleep_time.tv_nsec = 1000 * micros;
        micros = 0;
      }
      while (nanosleep(&sleep_time, &sleep_time) != 0 && errno == EINTR) {
        // Ignore signals and wait for the full interval to elapse.
      }
    }
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
