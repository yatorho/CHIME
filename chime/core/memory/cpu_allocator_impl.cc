// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

#include <unordered_map>

#include "chime/core/memory/allocator.h"
#include "chime/core/memory/allocator_registry.h"
#include "chime/core/memory/mem.h"
#include "chime/core/platform/macros.h"
#include "chime/core/platform/mutex.h"
#include "chime/core/platform/thread_annotations.h"
#include "chime/core/util/optional.hpp"

namespace chime {
namespace memory {

class CPUAllocator;
class SubCPUAllocator;

static bool cpu_allocator_collect_stats = false;

void EnableCPUAllocatorStats() { cpu_allocator_collect_stats = true; }

void DisableCPUAllocatorStats() { cpu_allocator_collect_stats = false; }

bool CPUAllocatorStatsEnabled() { return cpu_allocator_collect_stats; }

static const int MAX_TOTAL_ALLOCATION_WARNING = 1;

static const int MAX_SINGLE_ALLOCATION_WARNINGS = 5;

/// If cpu_allocator_collect_stats is true, warn when the total allocated
/// memory exceeds this threshold.
static const double TOTAL_ALLOCATION_WARNING_THRESHOLD = 0.5;

/// Individual allocations large than this amount will trigger a warning.
static const double LARGE_ALLOCATION_WARNING_THRESHOLD = 0.1;

static int64_t LargeAllocationWarningBytes() {
  static int64_t value = static_cast<int64_t>(
      port::AvailableRam() * LARGE_ALLOCATION_WARNING_THRESHOLD);
  return value;
}

static int64_t TotalAllocationWarningBytes() {
  static int64_t value = static_cast<int64_t>(
      port::AvailableRam() * TOTAL_ALLOCATION_WARNING_THRESHOLD);
  return value;
}

namespace {
class CPUAllocator : public Allocator {
 public:
  CPUAllocator()
      : _single_allocation_warning_count(0),
        _total_allocation_warning_count(0) {}

  ~CPUAllocator() override {}

  string Name() override { return "CPU Allocator"; }

  void *AllocateRaw(size_t alignment, size_t num_bytes) override {
    if (num_bytes > static_cast<size_t>(LargeAllocationWarningBytes()) &&
        _single_allocation_warning_count < MAX_SINGLE_ALLOCATION_WARNINGS) {
      ++_single_allocation_warning_count;
      LOG(WARNING) << "Allocation of " << num_bytes << " exceeds "
                   << 100 * LARGE_ALLOCATION_WARNING_THRESHOLD
                   << "% of free system memory.";
    }

    void *ptr = port::AlignedMalloc(num_bytes, alignment);
    if (cpu_allocator_collect_stats && !CPUAllocatorFullStatsEnabled()) {
      mutex_lock lock(_mu);
      const size_t alloc_size =
          port::GetAllocatedSize(ptr, num_bytes, alignment);
      ++_stats.num_allocs;
      _stats.bytes_in_use += alloc_size;
      _stats.peak_bytes_in_use =
          std::max<int64_t>(_stats.peak_bytes_in_use, _stats.bytes_in_use);
      _stats.largest_alloc_size =
          std::max<int64_t>(_stats.largest_alloc_size, alloc_size);

      if (_stats.bytes_in_use > TotalAllocationWarningBytes() &&
          _total_allocation_warning_count < MAX_TOTAL_ALLOCATION_WARNING) {
        ++_total_allocation_warning_count;
        LOG(WARNING) << "Total allocated memory " << _stats.bytes_in_use
                     << " exceeds " << 100 * TOTAL_ALLOCATION_WARNING_THRESHOLD
                     << "% of free system memory.";
      }
      if (ptr != nullptr) {
        _chunks.emplace(std::make_pair(ptr, alloc_size));
        AddTraceMe("MemoryAllocation", ptr, num_bytes, alloc_size);
      }
    }
    return ptr;
  }

  void DeallocateRaw(void *ptr) override {
    if (cpu_allocator_collect_stats && !CPUAllocatorFullStatsEnabled()) {
      mutex_lock lock(_mu);
      auto itr = _chunks.find(ptr);
      if (itr != _chunks.end()) {
        const size_t alloc_size = itr->second;
        _stats.bytes_in_use -= alloc_size;
        _chunks.erase(itr);
        AddTraceMe("MemoryDeallocation", ptr, 0, alloc_size);
      }
    }
    port::AlignedFree(ptr);
  }

  util::Optional<AllocatorStats> GetStats() override {
    if (!cpu_allocator_collect_stats) return util::nullopt;
    mutex_lock lock(_mu);
    return _stats;
  }

  bool ClearStats() override {
    if (cpu_allocator_collect_stats && !CPUAllocatorFullStatsEnabled()) {
      mutex_lock lock(_mu);
      _stats.num_allocs = 0;
      _stats.bytes_in_use = 0;
      _stats.peak_bytes_in_use = 0;
      _stats.largest_alloc_size = 0;
      _chunks.clear();
      return true;
    }
    return false;
  }

  size_t AllocatedSizeSlow(const void *ptr) const override {
    if (cpu_allocator_collect_stats && !CPUAllocatorFullStatsEnabled()) {
      mutex_lock lock(_mu);
      auto itr = _chunks.find(ptr);
      if (itr != _chunks.end()) {
        return itr->second;
      }
    }
    return port::GetAllocatedSize(ptr);
  }

  AllocatorMemoryType GetMemoryType() const override {
    return AllocatorMemoryType::HOST_PAGEABLE;
  }

  void AddTraceMe(std::string traceme_name, const void *ptr, size_t req_size,
                  size_t alloc_size) {
    // TODO(yatorho): Add a traceme for the allocation.
  }

 private:
  mutable mutex _mu;
  AllocatorStats _stats CHIME_GUARDED_BY(_mu);

  std::atomic<int> _single_allocation_warning_count;
  int _total_allocation_warning_count CHIME_GUARDED_BY(_mu);

  std::unordered_map<const void *, size_t> _chunks CHIME_GUARDED_BY(_mu);

  CHIME_DISALLOW_COPY_AND_ASSIGN(CPUAllocator);
};

class CPUAllocatorFactory : public AllocatorFactory {
 public:
  Allocator *CreateAllocator() override { return new CPUAllocator; }

  SubAllocator *CreateSubAllocator(int numa_node) override {
    return new CPUSubAllocator(new CPUAllocator);
  }

 private:
  class CPUSubAllocator : public SubAllocator {
   public:
    explicit CPUSubAllocator(CPUAllocator *cpu_allocator)
        : SubAllocator({}, {}), _cpu_allocator(cpu_allocator) {}

    void *Alloc(size_t alignment, size_t num_bytes,
                size_t *bytes_reserved) override {
      *bytes_reserved = num_bytes;
      return _cpu_allocator->AllocateRaw(alignment, num_bytes);
    }

    void Free(void *ptr, size_t num_bytes) override {
      _cpu_allocator->DeallocateRaw(ptr);
    }

    AllocatorMemoryType GetMemoryType() const override {
      return _cpu_allocator->GetMemoryType();
    }

   private:
    CPUAllocator *_cpu_allocator;
  };
};

REGISTER_MEM_ALLOCATOR("DefaultCPUAllocator", 100, CPUAllocatorFactory);

}  // namespace

}  // namespace memory
}  // namespace chime
