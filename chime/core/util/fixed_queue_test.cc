// Copyright by 2022.6 chime
// author: yatorho

#include "chime/core/util/fixed_queue.hpp"

#include <thread>

#include "chime/core/framework/common.hpp"

namespace chime {
namespace concurrent {

TEST(FixedQueue, TestCommon) {
  FixedQueue<int> queue;
  EXPECT_EQ(queue.size(), 0);
  EXPECT_TRUE(queue.empty());
  queue.push(1);
  EXPECT_EQ(queue.size(), 1);
  EXPECT_FALSE(queue.empty());
  queue.push(2);
  EXPECT_EQ(queue.size(), 2);
  EXPECT_FALSE(queue.empty());
  EXPECT_EQ(queue.front(), 1);
  EXPECT_EQ(queue.back(), 2);
  queue.pop();
  EXPECT_EQ(queue.size(), 1);
  EXPECT_FALSE(queue.empty());
  EXPECT_EQ(queue.front(), 2);
  EXPECT_EQ(queue.back(), 2);
  queue.pop();
  EXPECT_EQ(queue.size(), 0);
  EXPECT_TRUE(queue.empty());
}

struct Object {
  int x;
  int y;

  Object(int x, int y) : x(x), y(y) {}
  Object() : x(0), y(0) {}
};

TEST(FixedQueue, TestObject) {
  FixedQueue<Object> queue;
  EXPECT_EQ(queue.size(), 0);
  EXPECT_TRUE(queue.empty());
  Object obj;
  obj.x = 1;
  obj.y = 2;
  queue.push(obj);
  EXPECT_EQ(queue.size(), 1);
  EXPECT_FALSE(queue.empty());
  EXPECT_EQ(queue.front().y, 2);
  EXPECT_EQ(queue.back().x, 1);
  queue.pop();
  EXPECT_EQ(queue.size(), 0);
  EXPECT_TRUE(queue.empty());
}

TEST(FixedQueue, MoveObject) {
  FixedQueue<Object> queue;
  EXPECT_EQ(queue.size(), 0);
  EXPECT_TRUE(queue.empty());
  Object obj;
  obj.x = 1;
  obj.y = 2;
  queue.push(std::move(obj));
  EXPECT_EQ(queue.size(), 1);
  EXPECT_FALSE(queue.empty());
  EXPECT_EQ(queue.front().x, 1);
  EXPECT_EQ(queue.back().y, 2);
  queue.pop();
  EXPECT_EQ(queue.size(), 0);
  EXPECT_TRUE(queue.empty());
}

TEST(FixedQueue, TestMultiThread) {
  FixedQueue<int> queue;
  EXPECT_EQ(queue.size(), 0);
  EXPECT_TRUE(queue.empty());
  std::thread t1([&queue]() {
    for (int i = 0; i < 100; ++i) {
      queue.push(i);
    }
  });
  std::thread t2([&queue]() {
    for (int i = 0; i < 100; ++i) {
      queue.push(i);
    }
  });
  t1.join();
  t2.join();
  EXPECT_EQ(queue.size(), 200);
  EXPECT_EQ(queue.back(), 99);
  EXPECT_EQ(queue.front(), 0);
  for (int i = 0; i < 200; ++i) {
    queue.pop();
  }
  EXPECT_TRUE(queue.empty());
}

TEST(FixedQueue, TestMultiThreadObject) {
  FixedQueue<Object> queue;
  EXPECT_EQ(queue.size(), 0);
  EXPECT_TRUE(queue.empty());
  std::thread t1([&queue]() {
    for (int i = 0; i < 100; ++i) {
      Object obj;
      obj.x = i;
      obj.y = i;
      queue.push(obj);
    }
  });
  std::thread t2([&queue]() {
    for (int i = 0; i < 100; ++i) {
      Object obj;
      obj.x = i;
      obj.y = i;
      queue.push(obj);
    }
  });
  t1.join();
  t2.join();
  EXPECT_EQ(queue.size(), 200);
  EXPECT_EQ(queue.back().x, 99);
  EXPECT_EQ(queue.front().x, 0);
  for (int i = 0; i < 200; ++i) {
    queue.pop();
  }
  EXPECT_TRUE(queue.empty());
}

TEST(FixedQueue, TestMultiThreadMoveObject) {
  FixedQueue<Object> queue;
  EXPECT_EQ(queue.size(), 0);
  EXPECT_TRUE(queue.empty());
  std::thread t1([&queue]() {
    for (int i = 0; i < 100; ++i) {
      Object obj;
      obj.x = i;
      obj.y = i;
      queue.push(std::move(obj));
    }
  });
  std::thread t2([&queue]() {
    for (int i = 0; i < 100; ++i) {
      Object obj;
      obj.x = i;
      obj.y = i;
      queue.push(std::move(obj));
    }
  });
  t1.join();
  t2.join();
  EXPECT_EQ(queue.size(), 200);
  EXPECT_EQ(queue.back().x, 99);
  EXPECT_EQ(queue.front().x, 0);
  for (int i = 0; i < 200; ++i) {
    queue.pop();
  }
  EXPECT_TRUE(queue.empty());
}

TEST(FixedQueue, TestObjectEmplace) {
  FixedQueue<Object> queue;
  EXPECT_EQ(queue.size(), 0);
  EXPECT_TRUE(queue.empty());
  queue.emplace(1, 2);
  EXPECT_EQ(queue.size(), 1);
  EXPECT_FALSE(queue.empty());
  EXPECT_EQ(queue.front().y, 2);
  EXPECT_EQ(queue.back().x, 1);
  queue.pop();
  EXPECT_EQ(queue.size(), 0);
  EXPECT_TRUE(queue.empty());
}

TEST(FixedQueue, TestObjectMultiThreadEmplace) {
  FixedQueue<Object> queue;
  EXPECT_EQ(queue.size(), 0);
  EXPECT_TRUE(queue.empty());
  std::thread t1([&queue]() {
    for (int i = 0; i < 100; ++i) {
      queue.emplace(i, i);
    }
  });
  std::thread t2([&queue]() {
    for (int i = 0; i < 100; ++i) {
      queue.emplace(i, i);
    }
  });
  t1.join();
  t2.join();
  EXPECT_EQ(queue.size(), 200);
  EXPECT_EQ(queue.back().x, 99);
  EXPECT_EQ(queue.front().x, 0);
  for (int i = 0; i < 200; ++i) {
    queue.pop();
  }
  EXPECT_TRUE(queue.empty());
}

TEST(FixedQueue, TestStrictModel) {
  FixedQueue<int, true> queue;
  queue.push(1);
  queue.push(2);
  EXPECT_EQ(queue.front(), 1);
  EXPECT_EQ(queue.back(), 2);
}

TEST(FixedQueue, TestSwap) {
  FixedQueue<int> queue1;
  FixedQueue<int> queue2;
  queue1.push(1);
  queue1.push(2);
  queue2.push(3);
  queue2.push(4);
  queue1.swap(queue2);
  EXPECT_EQ(queue1.front(), 3);
  EXPECT_EQ(queue1.back(), 4);
  EXPECT_EQ(queue2.front(), 1);
  EXPECT_EQ(queue2.back(), 2);
}

TEST(FixedQueue, TestSwapObjectEmplace) {
  FixedQueue<Object> queue1;
  FixedQueue<Object> queue2;
  queue1.emplace(1, 2);
  queue1.emplace(3, 4);
  queue2.emplace(5, 6);
  queue2.emplace(7, 8);
  queue1.swap(queue2);
  EXPECT_EQ(queue1.front().x, 5);
  EXPECT_EQ(queue1.front().y, 6);
}

}  // namespace concurrent
}  // namespace chime
