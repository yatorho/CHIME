// Copyright 2022.5 chime. All rights reservedts.
// author: yatorho

#ifndef CHIME_CORE_FRAMEWORK_SHAPE_VEC_H_
#define CHIME_CORE_FRAMEWORK_SHAPE_VEC_H_

#include <vector>

#include "chime/core/platform/types.h"

namespace chime {

class ShapeRep {
 public:
  static constexpr uint8 MaxDimensions() { return 254; }
};

typedef std::vector<uint64_t> DimVector;

}  // namespace chime

#endif  // CHIME_CORE_FRAMEWORK_SHAPE_VEC_H_
