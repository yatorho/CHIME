// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

/// Classes to maintain a static registry of allocator factories.
#ifndef CHIME_CORE_MEMORY_ALLOCATOR_REGISTRY_H_
#define CHIME_CORE_MEMORY_ALLOCATOR_REGISTRY_H_

#include <memory>
#include <vector>

#include "chime/core/memory/allocator.h"
#include "chime/core/platform/macros.h"
#include "chime/core/platform/thread_annotations.h"
#include "chime/core/platform/numa.h"

namespace chime {
namespace memory {

class AllocatorFactory {
 public:
  virtual ~AllocatorFactory() {}

  virtual bool numa_enabled() { return false; }

  /// Creates a new allocator.
  virtual Allocator *create_allocator() = 0;

  virtual SubAllocator *create_sub_allocator(int numa_node) = 0;
};

/// ProcessState is defined in a package that cannot be a dependency of
/// framework.  This definition allows us to access the one method we need.
class ProcessStateInterface {
 public:
  virtual ~ProcessStateInterface() {}
  virtual Allocator *get_cpu_allocator(int numa_node) = 0;
};

/// A singleton registry of AllocatorFactories.
///
/// Allocators should be obtained through `ProcessState` or `cpu_allocator()`,
/// not directly through this interface. The purpuse of this registry is to
/// allow link-time discovery of ultiple AllocatorFactories among which
/// ProcessState will obtain the best fit at startup.
class AllocatorFactoryRegistry {
 public:
  AllocatorFactoryRegistry() {}

  ~AllocatorFactoryRegistry() {}

  void registry(const char *source_file, int source_line, const string &name,
                int priority, AllocatorFactory *factory);
  // Returns 'best fit' Allocator.  Find the factory with the highest priority
  // and return an allocator constructed by it. If multiple factories have
  // been registered with the same priority, picks one by unspecified criteria.
  Allocator *get_allocator();

  // Returns 'best fit' SubAllocator. First look for the highest priority
  // factory that is NUMA-enabled. If none is registered, fall back to the
  // highest priority non-NUMA-enabled factory. If NUMA-enabled, return a
  // SubAllocator specific to numa_node, otherwise return a NUMA-insensitive
  // SubAllocator.
  SubAllocator *get_sub_llocator(int numa_node);

  static AllocatorFactoryRegistry *singleton();

  ProcessStateInterface *process_state() { return _process_state; }

 protected:
  friend class ProcessState;
  ProcessStateInterface *_process_state;

 private:
  std::mutex _mutex;
  bool _first_alloc_made = false;
  struct FactoryEntry {
    const char *source_file;
    int source_line;
    string name;
    int priority;
    std::unique_ptr<AllocatorFactory> factory;
    std::unique_ptr<Allocator> allocator;
    std::vector<std::unique_ptr<SubAllocator>> sub_allocators;
  };

  std::vector<FactoryEntry> _factories CHIME_GUARDED_BY(_mutex);

  const FactoryEntry *find_entry(const string &name, int priority) const
      CHIME_EXCLUSIVE_LOCKS_REQUIRED(_mutex);

  CHIME_DISALLOW_COPY_AND_ASSIGN(AllocatorFactoryRegistry);
};

class AllocatorFactoryRegistration {
 public:
  AllocatorFactoryRegistration(const char *source_file, int source_line,
                               const string &name, int priority,
                               AllocatorFactory *factory) {
    AllocatorFactoryRegistry::singleton()->registry(source_file, source_line,
                                                    name, priority, factory);
  }
};



}  // namespace memory
}  // namespace chime

#endif  // CHIME_CORE_MEMORY_ALLOCATOR_REGISTRY_H_
