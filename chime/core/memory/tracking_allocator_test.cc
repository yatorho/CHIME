// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

#include "chime/core/memory/tracking_allocator.h"

#include <tuple>

#include "chime/core/memory/allocator.h"
#include "chime/core/memory/mem.h"
#include "chime/core/platform/test.hpp"
#include "chime/core/util/optional.hpp"

namespace chime {
namespace memory {

class TestableSizeTrackingAllocator : public Allocator {
 public:
  std::string Name() override { return "TestableSizeTrackingAllocator"; }
  void *AllocateRaw(size_t, size_t num_bytes) override {
    void *ptr = port::Malloc(num_bytes);
    _size_map[ptr] = num_bytes;
    return ptr;
  }
  void DeallocateRaw(void *ptr) override {
    const auto &iter = _size_map.find(ptr);
    EXPECT_NE(_size_map.end(), iter);
    _size_map.erase(iter);
    port::Free(ptr);
  }
  bool TracksAllocationSizes() const override { return true; }
  size_t RequestedSize(const void *ptr) const override {
    const auto &iter = _size_map.find(ptr);
    EXPECT_NE(_size_map.end(), iter);
    return iter->second;
  }
  util::Optional<AllocatorStats> GetStats() override { return util::nullopt; }

 private:
  std::unordered_map<const void *, size_t> _size_map;
};

class NoMemoryAllocator : public Allocator {
 public:
  std::string Name() override { return "NoMemoryAllocator"; }
  void *AllocateRaw(size_t, size_t num_bytes) override { return nullptr; }
  void DeallocateRaw(void *) override {}
  bool TracksAllocationSizes() const override { return true; }
  util::Optional<AllocatorStats> GetStats() override { return util::nullopt; }
};

TEST(TrackingAllocatorTest, SimpleNoTracking) {
  Allocator *a = CPUAllocator();

  EXPECT_FALSE(a->TracksAllocationSizes());

  TrackingAllocator *ta = new TrackingAllocator(a, false);
  void *p1 = ta->AllocateRaw(4, 4);
  ta->DeallocateRaw(p1);
  void *p2 = ta->AllocateRaw(4, 12);

  std::tuple<size_t, size_t, size_t> sizes = ta->GetSizes();

  EXPECT_EQ(16, std::get<0>(sizes));
  EXPECT_EQ(0, std::get<1>(sizes));
  EXPECT_EQ(0, std::get<2>(sizes));

  ta->DeallocateRaw(p2);
  auto records = ta->GetRecordsAndUnRef();  // deleted ta
  EXPECT_EQ(4, records[0].alloc_bytes);
  EXPECT_EQ(12, records[1].alloc_bytes);

  ta = new TrackingAllocator(a, true);
  p1 = ta->AllocateRaw(4, 4);
  EXPECT_EQ(4, ta->RequestedSize(p1));
  EXPECT_EQ(4, ta->AllocatedSizeSlow(p1));
  EXPECT_EQ(4, ta->AllocatedSize(p1));
  EXPECT_EQ(1, ta->AllocationID(p1));

  ta->DeallocateRaw(p1);
  p2 = ta->AllocateRaw(4, 12);
  EXPECT_EQ(12, ta->RequestedSize(p2));
  EXPECT_EQ(12, ta->AllocatedSizeSlow(p2));
  EXPECT_EQ(12, ta->AllocatedSize(p2));
  EXPECT_EQ(2, ta->AllocationID(p2));

  sizes = ta->GetSizes();
  EXPECT_EQ(16, std::get<0>(sizes));
  EXPECT_EQ(12, std::get<1>(sizes));
  EXPECT_EQ(12, std::get<2>(sizes));

  ta->DeallocateRaw(p2);
  sizes = ta->GetSizes();
  EXPECT_EQ(16, std::get<0>(sizes));
  EXPECT_EQ(12, std::get<1>(sizes));
  EXPECT_EQ(0, std::get<2>(sizes));

  records = ta->GetRecordsAndUnRef();  // deleted ta
  EXPECT_EQ(4, records[0].alloc_bytes);
  EXPECT_EQ(-4, records[1].alloc_bytes);
  EXPECT_EQ(12, records[2].alloc_bytes);
  EXPECT_EQ(-12, records[3].alloc_bytes);
}

TEST(TrackingAllocatorTest, SimpleTracks) {
  TestableSizeTrackingAllocator *a = new TestableSizeTrackingAllocator();
  EXPECT_TRUE(a->TracksAllocationSizes());

  TrackingAllocator *ta = new TrackingAllocator(a, false);
  void *p1 = ta->AllocateRaw(4, 12);
  ta->DeallocateRaw(p1);
  void *p2 = ta->AllocateRaw(4, 4);

  std::tuple<size_t, size_t, size_t> sizes = ta->GetSizes();
  EXPECT_EQ(16, std::get<0>(sizes));
  EXPECT_EQ(12, std::get<1>(sizes));
  EXPECT_EQ(4, std::get<2>(sizes));

  ta->DeallocateRaw(p2);

  auto records = ta->GetRecordsAndUnRef();  // deleted ta
  EXPECT_EQ(12, records[0].alloc_bytes);
  EXPECT_EQ(-12, records[1].alloc_bytes);
  EXPECT_EQ(4, records[2].alloc_bytes);
  EXPECT_EQ(-4, records[3].alloc_bytes);
  delete a;
}

TEST(TrackingAllocatorTest, OutOfMemory) {
  NoMemoryAllocator a;
  EXPECT_TRUE(a.TracksAllocationSizes());

  TrackingAllocator *ta = new TrackingAllocator(&a, false);

  void *p1 = ta->AllocateRaw(4, 12);
  EXPECT_EQ(nullptr, p1);

  std::tuple<size_t, size_t, size_t> sizes = ta->GetSizes();
  EXPECT_EQ(0, std::get<0>(sizes));
  EXPECT_EQ(0, std::get<1>(sizes));
  EXPECT_EQ(0, std::get<2>(sizes));

  EXPECT_EQ(0, ta->GetRecordsAndUnRef().size());
}

TEST(TrackingAllocatorTest, FreeNullPtr) {
  NoMemoryAllocator a;

  EXPECT_TRUE(a.TracksAllocationSizes());

  TrackingAllocator* ta = new TrackingAllocator(&a, false);

  ta->DeallocateRaw(nullptr);

  std::tuple<size_t, size_t, size_t> sizes = ta->GetSizes();

  EXPECT_EQ(0, std::get<0>(sizes));
  EXPECT_EQ(0, std::get<1>(sizes));
  EXPECT_EQ(0, std::get<2>(sizes));

  EXPECT_EQ(0, ta->GetRecordsAndUnRef().size());
}

}  // namespace memory
}  // namespace chime
