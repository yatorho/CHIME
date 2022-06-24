// Copyright by 2022.6 chime
// author: yatorho

#include "chime/core/util/ring_queue.hpp"

#include <thread>

#include "chime/core/framework/common.hpp"
#include "chime/core/platform/env.hpp"

namespace chime {
namespace concurrent {

struct Object {
  int x;
  int y;
  int z;
};

TEST(RingQueue, TestConstructorandIsEmpty) {
  RingQueue<int> ring_queue(4);
  EXPECT_FALSE(ring_queue.full());
  EXPECT_TRUE(ring_queue.empty());
  EXPECT_EQ(ring_queue.capacity(), 4);
}

TEST(RingQueue, TestFull) {
  RingQueue<int> ring_queue(4);
  EXPECT_FALSE(ring_queue.full());
  EXPECT_TRUE(ring_queue.empty());
  EXPECT_EQ(ring_queue.capacity(), 4);
  ring_queue.add_element(1);
  EXPECT_FALSE(ring_queue.full());
  EXPECT_FALSE(ring_queue.empty());
  ring_queue.add_element(2);
  ring_queue.add_element(3);
  ring_queue.add_element(4);
  EXPECT_TRUE(ring_queue.full());
  EXPECT_FALSE(ring_queue.empty());
  EXPECT_EQ(ring_queue.capacity(), 4);
  EXPECT_FALSE(ring_queue.add_element(5));
}

TEST(RingQueue, TestIsFullMultiThread) {
  const int count = 500;
  auto *data = new Object[count];
  RingQueue<Object *> ring_queue(count);

  std::atomic_uint over_count;
  over_count = 0;
  std::thread thread1([&data, &ring_queue, &over_count, &count]() {
    for (int32_t i = 0; i < count; i++) {
      if (!ring_queue.add_element(&data[i])) ++over_count;
    }
  });
  std::thread thread2([&data, &ring_queue, &over_count, &count]() {
    for (int32_t i = 0; i < count; i++) {
      if (!ring_queue.add_element(&data[i])) ++over_count;
    }
  });
  std::thread thread3([&data, &ring_queue, &over_count, &count]() {
    for (int32_t i = 0; i < count; i++) {
      if (!ring_queue.add_element(&data[i])) ++over_count;
    }
  });

  thread1.join();
  thread2.join();
  thread3.join();
  EXPECT_TRUE(ring_queue.full());
  EXPECT_EQ(over_count, 2 * count);
}

TEST(RingQueue, TestConsume) {
  const int count = 500;
  auto *data = new Object[count];
  Object *d_ptr = nullptr;
  RingQueue<Object *> ring_queue(count / 2);

  std::atomic_uint over_count;
  over_count = 0;

  std::thread thread1([&data, &ring_queue, &over_count, &count]() {
    for (int32_t i = 0; i < count; i++) {
      while (!ring_queue.add_element(&data[i]))
        ;
    }
  });

  std::thread thread2([&data, &ring_queue, &over_count, &count, &d_ptr]() {
    for (int32_t i = 0; i < count / 2; i++) {
      while (!ring_queue.get_element(&d_ptr))
        ;
    }
  });
  thread1.join();
  thread2.join();

  EXPECT_TRUE(ring_queue.full());
}

// Test Thread* enqueue RingQueue and dequeue RingQueue
TEST(RingQueue, TestEnvThreadQueue) {
  using platform::Env;
  using platform::Thread;
  using platform::ThreadOptions;
  std::atomic<int> value;
  value = 0;

  Env *env = Env::Default();

  RingQueue<Thread *> ring(10);
  for (int i = 0; i < 10; ++i) {
    ring.add_element(
        env->start_thread(ThreadOptions(), "test", [&value]() { value++; }));
  }

  Thread *thread;

  for (int i = 0; i < 10; ++i) {
    ring.get_element(&thread);
    thread->~Thread();
  }
  EXPECT_EQ(value, 10);
}
}  // namespace concurrent
}  // namespace chime
