// Copyright by 2022.7 chime
// author: yatorho

#include "chime/core/memory/mem.h"

#include "chime/core/platform/test.hpp"

namespace chime {

TEST(MemTest, AlignedMalloc) {
  void *ptr = port::AlignedMalloc(100, 64);
  ASSERT_NE(ptr, nullptr);
  port::AlignedFree(ptr);
}

TEST(MemTest, Malloc) {
  void *ptr = port::Malloc(100);
  ASSERT_NE(ptr, nullptr);
  port::Free(ptr);
}

TEST(MemTest, Realloc) {
  void *ptr = port::Malloc(100);
  ASSERT_NE(ptr, nullptr);
  ptr = port::Realloc(ptr, 200);
  ASSERT_NE(ptr, nullptr);
  port::Free(ptr);
}

}  // namespace chime
