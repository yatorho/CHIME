// Copyright by 2022.6 chime
// author: yatorho

#ifndef CHIME_CORE_PLATFORM_THREADPOOL_H_
#define CHIME_CORE_PLATFORM_THREADPOOL_H_

#include <cstdint>
#include <functional>
#include <optional>

#include "chime/core/framework/common.hpp"
#include "chime/core/platform/env.hpp"
#include "chime/core/platform/ring_buffer.hpp"
#include "chime/core/util/optional.hpp"

namespace chime {
namespace platform {

class ThreadPool;

/**
 * \brief
 */
class Task {
 public:
  virtual void run() = 0;
  virtual ~Task();
};  // class Task

/**
 * \brief Scheduler for parallel scheduling.
 */
class ThreadPool {
 public:
  /// Scheduling strategies for `parallel_for`. The strategy governs how the
  /// given units of work are distributed among the available threads in the
  /// threadpool.
  enum class SchedulingStrategy {
    /// The Adaptive scheduling strategy adaptively chooses the shard sizes
    /// based
    /// on the cost of each unit of work, and the cost model of the underlying
    /// threadpool device.
    ///
    /// The 'cost_per_unit' is an estimate of the number of CPU cycles (or
    /// nanoseconds if not CPU-bound) to complete a unit of work. Overestimating
    /// creates too many shards and CPU time will be dominated by per-shard
    /// overhead, such as Context creation. Underestimating may not fully make
    /// use of the specified parallelism, and may also cause inefficiencies due
    /// to load balancing issues and stragglers.
    ADAPTIVE,
    /// The Fixed Block Size scheduling strategy shards the given units of work
    /// into shards of fixed size. In case the total number of units is not
    /// evenly divisible by 'block_size', at most one of the shards may be of
    /// smaller size. The exact number of shards may be found by a call to
    /// num_shards_used_by_fixed_block_size_scheduling.
    ///
    /// Each shard may be executed on a different thread in parallel, depending
    /// on the number of threads available in the pool. Note that when there
    /// aren't enough threads in the pool to achieve full parallelism, function
    /// calls will be automatically queued.
    FIXED_BLOCK_SIZE
  };

  /// Contains additional parameters for either the Adaptive or the Fixed Block
  /// Size scheduling strategy.
  class SchedulingParams {
   public:
    explicit SchedulingParams(SchedulingStrategy strategy,
                              Optional<int64_t> cost_per_unit,
                              Optional<int64_t> block_size)
        : _strategy(strategy),
          _cost_per_unit(cost_per_unit),
          _block_size(block_size) {}

    SchedulingStrategy strategy() const { return _strategy; }
    Optional<int64_t> const_per_unit() const { return _cost_per_unit; }
    Optional<int64_t> block_size() const { return _block_size; }

   private:
    /// The underlying Scheduling Strategy for which this instance contains
    /// additional parameters.
    SchedulingStrategy _strategy;

    /// The estimated cost per unit of work in number of CPU cycles.
    /// Only applicable for Adaptive scheduling strategy.
    Optional<int64_t> _cost_per_unit;

    /// The block size of each shard. Only applicable for Fixed Block Size
    /// scheduling strategy.
    Optional<int64_t> _block_size;
  };

  ///
  ThreadPool(Env *env, const std::string &name, int64_t num_threads,
             bool low_latency_hint);

  ThreadPool(Env *env, const std::string &name, int64_t num_thread);

  /// Waits until all scheduled work has finished and then destroy the
  /// set of threads.
  ~ThreadPool();

  /// Schedules fn() for execution, which will be assigned a single thread in
  /// thread pool.
  void schedule(std::function<void()> fn);

  void schedule_with_hint(std::function<void()> fn, int64_t start,
                          int64_t limit);

  /// Returns the number of shards used by
  /// `_parallel_for_fixed_block_size_scheduling` with these parameters.
  int64_t num_shards_used_by_fixed_block_size_scheduling(
      int64_t total, const int64_t block_size) const;

  /// ParallelFor shards the "total" units of work assuming each unit of work
  /// having roughly "cost_per_unit" cost, in cycles. Each unit of work is
  /// indexed 0, 1, ..., total - 1. Each shard contains 1 or more units of work
  /// and the total cost of each shard is roughly the same.
  ///
  /// "cost_per_unit" is an estimate of the number of CPU cycles (or
  /// nanosecondsl if not CPU-bound) to complete a unit of work. Overestimating
  /// creates too many shards and CPU time will be dominated by per-shard
  /// overhead, such as Context creation. Underestimating may not fully make use
  /// of the specified parallelism, and may also cause inefficiencies due to
  /// load balancing issues and stragglers.
  void parallel_for(int64_t total, int64_t cost_per_unit,
                    const std::function<void(int64_t, int64_t)> &fn);

  /// Similar to ParallelFor above, but takes the specified scheduling strategy
  /// into account.
  void parallel_for(int64_t total, const SchedulingParams &scheduling_params,
                    const std::function<void(int64_t, int64_t)> &fn);

  /// Same as ParallelFor with Fixed Block Size scheduling strategy
  void parallel_for_with_fixed_block(
      int64_t total, int64_t block_size,
      const std::function<void(int64_t, int64_t)> &fn);

  /// Returns the number of threads in the pool.
  int64_t num_threads() const;

  /// Returns current thread id between 0 and NumThreads() - 1, if called from a
  /// thread in the pool. Returns -1 otherwise.
  int64_t current_thread_id() const;

 private:
  /// Divides the work represented by the range [0, total) into k shards.
  /// Calls fn(i*block_size, (i+1)*block_size) from the ith shard (0 <= i < k).
  /// Each shard may be executed on a different thread in parallel, depending on
  /// the number of threads available in the pool.
  /// When (i+1)*block_size > total, fn(i*block_size, total) is called instead.
  /// Here, k = num_shards_used_by_fixed_block_size_scheduling(total,
  /// block_size). Requires 0 < block_size <= total.
  void _parallel_for_fixed_block_size_scheduling(
      int64_t total, int64_t block_size,
      const std::function<void(int64_t, int64_t)> &fn);

 protected:
  ThreadPool();

 private:
  static ThreadPool _thread_pool;

  DISABLE_COPY_AND_ASSIGN(ThreadPool);
};  // class ThreadPool

}  // namespace platform
}  // namespace chime

#endif  // CHIME_CORE_PLATFORM_THREADPOOL_H_
