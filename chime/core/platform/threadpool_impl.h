// Copyright by 2022.6 chime
// author: yatorho

#ifndef CHIME_CORE_PLATFORM_THREADPOOL_IMPL_H_
#define CHIME_CORE_PLATFORM_THREADPOOL_IMPL_H_

#include <functional>
#include <vector>

#include "chime/core/platform/env.hpp"
#include "chime/core/util/container_wrapper.hpp"
#include "chime/core/util/fixed_queue.hpp"
#include "chime/core/platform/mutex.h"

namespace chime {
namespace platform {

struct ThreadPoolEnv;
class ThreadPool;

struct ThreadPoolEnv {  /// Just a wrapper for Env.
  typedef Thread EnvThread;

  ThreadPoolEnv(Env *env) : env(env) {}

  Env *env;
};

/// \brief Real implementation for thread pool.
class ThreadPoolImpl {
  typedef chime::concurrent::FixedQueue<std::function<void()>> tasks_queue_type;

 public:
  enum Status { UNINITIALIZED, RUNNING };

 public:
  /// low_latency_hint in ThreadPoolImpl just be ignored.
  ThreadPoolImpl(Env *env, const ThreadOptions &thread_options,
                 const std::string &name, int64_t num_threads,
                 bool low_latency_hint);

  ~ThreadPoolImpl();

  void Init();

  /// \brief Schedules a function for execution in the thread pool.
  void Schedule(std::function<void()> func);

  /// \brief Waits until all work has finished and there are no outstanding
  /// tasks.
  void Wait();

  /// \brief Returns the number of threads in the thread pool.
  int64_t NumThreads() const;

  /// \brief Returns the name of the thread pool.
  const std::string &Name() const;

  /// \brief Returns the number of tasks that have been scheduled but not yet
  /// executed.
  int64_t NumPendingTasks() const;

  /// \brief Returns the number of working threads in the thread pool.
  int64_t NumActiveWorkers() const;

  bool LowLatencyHint() const;

  Status GetStatus() const;

 public:
  friend class ThreadPool;

 private:
  std::string _name;
  int64_t _num_threads;
  int64_t _active_workers;
  bool _low_latency_hint;
  Status _status;
  bool _shutdown;
  ThreadPoolEnv _env;
  ThreadOptions _thread_options;
  int64_t *_success_init_flags;

  std::vector<ThreadPoolEnv::EnvThread *> _workers;
  tasks_queue_type _tasks_queue;
  mutex _mutex;
  condition_variable _cond;
};

}  // namespace platform
}  // namespace chime

#endif  // CHIME_CORE_PLATFORM_THREADPOOL_IMPL_H_
