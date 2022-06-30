// Copyright by 2022.6 chime
// author: yatorho

#include "chime/core/platform/threadpool.h"

#include <algorithm>
#include <functional>
#include <limits>

namespace chime {
namespace platform {

struct CostHelper {
  static int64_t estimate_num_threads_from_cost_per_unit(  /// To be optimized.
      int64_t cost_per_unit, int64_t num_units, int64_t max_num_threads) {
    float ratio =
        static_cast<float>(num_digits(cost_per_unit)) /
        static_cast<float>(num_digits(std::numeric_limits<int32_t>::max()));
    int64_t num_threads = std::min(
        static_cast<int64_t>(ratio * max_num_threads), max_num_threads);

    if (num_threads < 1) num_threads = 1;
    return num_threads;
  }

  /// Find how many digits a big number has.
  static int64_t num_digits(int64_t num) {
    int64_t digits = 0;
    while (num > 0) {
      num /= 10;
      digits++;
    }
    return digits;
  }
};

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
  _underlying_pool->init();
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

void ThreadPool::parallel_for_with_fixed_block(
    int64_t total, int64_t block_size,
    const std::function<void(int64_t, int64_t)> &fn) {
  _parallel_for_fixed_block_size_scheduling(total, block_size, fn);
}

void ThreadPool::parallel_for(int64_t total,
                              const SchedulingParams &scheduling_params,
                              const std::function<void(int64_t, int64_t)> &fn) {
  switch (scheduling_params.strategy()) {
    case SchedulingStrategy::ADAPTIVE:
      if (scheduling_params.cost_per_unit()) {
        parallel_for(total, *scheduling_params.cost_per_unit(), fn);
      } else
        LOG(WARNING)
            << "SchedulingStrategy::ADAPTIVE did not get cost_per_unit";
      break;
    case SchedulingStrategy::FIXED_BLOCK_SIZE:
      if (scheduling_params.block_size()) {
        _parallel_for_fixed_block_size_scheduling(
            total, *scheduling_params.block_size(), fn);
      } else
        LOG(WARNING)
            << "SchedulingStrategy::FIXED_BLOCK_SIZE did not get block_size";
      break;
    default:
      LOG(FATAL) << "Unknown scheduling strategy";
  }
}

void ThreadPool::_parallel_for_fixed_block_size_scheduling(
    int64_t total, int64_t block_size,
    const std::function<void(int64_t, int64_t)> &fn) {
  const int num_shards_used =
      num_shards_used_by_fixed_block_size_scheduling(total, block_size);

  if (num_shards_used == 1) {
    fn(0, total);
    return;
  }

  for (int64_t i = 0; i < num_shards_used - 1; ++i) {
    _underlying_pool->schedule(
        [i, &fn, block_size]() { fn(i * block_size, (i + 1) * block_size); });
  }
  _underlying_pool->schedule([&fn, total, block_size, num_shards_used]() {
    fn(block_size * (num_shards_used - 1), total);
  });

  _underlying_pool->wait();
}

void ThreadPool::parallel_for(int64_t total, int64_t cost_per_unit,
                              const std::function<void(int64_t, int64_t)> &fn) {
  CHECK_GT(total, 0);
  CHECK_GT(cost_per_unit, 0);
  const int64_t max_num_threads = num_threads() - num_active_threads();
  if (max_num_threads != num_threads())
    LOG(INFO) << "Idle threads in pool is less than num_threads in Adaptive "
                 "scheduling, which means single thread is used to execute "
                 "bigger task.";
  const int64_t num_threads =
      CostHelper::estimate_num_threads_from_cost_per_unit(cost_per_unit, total,
                                                          max_num_threads);
  if (num_threads == 1) {
    fn(0, total);
    return;
  }
  const int64_t shard_size = (total + num_threads - 1) / num_threads;
  for (int64_t i = 0; i < num_threads - 1; ++i) {
    _underlying_pool->schedule(
        [i, &fn, shard_size]() { fn(i * shard_size, (i + 1) * shard_size); });
  }
  _underlying_pool->schedule([&fn, total, shard_size, num_threads]() {
    fn(shard_size * (num_threads - 1), total);
  });
  _underlying_pool->wait();
}

void ThreadPool::wait() { _underlying_pool->wait(); }

ThreadPool::Status ThreadPool::status() const {
  return _underlying_pool->status();
}

int64_t ThreadPool::num_threads_by_adaptive_scheduling(
    int64_t total, int64_t cost_per_unit) const {
  return CostHelper::estimate_num_threads_from_cost_per_unit(
      cost_per_unit, total, num_threads() - num_active_threads());
}

}  // namespace platform
}  // namespace chime
