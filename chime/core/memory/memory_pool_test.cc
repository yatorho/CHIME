// Copyright by 2022.4 chime
// author: yatorho

#include "chime/core/memory/memory_pool.hpp"

namespace chime {
namespace memory {

class ChimeMemoryPoolTest : public ::testing::Test {};

TEST_F(ChimeMemoryPoolTest, TestInitialization) {
  mems_t pool_size = 1000ul;

  ChimeMemoryPool mp(PoolType::CPU_MEMORY_TYPE, pool_size);
  EXPECT_EQ(mp.pool_type(), PoolType::CPU_MEMORY_TYPE);
  EXPECT_EQ(mp.pool_size(), pool_size);

  EXPECT_FALSE(mp.check_cpu_head());
  EXPECT_FALSE(mp.check_cpu_memory_block());
  EXPECT_FALSE(mp.check_cpu_free_mb_list());
}


} // namespace memory
} // namespace chime
