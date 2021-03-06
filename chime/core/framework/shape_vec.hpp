// Copyright 2022.5 chime
// author: yatorho

#ifndef CHIME_CORE_FRAMEWORK_SHAPE_VEC_HPP_
#define CHIME_CORE_FRAMEWORK_SHAPE_VEC_HPP_

#include <vector>

#include "chime/core/framework/types.hpp"

namespace chime {

class ShapeRep {
 public:
  static constexpr utens_t MaxDimensions() { return 254; }
};

typedef std::vector<utens_t> DimVector;
}  // namespace chime

#endif  // CHIME_CORE_FRAMEWORK_SHAPE_VEC_HPP_
