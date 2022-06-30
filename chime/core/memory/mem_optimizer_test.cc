// Copyright by 2022.5 chime
// author: yatorho

#include "chime/core/memory/mem_optimizer.h"
#include "chime/core/platform/test.hpp"

namespace chime {
namespace memory {
class TestMemoryOptimizer : public ::testing::Test {};

TEST_F(TestMemoryOptimizer, TestDefaultAllocator) {
  DefaultAllocator &mo = DefaultAllocator::get_instance();

  void *ptr = nullptr, *ptr1 = nullptr;
  EXPECT_FALSE(ptr);
  EXPECT_FALSE(ptr1);

  mo.malloc(&ptr, 40, MemoryOptimizer::MALLOC_FROM_HOST_MEMORY);
  ptr1 = std::malloc(40);
  EXPECT_TRUE(ptr1);
  EXPECT_TRUE(ptr);

  for (mems_t i = 0; i < 40; i++) {
    ((char *) ptr)[i] = i;
    ((char *) ptr1)[i] = i;
  }

  for (mems_t i = 0; i < 40; i++) {
    EXPECT_EQ(((char *) ptr)[i], ((char *) ptr1)[i]);
  }

  mo.free(ptr, MemoryOptimizer::FREE_FROM_HOST_MEMORY);
  std::free(ptr1);
  ptr = nullptr;
  ptr1 = nullptr;
  EXPECT_FALSE(ptr);
  EXPECT_FALSE(ptr1);
}
}  // namespace memory
}  // namespace chime
