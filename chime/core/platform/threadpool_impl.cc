// Copyright by 2022.6 chime
// author: yatorho

#include "chime/core/platform/threadpool_impl.h"

#include <string>

#include "chime/core/platform/logging.hpp"
#include "chime/core/platform/mutex.h"
#include "chime/core/util/algorithm.hpp"

namespace chime {
namespace platform {

using algorithm::accumulate;

ThreadPoolImpl::ThreadPoolImpl(Env *env, const ThreadOptions &thread_options,
                               const std::string &name, int64_t num_threads,
                               bool low_latency_hint)
    : _name(name),
      _num_threads(num_threads),
      _active_workers(0),
      _low_latency_hint(low_latency_hint),
      _status(UNINITIALIZED),
      _shutdown(false),
      _env(env),
      _thread_options(thread_options),
      _success_init_flags(new int64_t[uint64_t(num_threads)]) {
  _workers.reserve(num_threads);
}

void ThreadPoolImpl::Init() {
  CHECK_GT(NumThreads(), 0);
  CHECK_EQ(_status, UNINITIALIZED) << "Init() called twice on the same "
                                      "ThreadPool.";
  _status = RUNNING;
  for (int i = 0; i < NumThreads(); i++) {
    _success_init_flags[i] = false;

    _workers.emplace_back(_env.env->StartThread(
        _thread_options, std::string("worker:") + std::to_string(i),
        [i, this]() {
          for (;;) {
            std::function<void()> task;

            {
              mutex_lock lock(_mutex);
              if (_active_workers > 0) _active_workers--;
              _success_init_flags[i] = true;
              _cond.wait(lock, [this]() {
                return _shutdown || !_tasks_queue.empty();
              });
              _active_workers++;
              if (_shutdown && this->_tasks_queue.empty()) return;

              task = std::move(_tasks_queue.front());
              _tasks_queue.pop();
            }

            task();
          }
        }));
  }
  while (accumulate(_success_init_flags, _success_init_flags + NumThreads(),
                    0ll) != NumThreads())
    ;
}

void ThreadPoolImpl::Schedule(std::function<void()> func) {
  {
    mutex_lock lock(_mutex);
    CHECK_NE(_status, UNINITIALIZED)
        << "Schedule a function on a non initialized thread pool";
    _tasks_queue.push(std::move(func));
  }
  _cond.notify_one();
}

void ThreadPoolImpl::Wait() {
  for (;;) {
    mutex_lock lock(_mutex);
    if (_tasks_queue.empty() && _active_workers == 0) return;
  }
}

ThreadPoolImpl::~ThreadPoolImpl() {
  {
    mutex_lock lock(_mutex);
    _shutdown = true;
  }
  _cond.notify_all();
  for (auto &worker : _workers) {
    worker->~Thread();
  }
  delete[] _success_init_flags;
}

int64_t ThreadPoolImpl::NumPendingTasks() const {
  return _tasks_queue.size();
}

int64_t ThreadPoolImpl::NumActiveWorkers() const { return _active_workers; }

int64_t ThreadPoolImpl::NumThreads() const { return _num_threads; }

const std::string &ThreadPoolImpl::Name() const { return _name; }

bool ThreadPoolImpl::LowLatencyHint() const { return _low_latency_hint; }

ThreadPoolImpl::Status ThreadPoolImpl::GetStatus() const { return _status; }

}  // namespace platform
}  // namespace chime
