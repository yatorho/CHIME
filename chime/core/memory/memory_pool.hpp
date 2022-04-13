// Copyright by 2022.4 chime
// author: yatorho

#ifndef CHIME_CORE_MEMORY_CHIME_MP_HPP_
#define CHIME_CORE_MEMORY_CHIME_MP_HPP_

#include "chime/core/framework/common.hpp"

namespace chime {

struct MemoryBlock {
  enum BlockStatus {
    OCCOPYIED,
    FREE
  };

  BlockStatus block_status;
  mems_t size;
  BlockStatus *rear;
  BlockStatus *next;
};  // struct MemoryBlock

class ChimeMP {};// class ChimeMP
}  // namespace chime

#endif  // CHIME_CORE_MEMORY_CHIME_MP_HPP_
