// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

#ifndef CHIME_CORE_MEMORY_TYPED_ALLOCATOR_H_
#define CHIME_CORE_MEMORY_TYPED_ALLOCATOR_H_

#include <limits>

#include "chime/core/framework/type_traits.h"
#include "chime/core/memory/allocator.h"

namespace chime {
namespace memory {

/// Convenience functions to do typed allocation. C++ constructors and
/// destructors are invoked for complex types if necessary.
class TypedAllocator {
 public:
  template <typename Tp>
  static Tp *Allocate(Allocator *raw_allocator, size_t num_elements,
                      const AllocationAttributes &allocation_attr) {
    if (num_elements > (std::numeric_limits<size_t>::max() / sizeof(Tp))) {
      return nullptr;
    }

    void *p =
        raw_allocator->AllocateRaw(Allocator::ALLOCATOR_ALIGNMENT,
                                   sizeof(Tp) * num_elements, allocation_attr);
    Tp *typed_p = reinterpret_cast<Tp *>(p);
    if (typed_p != nullptr) RunCtor<Tp>(raw_allocator, typed_p, num_elements);
    return typed_p;
  }

  template <typename Tp>
  static void Deallocate(Allocator *raw_allocator, Tp *ptr,
                         size_t num_elements) {
    if (ptr) {
      RunDtor<Tp>(raw_allocator, ptr, num_elements);
      raw_allocator->DeallocateRaw(ptr);
    }
  }

 private:
  template <typename Tp>
  static void RunCtor(Allocator *raw_allocator, Tp *p, size_t n) {
    static_assert(is_simple_type<Tp>::value, "T is not a simple type.");
  }

  template <typename Tp>
  static void RunDtor(Allocator *raw_allocator, Tp *p, size_t n) {}
};

}  // namespace memory
}  // namespace chime

#endif  // CHIME_CORE_MEMORY_TYPED_ALLOCATOR_H_
