// Copyright by 2022.6 chime
// author: yatorho

#ifndef CHIME_CORE_PLATFORM_THREADPOOL_IMPL_H_
#define CHIME_CORE_PLATFORM_THREADPOOL_IMPL_H_

#include <condition_variable>
#include <functional>
#include <mutex>
#include <vector>

#include "chime/core/platform/env.hpp"
#include "chime/core/util/container_wrapper.hpp"
#include "chime/core/util/fixed_queue.hpp"

namespace chime {
namespace platform {

struct ThreadPoolEnv;

struct ThreadPoolEnv {
  typedef std::mutex mutex_type;
  typedef std::unique_lock<std::mutex> unique_lock;
  typedef std::condition_variable condition_variable_type;
  typedef Thread EnvThread;
  typedef std::atomic<bool> atomic_bool;
  typedef std::atomic<int64_t> atomic_int64_t;

  ThreadPoolEnv(Env *env) : env(env) {}

  Env *env;
};

/// \brief Real implementation for thread pool.
class ThreadPoolImpl {
  typedef chime::concurrent::FixedQueue<std::function<void()>> tasks_queue_type;
  typedef ThreadPoolEnv::mutex_type mutex_type;
  typedef ThreadPoolEnv::unique_lock unique_lock;
  typedef ThreadPoolEnv::condition_variable_type condition_variable_type;
  typedef ThreadPoolEnv::atomic_bool atomic_bool;
  typedef ThreadPoolEnv::atomic_int64_t atomic_int64_t;

 public:
  enum Status { UNINITIALIZED, RUNNING };

 public:
  ThreadPoolImpl(Env *env, const ThreadOptions &thread_options,
                 const std::string &name, int64_t num_threads,
                 bool low_latency_hint);

  ~ThreadPoolImpl();

  void init();

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

  /// \brief Returns the number of working threads in the thread pool.
  int64_t num_active_workers() const;

  bool low_latency_hint() const;

  Status status() const;

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
  mutex_type _mutex;
  condition_variable_type _cond;
};

}  // namespace platform
}  // namespace chime

#endif  // CHIME_CORE_PLATFORM_THREADPOOL_IMPL_H_
