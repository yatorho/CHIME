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
  {
    mems_t pool_size = 3000ul;
    mems_t malloc_size = 3000ul;

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

    void *ptr;
    mp.malloc(&ptr, malloc_size, ChimeMemoryPool::MALLOC_FROM_CPU_MEMORY);
    EXPECT_TRUE(ptr);
    EXPECT_EQ(mp.pool_status(), ChimeMemoryPool::WORKING);

    mp.destroy();
    EXPECT_EQ(mp.pool_type(), ChimeMemoryPool::UNINITIALIZED);

    EXPECT_FALSE(mp.check_cpu_head());
    EXPECT_FALSE(mp.check_cpu_memory_block());
  }
}

TEST_F(ChimeMemoryPoolTest, TestFree) {
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

    mp.free(ptr2, ChimeMemoryPool::FREE_FROM_CPU_MEMORY);
    mp.free(ptr1, ChimeMemoryPool::FREE_FROM_CPU_MEMORY);
    EXPECT_EQ(mp.pool_status(), ChimeMemoryPool::READY_TO_BE_FREED);
    mp.destroy();
  }
  {
    mems_t pool_size = 3000ul;
    mems_t malloc_size = 3000ul;

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

    void *ptr;
    mp.malloc(&ptr, malloc_size, ChimeMemoryPool::MALLOC_FROM_CPU_MEMORY);
    EXPECT_TRUE(ptr);
    EXPECT_EQ(mp.pool_status(), ChimeMemoryPool::WORKING);

    mp.free(ptr);
    EXPECT_EQ(mp.pool_status(), ChimeMemoryPool::READY_TO_BE_FREED);

    mp.destroy();
    EXPECT_EQ(mp.pool_type(), ChimeMemoryPool::UNINITIALIZED);

    EXPECT_FALSE(mp.check_cpu_head());
    EXPECT_FALSE(mp.check_cpu_memory_block());
  }

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
    ASSERT_EQ(ptr1, const_cast<void *>(mp.cpu_head()));
    EXPECT_EQ(mp.pool_status(), ChimeMemoryPool::WORKING);
    mp.malloc(&ptr2, malloc_size, ChimeMemoryPool::MALLOC_FROM_CPU_MEMORY);
    EXPECT_TRUE(ptr2);
    EXPECT_EQ(mp.pool_status(), ChimeMemoryPool::WORKING);

    ASSERT_EQ(mp.cpu_memory_block()->block_status, MemoryBlock::OCCUPIED);
    ASSERT_EQ(mp.cpu_memory_block()->size, malloc_size);
    ASSERT_TRUE(mp.cpu_memory_block()->rear);
    ASSERT_EQ(mp.cpu_memory_block()->rear->block_status, MemoryBlock::OCCUPIED);
    ASSERT_EQ(mp.cpu_memory_block()->rear->size, malloc_size);
    ASSERT_TRUE(mp.cpu_memory_block()->rear->rear);
    ASSERT_EQ(mp.cpu_memory_block()->rear->rear->block_status,
              MemoryBlock::FREE);
    ASSERT_EQ(mp.cpu_memory_block()->rear->rear->size,
              pool_size - 2 * malloc_size);
    ASSERT_FALSE(mp.cpu_memory_block()->rear->rear->rear);

    ASSERT_TRUE(mp.cpu_next_malloc());
    ASSERT_EQ(mp.cpu_next_malloc(),
              const_cast<const mb_ptr>(mp.cpu_memory_block()->rear->rear));

    ASSERT_TRUE(mp.cpu_next_free());
    ASSERT_EQ(mp.cpu_next_free(),
              const_cast<const mb_ptr>(mp.cpu_memory_block()->rear));
    ASSERT_FALSE(mp.cpu_memory_block()->front);

    mp.free(ptr1, ChimeMemoryPool::FREE_FROM_CPU_MEMORY);
    ASSERT_EQ(mp.pool_status(), ChimeMemoryPool::WORKING);
    ASSERT_EQ(mp.cpu_memory_block()->block_status, MemoryBlock::FREE);
    ASSERT_EQ(mp.cpu_memory_block()->rear->block_status, MemoryBlock::OCCUPIED);
    ASSERT_EQ(mp.cpu_memory_block()->rear->rear->block_status,
              MemoryBlock::FREE);
    ASSERT_FALSE(mp.cpu_memory_block()->rear->rear->rear);

    mp.free(ptr2, ChimeMemoryPool::FREE_FROM_CPU_MEMORY);
    ASSERT_EQ(mp.cpu_memory_block()->block_status, MemoryBlock::FREE);
    ASSERT_EQ(mp.cpu_memory_block()->size, pool_size);
    ASSERT_FALSE(mp.cpu_memory_block()->rear);
    ASSERT_EQ(mp.pool_status(), ChimeMemoryPool::READY_TO_BE_FREED);
    ASSERT_FALSE(mp.cpu_next_free());
    ASSERT_EQ(mp.cpu_next_malloc(), mp.cpu_memory_block());

    mp.destroy();
    ASSERT_FALSE(mp.cpu_memory_block());
    ASSERT_FALSE(mp.cpu_head());
    ASSERT_FALSE(mp.cpu_next_free());
    ASSERT_FALSE(mp.cpu_next_malloc());
  }
}

}  // namespace memory
}  // namespace chime
