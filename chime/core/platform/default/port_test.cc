// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

#include <cstdint>

#include "chime/core/memory/mem.h"
#include "chime/core/platform/cpu_info.h"
#include "chime/core/platform/numa.h"
#include "chime/core/platform/test.hpp"

namespace chime {
namespace port {

TEST(Port, AlignedMalloc) {
  for (size_t alignment = 1; alignment <= 1 << 20; alignment <<= 1) {
    void *p = AlignedMalloc(1, alignment);
    ASSERT_TRUE(p != nullptr) << "alignment=" << alignment;
    uintptr_t pval = reinterpret_cast<uintptr_t>(p);
    EXPECT_EQ(pval % alignment, 0);
    AlignedFree(p);
  }
}

TEST(Port, GetCurrentCPU) {
  const int cpu = GetCurrentCPU();
  EXPECT_GE(cpu, 0);
  EXPECT_LT(cpu, NumTotalCPUs());
}

TEST(Port, NUMAMalloc) {
  const bool numa_enabled = NUMAEnabled();
  if (numa_enabled) {
    for (int i = 0; i < NUMANumNodes(); ++i) {
      for (size_t alignment = 1; alignment <= 1 << 20; alignment <<= 1) {
        void *p = NUMAMalloc(i, alignment, alignment);
        ASSERT_TRUE(p != nullptr) << "alignment=" << alignment;
        uintptr_t pval = reinterpret_cast<uintptr_t>(p);
        EXPECT_EQ(pval % alignment, 0);
        NUMAFree(p, i);
      }
    }
  } else {
    EXPECT_EQ(NUMANumNodes(), 1);
    for (size_t alignment = 1; alignment <= 1 << 20; alignment <<= 1) {
      void *p = NUMAMalloc(0, 1, alignment);
      ASSERT_TRUE(p != nullptr) << "alignment=" << alignment;
      NUMAFree(p, 1);
    }
  }
}

TEST(Port, NUMAGetThreadNodeAffinity) {
  int node_index = NUMAGetThreadNodeAffinity();
  EXPECT_GE(node_index, 0);
  EXPECT_LT(node_index, NUMANumNodes());
}

}  // namespace port
}  // namespace chime
