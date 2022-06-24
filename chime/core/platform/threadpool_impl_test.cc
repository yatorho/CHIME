// Copyright by 2022.6 chime
// author: yatorho

#include "chime/core/platform/threadpool_impl.h"

#include <atomic>
#include <thread>

#include "chime/core/platform/task.hpp"

namespace chime {
namespace platform {

static const int TEST_COUNT = 300;

TEST(ThreadPoolImpl, TestConstructor) {
  ThreadPoolImpl pool(Env::Default(), ThreadOptions(), "test", 10, false);
  EXPECT_EQ(pool.status(), ThreadPoolImpl::UNINITIALIZED);
  EXPECT_EQ(pool.num_threads(), 10);
  EXPECT_EQ(pool.name(), "test");
  EXPECT_EQ(pool.low_latency_hint(), false);
  EXPECT_EQ(pool.num_active_workers(), 0);
  EXPECT_EQ(pool.num_pending_tasks(), 0);
}

TEST(ThreadPoolImpl, TestInit) {
  ThreadPoolImpl pool(Env::Default(), ThreadOptions(), "test", 10, false);
  pool.init();
  EXPECT_EQ(pool.status(), ThreadPoolImpl::RUNNING);
  EXPECT_EQ(pool.num_threads(), 10);
  EXPECT_EQ(pool.name(), "test");
  EXPECT_EQ(pool.low_latency_hint(), false);
  EXPECT_EQ(pool.num_active_workers(), 0);
  EXPECT_EQ(pool.num_pending_tasks(), 0);
}

TEST(ThreadPoolImpl, TestSchedule) {
  for (int i = 0; i < TEST_COUNT; i++) {
    ThreadPoolImpl pool(Env::Default(), ThreadOptions(), "test", 10, false);
    pool.init();

    int value = 0;
    pool.schedule([&value]() {
      for (int j = 0; j < 10000; ++j) value++;
    });
    pool.wait();
    EXPECT_EQ(value, 10000);
  }
}

TEST(ThreadPoolImpl, TestScheduleWithIntPointer) {
  for (int i = 0; i < TEST_COUNT; i++) {
    ThreadPoolImpl pool(Env::Default(), ThreadOptions(), "test", 10, false);
    pool.init();

    int value = 0;
    int *p = &value;
    pool.schedule([p]() {
      for (int j = 0; j < 10000; ++j) (*p)++;
    });
    pool.wait();
    pool.schedule([p]() {
      for (int j = 0; j < 10000; ++j) (*p)++;
    });
    pool.wait();
    EXPECT_EQ(value, 20000);
  }
}

struct Object {
  int x;
  int y;
  Object(int x, int y) : x(x), y(y) {}
  Object() : x(0), y(0) {}
};

TEST(ThreadPoolImpl, TestScheduleWithObject) {
  for (int i = 0; i < TEST_COUNT; i++) {
    ThreadPoolImpl pool(Env::Default(), ThreadOptions(), "test", 10, false);
    pool.init();

    Object obj(0, 0);
    pool.schedule([&obj]() {
      for (int j = 0; j < 10000; ++j) {
        obj.x++;
        obj.y--;
      }
    });
    pool.wait();
    EXPECT_EQ(obj.x, 10000);
    EXPECT_EQ(obj.y, -10000);
  }
}

// Simple AddTask test.
class AddOperatorTask : public Task {
 public:
  AddOperatorTask(int *a, int *b, int *c, int32_t start, int32_t end)
      : a_(a), b_(b), c_(c), start_(start), end_(end) {}

  AddOperatorTask()
      : a_(nullptr), b_(nullptr), c_(nullptr), start_(0), end_(0) {}

  AddOperatorTask(int *a, int *b, int *c, int32_t len)
      : a_(a), b_(b), c_(c), start_(0), end_(len) {}

  ~AddOperatorTask() override {}

  void run() override {
    for (int i = start_; i < end_; i++) {
      c_[i] = a_[i] + b_[i];
    }
  }

 private:
  int *a_;
  int *b_;
  int *c_;
  int32_t start_;
  int32_t end_;
};

TEST(ThreadPoolImpl, TestScheduleWithAddOperator) {
  for (int i = 0; i < TEST_COUNT; i++) {
    ThreadPoolImpl pool(Env::Default(), ThreadOptions(), "test", 10, false);
    pool.init();

    int a[10000];
    int b[10000];
    int c[10000];
    for (int i = 0; i < 10000; i++) {
      a[i] = i;
      b[i] = i;
    }
    AddOperatorTask task(a, b, c, 10000);
    pool.schedule([&task] { task.run(); });
    pool.wait();
    for (int i = 0; i < 10000; i++) {
      EXPECT_EQ(c[i], 2 * i);
    }
  }
}

TEST(ThreadPoolImp, TestMultipleScheduleAddOperator) {
  int64_t thread_count = 10;
  for (int i = 0; i < TEST_COUNT; i++) {
    ThreadPoolImpl pool(Env::Default(), ThreadOptions(), "test", 10, false);
    pool.init();

    int *a = new int[10000];
    int *b = new int[10000];
    int *c = new int[10000];
    for (int i = 0; i < 10000; i++) {
      a[i] = i;
      b[i] = i;
    }
    auto tasks = new AddOperatorTask[10];

    for (int i = 0; i < 10; i++) {
      tasks[i] = AddOperatorTask(a, b, c, i * 1000, (i + 1) * 1000);
      pool.schedule([i, &tasks]() { tasks[i].run(); });
    }
    pool.wait();
    for (int i = 0; i < 10000; i++) {
      EXPECT_EQ(c[i], 2 * i);
    }
  }
}

TEST(ThreadPoolImpl, TestMultipleSchedule) {
  int64_t thread_count = 20;

  for (uint32_t i = 0; i < TEST_COUNT; i++) {
    ThreadPoolImpl pool(Env::Default(), ThreadOptions(), "test", thread_count,
                        false);
    pool.init();

    std::atomic<int64_t> value;
    value = 0;

    for (int i = 0; i < 2 * thread_count; i++) {
      pool.schedule([&value]() { value++; });
    }
    pool.wait();
    EXPECT_EQ(value, 2 * thread_count);
  }
}

}  // namespace platform

}  // namespace chime