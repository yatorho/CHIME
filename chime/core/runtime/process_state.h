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
    std::string debug_string() const;
  };

  void enable_numa() { _numa_enabled = true; }

  MemDesc ptr_type(const void *ptr);

  Allocator *get_cpu_allocator(int numa_node) override;

  /// Registers alloc visitor for the CPU allocator(s).
  /// REQUIRES: must be called before get_cpu_allocator.
  void add_cpu_alloc_visitor(SubAllocator::Visitor v);
  /// Registers free visitor for the CPU allocator(s).
  /// REQUIRES: must be called before get_cpu_allocator.
  void add_cpu_free_visitor(SubAllocator::Visitor v);

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

  std::string name() override { return _a->name(); }

  void *allocate_row(size_t alignment, size_t num_btyes) override {
    void *p = _a->allocate_row(alignment, num_btyes);
    mutex_lock l(*_mu);
    (*_mm)[p] = _md;
    return p;
  }

  void deallocate_row(void *p) override {
    mutex_lock l(*_mu);
    auto iter = _mm->find(p);
    _mm->erase(iter);
    _a->deallocate_row(p);
  }

  bool tracks_allocation_sizes() const override {
    return _a->tracks_allocation_sizes();
  }

  size_t requested_size(const void *p) const override {
    return _a->requested_size(p);
  }

  size_t allocated_size(const void *p) const override {
    return _a->allocated_size(p);
  }

  util::Optional<AllocatorStats> get_stats() override {
    return _a->get_stats();
  }

  bool clear_stats() override { return _a->clear_stats(); }

  AllocatorMemoryType get_memory_type() const override {
    return _a->get_memory_type();
  }

  ProcessState::MDMap *_mm;  // not owned
  Allocator *_a;             // not owned
  ProcessState::MemDesc _md;
  mutex *_mu;
};

}  // namespace memory
}  // namespace chime

#endif  // CHIME_CORE_RUNTIME_PROCESS_STATE_H_
