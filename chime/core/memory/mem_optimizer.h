// Copyright by 2022.4 chime
// author: yatorho

#ifndef CHIME_CORE_MEMORY_MEM_OPTIMIZER_HPP_
#define CHIME_CORE_MEMORY_MEM_OPTIMIZER_HPP_

#include "chime/core/framework/common.hpp"

namespace chime {
namespace memory {

class MemoryOptimizer {
 public:
  typedef enum {
    FREE_FROM_HOST_MEMORY = 0,
    FREE_FROM_DEVICE0_MEMORY = 1,
    FREE_FROM_DEVICE1_MEMORY = 2,
    FREE_FROM_DEVICE2_MEMORY = 3,
    FREE_FROM_DEVICE3_MEMORY = 4,
    FREE_FROM_DEVICE4_MEMORY = 5
  } FreeType;

  typedef enum {
    MALLOC_FROM_HOST_MEMORY = 0,
    MALLOC_FROM_DEVICE0_MEMORY = 1,
    MALLOC_FROM_DEVICE1_MEMORY = 2,
    MALLOC_FROM_DEVICE2_MEMORY = 3,
    MALLOC_FROM_DEVICE3_MEMORY = 4,
    MALLOC_FROM_DEVICE4_MEMORY = 5,
  } MallocType;

  typedef enum {
    COPY_FROM_HOST_MEMORY = 0,
    COPY_FROM_DEVICE0_MEMORY = 1,
    COPY_FROM_DEVICE1_MEMORY = 1,
    COPY_FROM_DEVICE2_MEMORY = 1,
    COPY_FROM_DEVICE3_MEMORY = 1,
    COPY_FROM_DEVICE4_MEMORY = 1,
  } CopyType;

  virtual ~MemoryOptimizer();

  virtual void malloc(void **ptr, mems_t size, MallocType type);
  virtual void free(void *ptr, FreeType type);

  virtual void memcpy(void *dst, const void *src, mems_t size, CopyType type);

  MemoryOptimizer() = default;

 private:
  DISABLE_COPY_AND_ASSIGN(MemoryOptimizer);
};

class DefaultAllocator : public MemoryOptimizer {
 public:
  DefaultAllocator() = default;
  void malloc(void **ptr, mems_t size, MallocType type) override;
  void free(void *ptr, FreeType type) override;
  void memcpy(void *dst, const void *src, mems_t size, CopyType type) override;

 private:
  DISABLE_COPY_AND_ASSIGN(DefaultAllocator);
};


} // namespace memory
} // namespace chime
#endif // CHIME_CORE_MEMORY_MEM_OPTIMIZER_HPP_