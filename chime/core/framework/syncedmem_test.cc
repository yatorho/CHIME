// Copyright by 2022.4 chime
// author: yatorho

#include "chime/core/framework/syncedmem.hpp"

#include "chime/core/memory/mem_optimizer.h"
#include "chime/core/memory/pool.hpp"

namespace chime {

class SyncedMemoryTest : public ::testing::Test {};

TEST_F(SyncedMemoryTest, TestInitialization) {
  memory::ChimeMemoryPool mp(memory::ChimeMemoryPool::CPU_MEMORY_TYPE, 20ul);
  mp.init();

  SyncedMemory mem1(mp, 10);
  EXPECT_EQ(mem1.size(), 10ul);
  EXPECT_EQ(mem1.head(), SyncedMemory::UNINITIALIZED);
  EXPECT_EQ(mem1.own_host_mem(), false);
  EXPECT_EQ(mem1.own_device_mem(), false);

  SyncedMemory mem2(mp, 10ul);
  EXPECT_EQ(mem2.size(), 10ul);
  EXPECT_EQ(mem2.head(), SyncedMemory::UNINITIALIZED);
  EXPECT_EQ(mem2.own_host_mem(), false);
  EXPECT_EQ(mem2.own_device_mem(), false);
}

TEST_F(SyncedMemoryTest, TestAllocation) {
  memory::ChimeMemoryPool mp(memory::ChimeMemoryPool::CPU_MEMORY_TYPE, 20ul);
  mp.init();

  SyncedMemory mem(mp, 20ul);
  EXPECT_TRUE(mem.host_mem());
  EXPECT_TRUE(mem.mutable_host_mem());
  EXPECT_TRUE(mem.own_host_mem());
  EXPECT_FALSE(mem.own_device_mem());

  const void *cpu_mem = mem.host_mem();
  for (uint32_t i = 0; i < mem.size(); i++) {
    EXPECT_EQ(static_cast<const char *>(cpu_mem)[i], 0);
  }
}

TEST_F(SyncedMemoryTest, TestCPUWritte) {
  memory::ChimeMemoryPool mp(memory::ChimeMemoryPool::CPU_MEMORY_TYPE, 20ul);
  mp.init();

  SyncedMemory mem(mp, 10ul);
  void *cpu_data = mem.mutable_host_mem();
  EXPECT_EQ(mem.head(), SyncedMemory::HEAD_AT_HOST);
  std::memset(cpu_data, 1, mem.size());
  for (uint32_t i = 0; i < mem.size(); i++) {
    EXPECT_EQ(static_cast<char *>(cpu_data)[i], 1);
  }

  cpu_data = mem.mutable_host_mem();
  EXPECT_EQ(mem.head(), SyncedMemory::HEAD_AT_HOST);
  std::memset(cpu_data, 2, mem.size());
  for (uint32_t i = 0; i < mem.size(); i++) {
    EXPECT_EQ(static_cast<char *>(cpu_data)[i], 2);
  }
}

TEST_F(SyncedMemoryTest, TestCpuMemoryCopy) {
  SyncedMemory mem1(memory::default_allocator, 20);

  SyncedMemory mem2(memory::default_allocator, 20);

  auto cpu_data1 = mem1.mutable_host_mem();
  std::memset(cpu_data1, 1, mem1.size());

  for (uint32_t i = 0; i < mem1.size(); i++) {
    EXPECT_EQ(static_cast<char *>(cpu_data1)[i], 1);
  }

  auto ptr = mem2.mutable_host_mem();
  std::memset(ptr, 2, mem2.size());
  for (uint32_t i = 0; i < mem2.size(); i++) {
    EXPECT_EQ(static_cast<const char *>(ptr)[i], 2);
  }

  mem2.host_mem_cpy(mem1);
  auto cpu_data2 = mem2.host_mem();
  for (uint32_t i = 0; i < mem2.size(); i++) {
    EXPECT_EQ(static_cast<const char *>(cpu_data2)[i], 1);
  }
}
} // namespace chime