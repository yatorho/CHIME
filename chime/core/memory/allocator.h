// Copyright by 2022.6 chime
// author: yatorho

#ifndef CHIME_CORE_MEMORY_ALLOCATOR_H_
#define CHIME_CORE_MEMORY_ALLOCATOR_H_

#include <stdlib.h>

#include <functional>

#include "chime/core/framework/types.hpp"
#include "chime/core/platform/logging.hpp"
#include "chime/core/platform/macros.h"

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

class Allocator {
 public:
  // Align to 64 bytes boundary.
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
  /// REQUIRES: tracks_allocation_sizes() is
  /// true. REQUIRES: 'ptr!=nullptr' and points to a buffer previously allocated
  /// by this allocator.
  virtual size_t allocated_size(const void *ptr) const {
    return requested_size(ptr);
  }
};

}  // namespace memory
}  // namespace chime

#endif  // CHIME_CORE_MEMORY_ALLOCATOR_H_
