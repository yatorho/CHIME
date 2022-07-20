// Copyright by 2022.6 chime
// author: yatorho

#include "chime/core/platform/threadpool_impl.h"

#include <atomic>
#include <thread>

#include "chime/core/platform/task.hpp"
#include "chime/core/platform/test.hpp"

namespace chime {
namespace platform {

static const int TEST_COUNT = 300;

TEST(ThreadPoolImpl, TestConstructor) {
  ThreadPoolImpl pool(Env::Default(), ThreadOptions(), "test", 10, false);
  EXPECT_EQ(pool.GetStatus(), ThreadPoolImpl::UNINITIALIZED);
  EXPECT_EQ(pool.NumThreads(), 10);
  EXPECT_EQ(pool.Name(), "test");
  EXPECT_EQ(pool.LowLatencyHint(), false);
  EXPECT_EQ(pool.NumActiveWorkers(), 0);
  EXPECT_EQ(pool.NumPendingTasks(), 0);
}

TEST(ThreadPoolImpl, TestInit) {
  ThreadPoolImpl pool(Env::Default(), ThreadOptions(), "test", 10, false);
  pool.Init();
  EXPECT_EQ(pool.GetStatus(), ThreadPoolImpl::RUNNING);
  EXPECT_EQ(pool.NumThreads(), 10);
  EXPECT_EQ(pool.Name(), "test");
  EXPECT_EQ(pool.LowLatencyHint(), false);
  EXPECT_EQ(pool.NumActiveWorkers(), 0);
  EXPECT_EQ(pool.NumPendingTasks(), 0);
}

TEST(ThreadPoolImpl, TestSchedule) {
  for (int i = 0; i < TEST_COUNT; i++) {
    ThreadPoolImpl pool(Env::Default(), ThreadOptions(), "test", 10, false);
    pool.Init();

    int value = 0;
    pool.Schedule([&value]() {
      for (int j = 0; j < 10000; ++j) value++;
    });
    pool.Wait();
    EXPECT_EQ(value, 10000);
  }
}

TEST(ThreadPoolImpl, TestScheduleWithIntPointer) {
  for (int i = 0; i < TEST_COUNT; i++) {
    ThreadPoolImpl pool(Env::Default(), ThreadOptions(), "test", 10, false);
    pool.Init();

    int value = 0;
    int *p = &value;
    pool.Schedule([p]() {
      for (int j = 0; j < 10000; ++j) (*p)++;
    });
    pool.Wait();
    pool.Schedule([p]() {
      for (int j = 0; j < 10000; ++j) (*p)++;
    });
    pool.Wait();
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
    pool.Init();

    Object obj(0, 0);
    pool.Schedule([&obj]() {
      for (int j = 0; j < 10000; ++j) {
        obj.x++;
        obj.y--;
      }
    });
    pool.Wait();
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

  void Run() override {
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
    pool.Init();

    int a[10000];
    int b[10000];
    int c[10000];
    for (int i = 0; i < 10000; i++) {
      a[i] = i;
      b[i] = i;
    }
    AddOperatorTask task(a, b, c, 10000);
    pool.Schedule([&task] { task.Run(); });
    pool.Wait();
    for (int i = 0; i < 10000; i++) {
      EXPECT_EQ(c[i], 2 * i);
    }
  }
}

TEST(ThreadPoolImp, TestMultipleScheduleAddOperator) {
  int64_t thread_count = 10;
  for (int i = 0; i < TEST_COUNT; i++) {
    ThreadPoolImpl pool(Env::Default(), ThreadOptions(), "test", thread_count,
                        false);
    pool.Init();

    int *a = new int[10000];
    int *b = new int[10000];
    int *c = new int[10000];
    for (int i = 0; i < 10000; i++) {
      a[i] = i;
      b[i] = 2 * i;
    }
    auto tasks = new AddOperatorTask[20];

    for (int i = 0; i < 20; i++) {
      tasks[i] = AddOperatorTask(a, b, c, 500 * i, 500 * (i + 1));
      pool.Schedule([i, &tasks]() { tasks[i].Run(); });
    }
    // test following code's execution time
    pool.Wait();
    for (int i = 0; i < 10000; i++) {
      ASSERT_EQ(c[i], 3 * i);
    }
    delete[] a;
    delete[] b;
    delete[] c;
  }
}

TEST(ThreadPoolImpl, TestMultipleSchedule) {
  int64_t thread_count = 20;

  for (uint32_t i = 0; i < TEST_COUNT; i++) {
    ThreadPoolImpl pool(Env::Default(), ThreadOptions(), "test", thread_count,
                        false);
    pool.Init();

    std::atomic<int64_t> value;
    value = 0;

    for (int i = 0; i < 2 * thread_count; i++) {
      pool.Schedule([&value]() { value++; });
    }
    pool.Wait();
    EXPECT_EQ(value, 2 * thread_count);
  }
}

}  // namespace platform
}  // namespace chime
