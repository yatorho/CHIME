// Copyright by 2022.6 chime
// author: yatorho

#ifndef CHIME_CORE_PLATFORM_THREADPOOL_IMPL_H_
#define CHIME_CORE_PLATFORM_THREADPOOL_IMPL_H_

#include <condition_variable>
#include <functional>
#include <mutex>

#include "chime/core/platform/env.hpp"
#include "chime/core/platform/ring_buffer.hpp"

namespace chime {
namespace platform {

class ThreadPoolImpl;
struct ThreadPoolEnv;

struct ThreadPoolEnv {
  typedef std::mutex mutex_type;
  typedef std::unique_lock<std::mutex> unique_lock;
  typedef std::condition_variable condition_variable_type;
  typedef Thread EnvThread;
};

/// \brief Real implementation for thread pool.
class ThreadPoolImpl {
 public:
  ThreadPoolImpl(Env *env, const ThreadOptions &thread_options,
                 const std::string &name, int64_t num_threads,
                 bool low_latency_hint);

  ~ThreadPoolImpl();

  /// \brief Schedules a function for execution in the thread pool.
  void schedule(std::function<void()> func);

  /// \brief Waits until all work has finished and there are no outstanding
  /// tasks.
  void wait();

  /// \brief Returns the number of threads in the thread pool.
  int64_t num_threads() const;

  /// \brief Returns the name of the thread pool.
  const std::string &name() const;

  /// \brief Returns the number of tasks that have been scheduled but not yet
  /// executed.
  int64_t num_pending_tasks() const;

  /// \brief Returns the number of tasks that have been executed.
  int64_t num_completed_tasks() const;

  bool low_latency_hint() const;

 private:
  std::string _name;
  int64_t _num_threads;
  bool _low_latency_hint;
  ThreadPoolEnv *_env;
  RingBuffer<ThreadPoolEnv::EnvThread> _threads;
};

}  // namespace platform
}  // namespace chime

#endif  // CHIME_CORE_PLATFORM_THREADPOOL_IMPL_H_
