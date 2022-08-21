// Copyright by 2022.6 chime
// author: yatorho

#include "chime/core/platform/threadpool.h"

#include <functional>

#include "chime/core/platform/env.hpp"
#include "chime/core/platform/test.hpp"
#include "chime/core/util/optional.hpp"

namespace chime {
namespace platform {

static const int32_t TEST_COUNT = 300;

TEST(ThreadPool, TestConstructor) {
  for (int32_t i = 0; i < TEST_COUNT; ++i) {
    ThreadPool pool(Env::Default(), "test_pool", 4);
    EXPECT_EQ(pool.NumThreads(), 4);
    EXPECT_EQ(pool.Name(), "test_pool");
    EXPECT_EQ(pool.GetStatus(), ThreadPool::Status::RUNNING);
    EXPECT_EQ(pool.LowLatencyHint(), true);
    EXPECT_EQ(pool.NumActiveThreads(), 0);
    EXPECT_EQ(pool.NumPendingTasks(), 0);
  }
}

TEST(ThreadPool, TestSchedule) {
  for (int32_t i = 0; i < TEST_COUNT; ++i) {
    ThreadPool pool(Env::Default(), ThreadOptions(), "test_pool", 4, true);

    int32_t value = 0;
    pool.Schedule([&value]() { value = 1; });
    pool.Wait();
    EXPECT_EQ(value, 1);
  }
}

TEST(ThreadPool, TestMultipleSchedule) {
  int64_t NumThreads = 10;
  int64_t num_tasks = 20;
  for (int32_t i = 0; i < TEST_COUNT; ++i) {
    ThreadPool pool(Env::Default(), ThreadOptions(), "test_pool", NumThreads,
                    true);

    std::atomic_int32_t value;
    value = 0;

    for (uint32_t j = 0; j < num_tasks; ++j) {
      pool.Schedule([&value]() { value++; });
    }
    pool.Wait();
    EXPECT_EQ(value, num_tasks);
  }
}

TEST(ThreadPool, TestNumShardsUsedByFixedBlockSizeSchedule) {
  for (int32_t i = 0; i < TEST_COUNT; ++i) {
    ThreadPool pool(Env::Default(), ThreadOptions(), "test_pool", 4, true);
    EXPECT_EQ(pool.NumShardsUsedByFixedBlockSizeScheduling(10, 3), 4);
    EXPECT_EQ(pool.NumShardsUsedByFixedBlockSizeScheduling(10, 4), 3);
    EXPECT_EQ(pool.NumShardsUsedByFixedBlockSizeScheduling(10, 5), 2);
    EXPECT_EQ(pool.NumShardsUsedByFixedBlockSizeScheduling(9, 2), 5);
  }
}

TEST(ThreadPool, TestParralelForWithFixedBlocksStrategy) {
  int64_t NumThreads = 10;
  int64_t total = 4324;      // Could be any number.
  int64_t block_size = 327;  // Could be any number.
  for (int32_t i = 0; i < TEST_COUNT; ++i) {
    ThreadPool pool(Env::Default(), ThreadOptions(), "test_pool", NumThreads,
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

    pool.ParallelForWithFixedBlock(total, block_size, init_data);

    for (int64_t i = 0; i < total; ++i) {
      ASSERT_EQ(data[i], 0);
    }

    pool.ParallelFor(total,
                     ThreadPool::SchedulingParams(
                         ThreadPool::SchedulingStrategy::FIXED_BLOCK_SIZE,
                         util::nullopt, block_size),
                     assign_data);
    for (int64_t i = 0; i < total; ++i) {
      ASSERT_EQ(data[i], i);
    }
  }
}

TEST(ThreadPool, TestParralelForWithAdaptiveStrategy) {
  int64_t NumThreads = 20;
  int64_t total = 10000;  // Could be any number.
  /// Make parrallel_for use as many threads as possible.
  int64_t cost_per_unit = 1 << 30;

  for (int32_t i = 0; i < TEST_COUNT; ++i) {
    ThreadPool pool(Env::Default(), ThreadOptions(), "test_pool", NumThreads,
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
    pool.ParallelFor(total, cost_per_unit, init_data);
    for (int64_t i = 0; i < total; ++i) {
      ASSERT_EQ(data[i], 0);
    }
    pool.ParallelFor(
        total,
        ThreadPool::SchedulingParams(ThreadPool::SchedulingStrategy::ADAPTIVE,
                                     cost_per_unit, util::nullopt),
        assign_data);
    for (int64_t i = 0; i < total; ++i) {
      ASSERT_EQ(data[i], i);
    }
  }
}

TEST(ThreadPool, TotalLessThread) {
  int64_t NumTheads = 16;
  int64_t total = 4;

  const int large_total = 1000;

  /// Make parrallel_for use as many threads as possible.
  int64_t cost_per_unit = 1 << 30;

  int *data = new int[large_total];

  for (int i = 0; i < TEST_COUNT; i++) {
    for (int j = 0; j < large_total; j++) {
      data[j] = 0;
    }

    ThreadPool pool(Env::Default(), ThreadOptions(), "test_pool", NumTheads,
                    true);

    pool.ParallelFor(total, cost_per_unit, [&data](int start, int end) {
      for (int i = start; i < end; ++i) {
        data[i] = i;
      }
    });

    for (int j = 0; j < total; j++) {
      ASSERT_EQ(data[j], j) << "j = " << j << " data[j] = " << data[j];
    }
    for (int j = total; j < large_total; j++) {
      ASSERT_EQ(data[j], 0) << "j = " << j << " data[j] = " << data[j];
    }
  }
  delete[] data;
}

TEST(ThreadPool, TotalLessShard) {
  int64_t NumThreads = 16;
  int64_t total = 4;
  int64_t block_size = 32;

  const int large_total = 1000;

  int *data = new int[large_total];

  for (int i = 0; i < TEST_COUNT; i++) {
    for (int j = 0; j < large_total; j++) {
      data[j] = 0;
    }
    ThreadPool pool(Env::Default(), ThreadOptions(), "test_pool", NumThreads,
                    true);
    pool.ParallelFor(total,
                     ThreadPool::SchedulingParams(
                         ThreadPool::SchedulingStrategy::FIXED_BLOCK_SIZE,
                         util::nullopt, block_size),
                     [&data](int start, int end) {
                       for (int i = start; i < end; ++i) {
                         data[i] = i;
                       }
                     });
    for (int j = 0; j < total; j++) {
      ASSERT_EQ(data[j], j) << "j = " << j << " data[j] = " << data[j];
    }
    for (int j = total; j < large_total; j++) {
      ASSERT_EQ(data[j], 0) << "j = " << j << " data[j] = " << data[j];
    }
  }
  delete[] data;
}

}  // namespace platform
}  // namespace chime
