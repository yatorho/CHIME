// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

#include "chime/core/memory/allocator_registry.h"

#include <memory>

#include "chime/core/memory/allocator.h"
#include "chime/core/platform/logging.hpp"
#include "chime/core/platform/mutex.h"
#include "chime/core/platform/numa.h"

namespace chime {
namespace memory {

AllocatorFactoryRegistry *AllocatorFactoryRegistry::Singleton() {
  static AllocatorFactoryRegistry *registry = new AllocatorFactoryRegistry;
  return registry;
}

const AllocatorFactoryRegistry::FactoryEntry *
AllocatorFactoryRegistry::find_entry(const string &name, int priority) const {
  for (auto &entry : _factories) {
    if (!name.compare(entry.name) && priority == entry.priority) {
      return &entry;
    }
  }
  return nullptr;
}

void AllocatorFactoryRegistry::Registry(const char *source_file,
                                        int source_line, const string &name,
                                        int priority,
                                        AllocatorFactory *factory) {
  mutex_lock lock(_mutex);
  CHECK(!_first_alloc_made) << "Attempt to register an AllocatorFactory after "
                               "the call to get_allocator().";
  CHECK(!name.empty()) << "Need a valid name for Allcator";
  CHECK_GE(priority, 0) << "Priority needs to be non-negative";

  const FactoryEntry *existing = find_entry(name, priority);
  if (existing != nullptr) {
    LOG(FATAL) << "New registration for AllocatorFactory " << name
               << " with priority " << priority
               << " conflicts with existing registration from "
               << existing->source_file << ":" << existing->source_line;
  }

  FactoryEntry entry;
  entry.source_file = source_file;
  entry.source_line = source_line;
  entry.name = name;
  entry.priority = priority;
  entry.factory.reset(factory);
  _factories.push_back(std::move(entry));
}

Allocator *AllocatorFactoryRegistry::GetAllocator() {
  mutex_lock lock(_mutex);
  _first_alloc_made = true;
  FactoryEntry *best_entry = nullptr;
  for (auto &entry : _factories) {
    if (best_entry == nullptr) {
      best_entry = &entry;
    } else if (entry.priority > best_entry->priority) {
      best_entry = &entry;
    }
  }
  if (best_entry) {
    if (!best_entry->allocator) {
      best_entry->allocator.reset(best_entry->factory->CreateAllocator());
    }
    return best_entry->allocator.get();
  } else {
    LOG(FATAL) << "No AllocatorFactory registered";
    return nullptr;
  }
}

SubAllocator *AllocatorFactoryRegistry::GetSubAllocator(int numa_node) {
  mutex_lock lock(_mutex);
  _first_alloc_made = true;

  FactoryEntry *best_entry = nullptr;
  for (auto &entry : _factories) {
    if (best_entry == nullptr) {
      best_entry = &entry;
    } else if (best_entry->factory->NUMAEnabled()) {
      if (entry.factory->NUMAEnabled() &&
          (entry.priority > best_entry->priority)) {
        best_entry = &entry;
      }
    } else {
      DCHECK(!best_entry->factory->NUMAEnabled());
      if (entry.factory->NUMAEnabled() ||
          entry.priority > best_entry->priority) {
        best_entry = &entry;
      }
    }
  }

  if (best_entry) {
    int index = 0;
    if (numa_node != port::NUMA_NO_AFFINITY) {
      CHECK_LE(numa_node, port::NUMANumNodes());
      index = 1 + numa_node;
    }
    if (best_entry->sub_allocators.size() < static_cast<size_t>(index + 1)) {
      best_entry->sub_allocators.resize(index + 1);
    }
    if (!best_entry->sub_allocators[index].get()) {
      best_entry->sub_allocators[index].reset(
          best_entry->factory->CreateSubAllocator(numa_node));
    }
    return best_entry->sub_allocators[index].get();
  } else {
    LOG(FATAL) << "No AllocatorFactory registered";
    return nullptr;
  }
}
}  // namespace memory
}  // namespace chime
