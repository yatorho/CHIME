// Copyright by 2022.6 chime
// author: yatorho

#ifndef CHIME_CORE_MEMORY_ALLOCATOR_H_
#define CHIME_CORE_MEMORY_ALLOCATOR_H_

#include <stdlib.h>

#include <cstdint>
#include <functional>

#include "chime/core/framework/types.hpp"
#include "chime/core/platform/logging.hpp"
#include "chime/core/platform/macros.h"
#include "chime/core/util/optional.hpp"

namespace chime {
namespace memory {

/// Attributes for a single memory allocation call. Different calls to the same
/// allocator could potentially have different attributes.
struct AllocationAttributes {
  AllocationAttributes() = default;

  AllocationAttributes(bool retry_on_failure, bool allocation_will_be_logged,
                       std::function<uint64()> *freed_by_func)
      : retry_on_failure(retry_on_failure),
        allocation_will_be_logged(allocation_will_be_logged),
        freed_by_func(freed_by_func) {}

  /// If the first attempt to allocate the memory fails, the allocation should
  /// wait and retry (with a timeout).
  ///
  /// This is usually set to true, but we may set it to false in cases where a
  /// failure has only performance impact (e.g. optional scratch space
  /// allocation).
  bool retry_on_failure = false;

  /// If true, the allocation will be logged.
  bool allocation_will_be_logged = false;

  std::function<uint64()> *freed_by_func = nullptr;
  CHIME_DISALLOW_COPY_AND_ASSIGN(AllocationAttributes);
};

struct AllocatorStats {
  int64_t num_allocs;
  int64_t num_bytes_in_use;
  int64_t peak_bytes_in_use;
  int64_t largest_alloc_size;

  /// The upper limit of bytes of user allocatable device memory, if such a
  /// limit is known.
  util::Optional<int64_t> bytes_limit;

  int64_t bytes_reserved;
  int64_t peak_bytes_reserved;

  util::Optional<int64_t> bytes_reserved_limit;

  int64_t largest_free_block_bytes;  // largest free block's size in heap.

  AllocatorStats()
      : num_allocs(0),
        num_bytes_in_use(0),
        peak_bytes_in_use(0),
        largest_alloc_size(0),
        bytes_reserved(0),
        peak_bytes_reserved(0),
        largest_free_block_bytes(0) {}

  std::string debug_string() const;
};

enum class AllocatorMemoryType {
  UNKNOWN = 0,
  DEVICE = 1,
  HOST_PAGEABLE = 2,
  HOST_PINNED = 3,
};

class Allocator {
 public:
  /// Align to 64 bytes boundary.
  static constexpr size_t ALLOCATOR_ALIGNMENT = 64;

  virtual ~Allocator();

  /// Return an uninitialized block of memory that is "num_bytes" bytes
  /// in size.  The returned pointer is guaranteed to be aligned to a
  /// multiple of "alignment" bytes.
  /// REQUIRES: "alignment" is a power of 2.
  virtual void *allocate_row(size_t alignment, size_t num_btyes) = 0;

  /// Return an uninitialized block of memory that is "num_bytes" bytes
  /// in size with specified allocation attributes.  The returned pointer is
  /// guaranteed to be aligned to a multiple of "alignment" bytes.
  /// REQUIRES: "alignment" is a power of 2.
  virtual void *allocate_row(size_t alignment, size_t num_btyes,
                             const AllocationAttributes &allocation_attr) {
    /// The default behavior is to use the implementation without any allocation
    /// attributes.
    return allocate_row(alignment, num_btyes);
  }

  /// Deallocate the memory block previously returned by allocate_row.
  /// REQUIRES: "ptr" was returned by a call to allocate_row.
  virtual void deallocate_row(void *ptr) = 0;

  /// Returns true if this allocator tracks the size of allocations.
  ///
  virtual bool tracks_allocation_sizes() const { return false; }

  /// Returns the user-requested size of the data allocated at
  /// 'ptr'. Note that the actual buffer allocated might be larger
  /// than requested, but this function returns the size requested by
  /// the user.
  ///
  /// REQUIRES: tracks_allocation_sizes() is true.
  ///
  /// REQUIRES: 'ptr!=nullptr' and points to a buffer previously
  /// allocated by this allocator.
  virtual size_t requested_size(const void *ptr) const {
    CHECK(false) << "allocator does not track allocation sizes";
    return size_t(0);
  }

  /// Returns the really allocated size buffer pointed by 'ptr' if known,
  /// otherwise returns requested_size(ptr). `allocated_size(ptr)` is guaranteed
  /// to be >= `requested_size(ptr)`.
  ///
  /// REQUIRES: tracks_allocation_sizes() is
  /// true. REQUIRES: 'ptr!=nullptr' and points to a buffer previously allocated
  /// by this allocator.
  virtual size_t allocated_size(const void *ptr) const {
    return requested_size(ptr);
  }

  virtual util::Optional<AllocatorStats> get_stats() { return util::nullopt; }

  virtual AllocatorMemoryType get_memory_type() const {
    return AllocatorMemoryType::UNKNOWN;
  }
};

class AllocatorWrapper : public Allocator {
 public:
  explicit AllocatorWrapper(Allocator *allocator) : _wrapped(allocator) {}

  ~AllocatorWrapper() override {}

  Allocator *wrapped() const { return _wrapped; }

  void *allocate_row(size_t alignment, size_t num_btyes) override {
    return _wrapped->allocate_row(alignment, num_btyes);
  }

  void *allocate_row(size_t alignment, size_t num_btyes,
                     const AllocationAttributes &allocation_attr) override {
    return _wrapped->allocate_row(alignment, num_btyes, allocation_attr);
  }

  void deallocate_row(void *ptr) override { _wrapped->deallocate_row(ptr); }

  bool tracks_allocation_sizes() const override {
    return _wrapped->tracks_allocation_sizes();
  }

  size_t requested_size(const void *ptr) const override {
    return _wrapped->requested_size(ptr);
  }

  size_t allocated_size(const void *ptr) const override {
    return _wrapped->allocated_size(ptr);
  }

  AllocatorMemoryType get_memory_type() const override {
    return _wrapped->get_memory_type();
  }

 private:
  Allocator *_wrapped;
};

Allocator *cpu_allocator_base();

Allocator *cpu_allocator();

void enable_cpu_allocator_stats();

void disable_cpu_allocator_stats();

bool cpu_allocator_stats_enabled();

void enable_cpu_allocator_full_stats();

bool cpu_allocator_full_stats_enabled();

/// An object that does the underlying suballoc/free of memory for a
/// higher-level allocator.  The expectation is that the higher-level allocator
/// is doing some kind of cache or pool management so that it will call
/// SubAllocator::Alloc and Free relatively infrequently, compared to the number
/// of times its own AllocateRaw and Free methods are called.
class SubAllocator {
 public:
  /// Visitor gets called with a pointer to a memory area and its
  /// size in bytes. The index value will be numa_node for a CPU
  /// allocator and GPU id for a GPU allocator.
  typedef std::function<void(void *, int64_t index, size_t)> Visitor;

  SubAllocator(const std::vector<Visitor> &alloc_visitors,
               const std::vector<Visitor> &free_visitors);

  virtual ~SubAllocator() {}

  /// Allocates at least `num_bytes`. Returns actual number of bytes allocated
  /// in `bytes_allocated`. The caller can safely use the full `bytes_reverved`
  /// sized buffer following the returned pointer.
  virtual void *alloc(size_t alignment, size_t num_bytes,
                      size_t *bytes_reserved) = 0;
  virtual void free(void *ptr) = 0;

  virtual AllocatorMemoryType get_memory_type() const {
    return AllocatorMemoryType::UNKNOWN;
  }

 protected:
  /// Implementation of `alloc()` method must call this on newly allocated
  /// value.
  void visit_alloc(void *ptr, int64_t index, size_t num_bytes);

  /// Implementation of `free()` method must call this on value to be freed
  /// immediately before deallocation.
  void visit_free(void *ptr, int64_t index, size_t num_bytes);

  const std::vector<Visitor> _alloc_visitors;
  const std::vector<Visitor> _free_visitors;
};

}  // namespace memory
}  // namespace chime

#endif  // CHIME_CORE_MEMORY_ALLOCATOR_H_
