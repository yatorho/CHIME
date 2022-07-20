// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

#include "chime/core/memory/allocator.h"

#include "chime/core/platform/test.hpp"

namespace chime {
namespace memory {

TEST(CPUAllocatorTest, Simple) { 
  EnableCPUAllocatorStats();
  Allocator *allocator = CPUAllocator();
  
}

}  // namespace memory
}  // namespace chime
