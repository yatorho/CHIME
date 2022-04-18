// Copyright by 2022.4 chime
// author: yatorho

#include "chime/core/memory/memory_pool.hpp"

namespace chime {
namespace memory {

class ChimeMemoryPoolTest : public ::testing::Test {};

TEST_F(ChimeMemoryPoolTest, TestConstruct) {
  {
    mems_t pool_size = 1000ul;

    ChimeMemoryPool mp(PoolType::CPU_MEMORY_TYPE, pool_size);
    EXPECT_EQ(mp.pool_type(), PoolType::CPU_MEMORY_TYPE);
    EXPECT_EQ(mp.pool_size(), pool_size);
    EXPECT_EQ(mp.pool_type(), ChimeMemoryPool::UNINITIALIZED);

    EXPECT_FALSE(mp.check_cpu_head());
    EXPECT_FALSE(mp.check_cpu_memory_block());
  }
}

TEST_F(ChimeMemoryPoolTest, TestInitialization) {
  {
    mems_t pool_size = 2000ul;

    ChimeMemoryPool mp(PoolType::CPU_MEMORY_TYPE, pool_size);
    EXPECT_EQ(mp.pool_type(), PoolType::CPU_MEMORY_TYPE);
    EXPECT_EQ(mp.pool_size(), pool_size);
    EXPECT_EQ(mp.pool_type(), ChimeMemoryPool::UNINITIALIZED);

    EXPECT_FALSE(mp.check_cpu_head());
    EXPECT_FALSE(mp.check_cpu_memory_block());

    mp.init();
    EXPECT_EQ(mp.pool_status(), ChimeMemoryPool::READY_TO_BE_FREED);
    EXPECT_TRUE(mp.check_cpu_memory_block());
    EXPECT_TRUE(mp.check_cpu_head());
  }
}

TEST_F(ChimeMemoryPoolTest, TestAllocation) {
  {
    mems_t pool_size = 3000ul;
    mems_t malloc_size = 400ul;

    ChimeMemoryPool mp(PoolType::CPU_MEMORY_TYPE, pool_size);
    EXPECT_EQ(mp.pool_type(), PoolType::CPU_MEMORY_TYPE);
    EXPECT_EQ(mp.pool_size(), pool_size);
    EXPECT_EQ(mp.pool_type(), ChimeMemoryPool::UNINITIALIZED);

    EXPECT_FALSE(mp.check_cpu_head());
    EXPECT_FALSE(mp.check_cpu_memory_block());

    mp.init();
    EXPECT_EQ(mp.pool_status(), ChimeMemoryPool::READY_TO_BE_FREED);
    EXPECT_TRUE(mp.check_cpu_memory_block());
    EXPECT_TRUE(mp.check_cpu_head());

    void *ptr1, *ptr2;
    mp.malloc(&ptr1, malloc_size, ChimeMemoryPool::MALLOC_FROM_CPU_MEMORY);
    EXPECT_TRUE(ptr1);
    EXPECT_EQ(mp.pool_status(), ChimeMemoryPool::WORKING);
    mp.malloc(&ptr2, malloc_size, ChimeMemoryPool::MALLOC_FROM_CPU_MEMORY);
    EXPECT_TRUE(ptr2);
    EXPECT_EQ(mp.pool_status(), ChimeMemoryPool::WORKING);

    mp.destroy();
    EXPECT_EQ(mp.pool_type(), ChimeMemoryPool::UNINITIALIZED);

    EXPECT_FALSE(mp.check_cpu_head());
    EXPECT_FALSE(mp.check_cpu_memory_block());
  }
}

} // namespace memory
} // namespace chime
