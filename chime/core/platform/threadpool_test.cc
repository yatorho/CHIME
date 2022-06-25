// Copyright by 2022.6 chime
// author: yatorho

#include "chime/core/platform/threadpool.h"

#include <functional>

#include "chime/core/platform/env.hpp"
#include "chime/core/util/optional.hpp"

namespace chime {
namespace platform {

static const int32_t TEST_COUNT = 300;

TEST(ThreadPool, TestConstructor) {
  for (int32_t i = 0; i < TEST_COUNT; ++i) {
    ThreadPool pool(Env::Default(), "test_pool", 4);
    EXPECT_EQ(pool.num_threads(), 4);
    EXPECT_EQ(pool.name(), "test_pool");
    EXPECT_EQ(pool.status(), ThreadPool::Status::RUNNING);
    EXPECT_EQ(pool.low_latency_hint(), true);
    EXPECT_EQ(pool.num_active_threads(), 0);
    EXPECT_EQ(pool.num_pending_tasks(), 0);
  }
}

TEST(ThreadPool, TestSchedule) {
  for (int32_t i = 0; i < TEST_COUNT; ++i) {
    ThreadPool pool(Env::Default(), ThreadOptions(), "test_pool", 4, true);

    int32_t value = 0;
    pool.schedule([&value]() { value = 1; });
    pool.wait();
    EXPECT_EQ(value, 1);
  }
}

TEST(ThreadPool, TestMultipleSchedule) {
  int64_t num_threads = 10;
  int64_t num_tasks = 20;
  for (int32_t i = 0; i < TEST_COUNT; ++i) {
    ThreadPool pool(Env::Default(), ThreadOptions(), "test_pool", num_threads,
                    true);

    std::atomic_int32_t value;
    value = 0;

    for (uint32_t j = 0; j < num_tasks; ++j) {
      pool.schedule([&value]() { value++; });
    }
    pool.wait();
    EXPECT_EQ(value, num_tasks);
  }
}

TEST(ThreadPool, TestNumShardsUsedByFixedBlockSizeSchedule) {
  for (int32_t i = 0; i < TEST_COUNT; ++i) {
    ThreadPool pool(Env::Default(), ThreadOptions(), "test_pool", 4, true);
    EXPECT_EQ(pool.num_shards_used_by_fixed_block_size_scheduling(10, 3), 4);
    EXPECT_EQ(pool.num_shards_used_by_fixed_block_size_scheduling(10, 4), 3);
    EXPECT_EQ(pool.num_shards_used_by_fixed_block_size_scheduling(10, 5), 2);
    EXPECT_EQ(pool.num_shards_used_by_fixed_block_size_scheduling(9, 2), 5);
  }
}

TEST(ThreadPool, TestParralelForWithFixedBlocksStrategy) {
  int64_t num_threads = 10;
  int64_t total = 4324;      // Could be any number.
  int64_t block_size = 327;  // Could be any number.
  for (int32_t i = 0; i < TEST_COUNT; ++i) {
    ThreadPool pool(Env::Default(), ThreadOptions(), "test_pool", num_threads,
                    true);

    auto *data = new int64_t[total];

    std::function<void(int64_t, int64_t)> init_data = [&data](int64_t start,
                                                              int64_t end) {
      for (int64_t i = start; i < end; ++i) data[i] = 0;
    };

    std::function<void(int64_t, int64_t)> assign_data = [&data](int64_t start,
                                                                int64_t end) {
      for (int64_t i = start; i < end; ++i) data[i] = i;
    };

    pool.parallel_for_with_fixed_block(total, block_size, init_data);

    for (int64_t i = 0; i < total; ++i) {
      ASSERT_EQ(data[i], 0);
    }

    pool.parallel_for(total,
                      ThreadPool::SchedulingParams(
                          ThreadPool::SchedulingStrategy::FIXED_BLOCK_SIZE,
                          nullopt, block_size),
                      assign_data);
    for (int64_t i = 0; i < total; ++i) {
      ASSERT_EQ(data[i], i);
    }
  }
}

TEST(ThreadPool, TestParralelForWithAdaptiveStrategy) {
  int64_t num_threads = 20;
  int64_t total = 10000;  // Could be any number.
  /// Make parrallel_for use as many threads as possible.
  int64_t cost_per_unit = 1 << 30;

  for (int32_t i = 0; i < TEST_COUNT; ++i) {
    ThreadPool pool(Env::Default(), ThreadOptions(), "test_pool", num_threads,
                    true);

    auto *data = new int64_t[total];
    std::function<void(int64_t, int64_t)> init_data = [&data](int64_t start,
                                                              int64_t end) {
      for (int64_t i = start; i < end; ++i) data[i] = 0;
    };
    std::function<void(int64_t, int64_t)> assign_data = [&data](int64_t start,
                                                                int64_t end) {
      for (int64_t i = start; i < end; ++i) data[i] = i;
    };
    pool.parallel_for(total, cost_per_unit, init_data);
    for (int64_t i = 0; i < total; ++i) {
      ASSERT_EQ(data[i], 0);
    }
    pool.parallel_for(
        total,
        ThreadPool::SchedulingParams(ThreadPool::SchedulingStrategy::ADAPTIVE,
                                     cost_per_unit, nullopt),
        assign_data);
    for (int64_t i = 0; i < total; ++i) {
      ASSERT_EQ(data[i], i);
    }
  }
}

}  // namespace platform
}  // namespace chime
