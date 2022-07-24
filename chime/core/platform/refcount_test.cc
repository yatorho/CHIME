// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

#include "chime/core/platform/refcount.h"

#include <atomic>

#include "chime/core/platform/env.hpp"
#include "chime/core/platform/test.hpp"

namespace chime {
namespace core {

class RefTest : public ::testing::Test {
 public:
  RefTest() {
    constructed = 0;
    destroyed = 0;
  }

  static int constructed;
  static int destroyed;
};

int RefTest::constructed;
int RefTest::destroyed;

class MyRef : public RefCounted {
 public:
  MyRef() { RefTest::constructed++; }
  ~MyRef() { RefTest::destroyed++; }
};

TEST_F(RefTest, New) {
  MyRef *ref = new MyRef;
  ASSERT_EQ(1, constructed);
  ASSERT_EQ(0, destroyed);
  EXPECT_TRUE(ref->Unref());
  ASSERT_EQ(1, constructed);
  ASSERT_EQ(1, destroyed);
}

TEST_F(RefTest, RefUnref) {
  MyRef *ref = new MyRef;
  ASSERT_EQ(1, constructed);
  ASSERT_EQ(0, destroyed);
  ref->Ref();
  ASSERT_EQ(0, destroyed);
  ref->Unref();
  ASSERT_EQ(0, destroyed);
  ref->Unref();
  ASSERT_EQ(1, destroyed);
}

TEST_F(RefTest, RefCountOne) {
  MyRef *ref = new MyRef;
  ASSERT_TRUE(ref->RefCountIsOne());
  ref->Unref();
}

TEST_F(RefTest, RefCountNotOne) {
  MyRef *ref = new MyRef;
  ref->Ref();
  ASSERT_FALSE(ref->RefCountIsOne());
  ASSERT_FALSE(ref->Unref());
  ASSERT_TRUE(ref->Unref());
}

TEST_F(RefTest, ScopedUnref) {
  {
    MyRef *ref = new MyRef;
    ScopedUnref unref(ref);
  }
  EXPECT_EQ(1, destroyed);
}

class ObjType : public WeakRefCounted {};

TEST(WeakPtr, SingleThread) {
  auto obj = new ObjType();
  EXPECT_EQ(obj->RefCount(), 1);
  WeakPtr<ObjType> weak_ptr(obj);

  ASSERT_TRUE(obj->RefCountIsOne());
  EXPECT_EQ(obj->WeakRefCount(), 1);
  EXPECT_NE(weak_ptr.GetNewRef(), nullptr);

  obj->Unref();
  EXPECT_EQ(weak_ptr.GetNewRef(), nullptr);
}

TEST(WeakPtr, MultiThreadWeakRef) {
  std::atomic<int> hit_destructed{0};

  auto env = platform::Env::Default();

  for (int i = 0; i < 1; i++) {
    auto obj = new ObjType();
    WeakPtr<ObjType> weak_ptr(obj);

    bool obj_destroyed = false;
    EXPECT_EQ(obj->WeakRefCount(), 1);

    auto fn = [&]() {
      auto ref = weak_ptr.GetNewRef();
      if (ref != nullptr) {
        EXPECT_EQ(ref.get(), obj);
        EXPECT_EQ(ref->WeakRefCount(), 1);
        EXPECT_GE(ref->RefCount(), 1);
      } else {
        hit_destructed++;
        EXPECT_TRUE(obj_destroyed);
      }
    };

    auto t1 = env->StartThread(platform::ThreadOptions{}, "thread-1", fn);
    auto t2 = env->StartThread(platform::ThreadOptions{}, "thread-2", fn);

    env->SleepForMicroseconds(10);
    obj_destroyed = true;
    obj->Unref();

    delete t1;
    delete t2;

    EXPECT_EQ(weak_ptr.GetNewRef(), nullptr);
  }

  if (hit_destructed == 0) {
    LOG(WARNING) << "The destructed weakref test branch is not exercised.";
  }
  if (hit_destructed == 200) {
    LOG(WARNING) << "The valid weakref test branch is not exercised.";
  }
}

TEST(WeakPtr, NotifyCalled) {
  auto obj = new ObjType();
  int num_calls1 = 0;
  int num_calls2 = 0;

  auto notify_fn1 = [&num_calls1]() { num_calls1++; };
  auto notify_fn2 = [&num_calls2]() { num_calls2++; };

  WeakPtr<ObjType> weakptr1(obj, notify_fn1);
  WeakPtr<ObjType> weakptr2(obj, notify_fn2);

  ASSERT_TRUE(obj->RefCountIsOne());
  EXPECT_EQ(obj->WeakRefCount(), 2);
  EXPECT_NE(weakptr1.GetNewRef(), nullptr);
  EXPECT_NE(weakptr2.GetNewRef(), nullptr);

  ASSERT_EQ(num_calls1, 0);
  ASSERT_EQ(num_calls2, 0);

  obj->Unref();
  EXPECT_EQ(weakptr1.GetNewRef(), nullptr);
  EXPECT_EQ(weakptr2.GetNewRef(), nullptr);
  EXPECT_EQ(num_calls1, 1);
  EXPECT_EQ(num_calls2, 1);
}

TEST(WeakPtr, MoveTargetNotCalled) {
  auto obj = new ObjType();
  int num_calls1 = 0;
  int num_calls2 = 0;
  int num_calls3 = 0;

  auto notify_fn1 = [&num_calls1]() { num_calls1++; };
  auto notify_fn2 = [&num_calls2]() { num_calls2++; };
  auto notify_fn3 = [&num_calls3]() { num_calls3++; };
  WeakPtr<ObjType> weakptr1(obj, notify_fn1);
  WeakPtr<ObjType> weakptr2(obj, notify_fn2);
  WeakPtr<ObjType> weakptr3(WeakPtr<ObjType>(obj, notify_fn3));

  weakptr2 = std::move(weakptr1);

  ASSERT_TRUE(obj->RefCountIsOne());
  EXPECT_EQ(obj->WeakRefCount(), 2);
  EXPECT_NE(weakptr2.GetNewRef(), nullptr);
  EXPECT_NE(weakptr3.GetNewRef(), nullptr);

  EXPECT_EQ(num_calls1, 0);
  EXPECT_EQ(num_calls2, 0);
  EXPECT_EQ(num_calls3, 0);
  obj->Unref();
  EXPECT_EQ(weakptr2.GetNewRef(), nullptr);
  EXPECT_EQ(weakptr3.GetNewRef(), nullptr);
  EXPECT_EQ(num_calls1, 1);
  EXPECT_EQ(num_calls2, 0);
  EXPECT_EQ(num_calls3, 1);
}

TEST(WeakPtr, DestroyedNotifyNotCalled) {
  auto obj = new ObjType();
  int num_calls = 0;
  auto notify_fn = [&num_calls]() { num_calls++; };
  { WeakPtr<ObjType> weakptr(obj, notify_fn); }
  ASSERT_TRUE(obj->RefCountIsOne());
  EXPECT_EQ(obj->WeakRefCount(), 0);

  EXPECT_EQ(num_calls, 0);
  obj->Unref();
  EXPECT_EQ(num_calls, 0);
}

}  // namespace core
}  // namespace chime
