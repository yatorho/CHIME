// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

#include "chime/core/memory/allocator.h"

#include <algorithm>
#include <vector>

#include "chime/core/memory/mem.h"
#include "chime/core/memory/typed_allocator.h"
#include "chime/core/platform/test.hpp"
#include "chime/core/util/optional.hpp"

namespace chime {
namespace memory {

static void CheckStats(Allocator *a, int64_t num_allocs, int64_t bytes_in_use,
                       int64_t peak_bytes_in_use, int64_t largest_alloc_size) {
  util::Optional<AllocatorStats> stats = a->GetStats();
  EXPECT_TRUE(stats);
  if (!stats) return;
  EXPECT_EQ(stats->num_allocs, num_allocs);
  EXPECT_EQ(stats->bytes_in_use, bytes_in_use);
  EXPECT_EQ(stats->peak_bytes_in_use, peak_bytes_in_use);
  EXPECT_EQ(stats->largest_alloc_size, largest_alloc_size);
}

TEST(CPUAllocatorTest, Simple) {
  EnableCPUAllocatorStats();
  Allocator *allocator = CPUAllocator();
  EXPECT_EQ(allocator->Name(), "CPU Allocator");
  EXPECT_TRUE(allocator != nullptr);

  std::vector<void *> ptrs;
  for (int s = 1; s < 1024; ++s) {
    void *raw = allocator->AllocateRaw(1, s);
    ptrs.push_back(raw);
  }
  std::sort(ptrs.begin(), ptrs.end());
  CheckStats(allocator, 1023, 527360, 527360, 1024);

  for (size_t i = 0; i < ptrs.size(); ++i) {
    if (i > 0) {
      ASSERT_NE(ptrs[i], ptrs[i - 1]);  // No dups
    }
    allocator->DeallocateRaw(ptrs[i]);
  }
  CheckStats(allocator, 1023, 0, 527360, 1024);

  float *t1 = TypedAllocator::Allocate<float>(allocator, 1024, {});
  double *t2 = TypedAllocator::Allocate<double>(allocator, 134217728ull, {});

  CheckStats(allocator, 1025,
             1024 * sizeof(float) + 134217728ull * sizeof(double),
             1024 * sizeof(float) + 134217728ull * sizeof(double),
             134217728ull * sizeof(double));
  /// Check alignment
  unsigned long align = reinterpret_cast<unsigned long>(t1);
  EXPECT_EQ(align % Allocator::ALLOCATOR_ALIGNMENT, 0);
  align = reinterpret_cast<unsigned long>(t2);
  EXPECT_EQ(align % Allocator::ALLOCATOR_ALIGNMENT, 0);

  TypedAllocator::Deallocate(allocator, t1, 1024);
  TypedAllocator::Deallocate(allocator, t2, 134217728ull);

  CheckStats(allocator, 1025, 0,
             1024 * sizeof(float) + 134217728ull * sizeof(double),
             134217728ull * sizeof(double));

  EXPECT_TRUE(allocator->ClearStats());
  CheckStats(allocator, 0, 0, 0, 0);
  DisableCPUAllocatorStats();
}

struct TestStruct {
  int x;
};

TEST(CPUAllocatorTest, CheckStructSize) { ASSERT_GT(sizeof(TestStruct), 1); }

TEST(CPUAllocatorTest, AllocateOverflowMaxSizeT) {
  Allocator *a = CPUAllocator();
  EXPECT_FALSE(a->GetStats());

  size_t count_to_allocate = std::numeric_limits<size_t>::max();

  TestStruct *ptr =
      TypedAllocator::Allocate<TestStruct>(a, count_to_allocate, {});

  ASSERT_EQ(ptr, nullptr);
}



}  // namespace memory
}  // namespace chime
