// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

#ifndef CHIME_CORE_RUNTIME_PROCESS_STATE_H_
#define CHIME_CORE_RUNTIME_PROCESS_STATE_H_

#include <unordered_map>

#include "chime/core/framework/device_types.h"
#include "chime/core/memory/allocator.h"
#include "chime/core/memory/allocator_registry.h"
#include "chime/core/platform/mutex.h"
#include "chime/core/platform/thread_annotations.h"
#include "chime/core/util/optional.hpp"

namespace chime {

namespace memory {
class ProcessState : public ProcessStateInterface {
 public:
  static ProcessState *singleton();

  /// Descriptor for a memory allocation attributes, used by optional runtime
  /// correctness analysis logic.
  struct MemDesc {
    DeviceType loc;
    int dev_index;
    bool gpu_registered;
    bool nic_registered;
    MemDesc()
        : loc(DeviceType::CENTRAL_PROCESSING_UNIT),
          dev_index(0),
          gpu_registered(false),
          nic_registered(false) {}
    std::string DebugString() const;
  };

  void EnableNUMA() { _numa_enabled = true; }

  MemDesc PtrType(const void *ptr);

  Allocator *GetCPUAllocator(int numa_node) override;

  /// Registers alloc visitor for the CPU allocator(s).
  /// REQUIRES: must be called before get_cpu_allocator.
  void AddCPUAllocVisitor(SubAllocator::Visitor v);
  /// Registers free visitor for the CPU allocator(s).
  /// REQUIRES: must be called before get_cpu_allocator.
  void AddCPUFreeVisitor(SubAllocator::Visitor v);

  typedef std::unordered_map<const void *, MemDesc> MDMap;

 protected:
  ProcessState();
  virtual ~ProcessState() {}

  bool _numa_enabled;

  mutex _mutex;

  std::vector<Allocator *> _cpu_allocators CHIME_GUARDED_BY(_mutex);
  std::vector<SubAllocator::Visitor> _cpu_alloc_visitors
      CHIME_GUARDED_BY(_mutex);
  std::vector<SubAllocator::Visitor> _cpu_free_visitors
      CHIME_GUARDED_BY(_mutex);

  MDMap _mem_desc;
};

class RecordingAllocator : public Allocator {
 public:
  RecordingAllocator(ProcessState::MDMap *mm, Allocator *a,
                     ProcessState::MemDesc md, mutex *mu)
      : _mm(mm), _a(a), _md(md), _mu(mu) {}

  std::string Name() override { return _a->Name(); }

  void *AllocateRaw(size_t alignment, size_t num_btyes) override {
    void *p = _a->AllocateRaw(alignment, num_btyes);
    mutex_lock l(*_mu);
    (*_mm)[p] = _md;
    return p;
  }

  void DeallocateRaw(void *p) override {
    mutex_lock l(*_mu);
    auto iter = _mm->find(p);
    _mm->erase(iter);
    _a->DeallocateRaw(p);
  }

  bool TracksAllocationSizes() const override {
    return _a->TracksAllocationSizes();
  }

  size_t RequestedSize(const void *p) const override {
    return _a->RequestedSize(p);
  }

  size_t AllocatedSize(const void *p) const override {
    return _a->AllocatedSize(p);
  }

  util::Optional<AllocatorStats> GetStats() override {
    return _a->GetStats();
  }

  bool ClearStats() override { return _a->ClearStats(); }

  AllocatorMemoryType GetMemoryType() const override {
    return _a->GetMemoryType();
  }

  ProcessState::MDMap *_mm;  // not owned
  Allocator *_a;             // not owned
  ProcessState::MemDesc _md;
  mutex *_mu;
};

}  // namespace memory
}  // namespace chime

#endif  // CHIME_CORE_RUNTIME_PROCESS_STATE_H_
