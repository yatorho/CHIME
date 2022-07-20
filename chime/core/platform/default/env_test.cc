// Copyright by 2022.6 chime
// author: yatorho

#include "chime/core/platform/env.hpp"

#include "chime/core/framework/types.hpp"
#include "chime/core/platform/task.hpp"
#include "chime/core/platform/test.hpp"

namespace chime {
namespace platform {

TEST(Env, PosixEnvCreateThread) {
  int value = 0;
  Env *env = Env::Default();
  Thread *thread =
      env->StartThread(ThreadOptions(), "test_thread", [&value]() {
        for (int32 i = 0; i < 10; i++) {
          value++;
        }
      });
  delete thread;
  EXPECT_EQ(value, 10);
}

TEST(Env, PosixenvGetCurrentThreadId) {
  Env *env = Env::Default();
  env->GetCurrentThreadID();
}

// Simple add function task.
class TestTask : public Task {
 public:
  TestTask(int *a, int *b, int *c, int len) : _a(a), _b(b), _c(c), _len(len) {}

  void Run() override {
    for (int32_t i = 0; i < _len; i++) {
      _c[i] = _a[i] + _b[i];
    }
  }

  ~TestTask() {}

 private:
  int *_a;
  int *_b;
  int *_c;
  int _len;
};

TEST(Env, TaskThread) {
  const int length = 10;
  int *a = ::new int[length];
  int *b = ::new int[length];
  int *c = ::new int[length];

  for (int32 i = 0; i < length; ++i) {
    a[i] = i;
    b[i] = i;
    c[i] = 0;
  }

  TestTask *test_task = new TestTask(a, b, c, length);

  Env *env = Env::Default();
  Thread *thread = env->StartThread(ThreadOptions(), "add_task_test",
                                     [&test_task]() { test_task->Run(); });
  delete thread;

  for (int32 i = 0; i < length; ++i) {
    EXPECT_EQ(c[i], 2 * i);
  }
}

TEST(Env, EnvTime) {
  uint64_t now_nanos = EnvTime::NowNanos();
  uint64_t now_micros = EnvTime::NowMicros();
  uint64_t now_seconds = EnvTime::NowSeconds();
  EXPECT_GT(now_nanos, 0);
  EXPECT_GT(now_micros, 0);
  EXPECT_GT(now_seconds, 0);
}

}  // namespace platform
}  // namespace chime
