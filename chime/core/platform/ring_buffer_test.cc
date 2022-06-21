// Copyright by 2022.6 chime
// author: yatorho

#include "chime/core/platform/ring_buffer.hpp"

#include <atomic>
#include <functional>
#include <thread>

namespace chime {
namespace platform {

struct Object {
  int x;
  int y;
  int z;
};

TEST(RingBuffer, TestConstructorandIsEmpty) {
  RingBuffer<int> ring_buffer(4);
  EXPECT_FALSE(ring_buffer.is_full());
  EXPECT_TRUE(ring_buffer.is_empty());
  EXPECT_EQ(ring_buffer.capacity(), 4);
}

TEST(RingBuffer, TestIsFull) {
  RingBuffer<int> ring_buffer(10);
  auto *data = new int[11];
  for (int32_t i = 0; i < 10; i++) {
    data[i] = i;
    EXPECT_TRUE(ring_buffer.add_element(&data[i]));
    if (i != 9) EXPECT_FALSE(ring_buffer.is_full());
  }
  EXPECT_TRUE(ring_buffer.is_full());
  data[10] = 10;
  EXPECT_FALSE(ring_buffer.add_element(&data[10]));
}

TEST(RingBuffer, TestIsFullMultiThread) {
  const int count = 500;
  auto *data = new Object[count];
  RingBuffer<Object> ring_buffer(count);

  std::atomic_uint over_count;
  over_count = 0;
  std::thread thread1([&data, &ring_buffer, &over_count, &count]() {
    for (int32_t i = 0; i < count; i++) {
      if (!ring_buffer.add_element(&data[i])) ++over_count;
    }
  });
  std::thread thread2([&data, &ring_buffer, &over_count, &count]() {
    for (int32_t i = 0; i < count; i++) {
      if (!ring_buffer.add_element(&data[i])) ++over_count;
    }
  });
  std::thread thread3([&data, &ring_buffer, &over_count, &count]() {
    for (int32_t i = 0; i < count; i++) {
      if (!ring_buffer.add_element(&data[i])) ++over_count;
    }
  });

  thread1.join();
  thread2.join();
  thread3.join();
  EXPECT_EQ(over_count, 2 * count);
}

TEST(RingBuffer, TestConsume) {
  const int count = 500;
  auto *data = new Object[count];
  Object *d_ptr = nullptr;
  RingBuffer<Object> ring_buffer(count / 2);

  std::atomic_uint over_count;
  over_count = 0;

  std::thread thread1([&data, &ring_buffer, &over_count, &count]() {
    for (int32_t i = 0; i < count; i++) {
      while (!ring_buffer.add_element(&data[i]))
        ;
    }
  });

  std::thread thread2([&data, &ring_buffer, &over_count, &count, &d_ptr]() {
    for (int32_t i = 0; i < count / 2; i++) {
      while (!ring_buffer.get_element(&d_ptr))
        ;
    }
  });
  thread1.join();
  thread2.join();

  EXPECT_TRUE(ring_buffer.is_full());
}

}  // namespace platform
}  // namespace chime
