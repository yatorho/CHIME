// Copyright by 2022.4 chime
// author: yatorho

#include "chime/core/framework/syncedmem.hpp"

namespace chime {

class SyncedMemoryTest : public ::testing::Test {};

TEST_F(SyncedMemoryTest, TestInitialization) {
  SyncedMemory mem1;
  EXPECT_EQ(mem1.size(), 0ul);
  EXPECT_EQ(mem1.head(), SyncedMemory::UNINITIALIZED);
  EXPECT_EQ(mem1.own_cpu_data(), false);
  EXPECT_EQ(mem1.own_gpu_data(), false);

  SyncedMemory mem2(10ul);
  EXPECT_EQ(mem2.size(), 10ul);
  EXPECT_EQ(mem2.head(), SyncedMemory::UNINITIALIZED);
  EXPECT_EQ(mem2.own_cpu_data(), false);
  EXPECT_EQ(mem2.own_gpu_data(), false);
}

TEST_F(SyncedMemoryTest, TestAllocation) {
  SyncedMemory mem(10ul);
  EXPECT_TRUE(mem.cpu_mem());
  EXPECT_TRUE(mem.mutable_cpu_mem());
  EXPECT_TRUE(mem.own_cpu_data());
  EXPECT_FALSE(mem.own_gpu_data());
  
  const void *cpu_mem = mem.cpu_mem();
  for (uint32_t i = 0; i < mem.size(); i++) {
    EXPECT_EQ(static_cast<const char *>(cpu_mem)[i], 0);
  }
}

TEST_F(SyncedMemoryTest, TestCPUWritte) {
  SyncedMemory mem(10ul);
  void *cpu_data = mem.mutable_cpu_mem();
  EXPECT_EQ(mem.head(), SyncedMemory::HEAD_AT_CPU);
  std::memset(cpu_data, 1, mem.size());
  for (uint32_t i = 0; i < mem.size(); i++) {
    EXPECT_EQ(static_cast<char *>(cpu_data)[i], 1);
  }

  cpu_data = mem.mutable_cpu_mem();
  EXPECT_EQ(mem.head(), SyncedMemory::HEAD_AT_CPU);
  std::memset(cpu_data, 2, mem.size());
  for (uint32_t i = 0; i < mem.size(); i++) {
    EXPECT_EQ(static_cast<char *>(cpu_data)[i], 2);
  }
}

} // namespace chime