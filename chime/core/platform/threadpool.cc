// Copyright by 2022.6 chime
// author: yatorho

#include "chime/core/platform/threadpool.h"

#include <functional>

#include "chime/core/framework/common.hpp"

namespace chime {
namespace platform {

int64_t ThreadPool::num_shards_used_by_fixed_block_size_scheduling(
    int64_t total, const int64_t block_size) const {
  if (block_size <= 0 || total <= 1 || total <= block_size ||
      num_threads() == 1)
    return 1;
  return (total + block_size - 1) / block_size;
}

ThreadPool::ThreadPool(Env *env, const ThreadOptions &thread_options,
                       const std::string &name, int64_t num_threads,
                       bool low_latency_hint) {
  DCHECK_GT(num_threads, 0);
  _underlying_pool.reset(new ThreadPoolImpl(env, thread_options, name,
                                            num_threads, low_latency_hint));
}

ThreadPool::ThreadPool(Env *env, const std::string &name, int64_t num_threads,
                       bool low_latency_hint)
    : ThreadPool(env, ThreadOptions(), name, num_threads, low_latency_hint) {}

ThreadPool::ThreadPool(Env *env, const std::string &name, int64_t num_threads)
    : ThreadPool(env, name, num_threads, true) {}

ThreadPool::~ThreadPool() {}

void ThreadPool::schedule(std::function<void()> fn) {
  _underlying_pool->schedule(std::move(fn));
}

int64_t ThreadPool::num_threads() const {
  return _underlying_pool->num_threads();
}

int64_t ThreadPool::current_thread_id() const {
  return _underlying_pool->_env.env->get_current_thread_id();
}

const std::string &ThreadPool::name() const { return _underlying_pool->_name; }

int64_t ThreadPool::num_active_threads() const {
  return _underlying_pool->_active_workers;
}

int64_t ThreadPool::num_pending_tasks() const {
  return _underlying_pool->num_pending_tasks();
}

bool ThreadPool::low_latency_hint() const {
  return _underlying_pool->_low_latency_hint;
}

void ThreadPool::parallel_for_with_fixed_block(int64_t total, int64_t block_size, const std::function<void (int64_t, int64_t)> &fn) {
  NOT_IMPLEMENTED;
}

}  // namespace platform
}  // namespace chime
