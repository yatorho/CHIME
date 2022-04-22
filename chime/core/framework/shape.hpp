// Copyright 2022.4 chime
// author: yatorho

#ifndef CHIME_CORE_FRAMEWORK_SHAPE_HPP_
#define CHIME_CORE_FRAMEWORK_SHAPE_HPP_

#include "chime/core/framework/common.hpp"

namespace chime {
class Dimension {
 private:
  Dimension();
  Dimension(utens_t value);
  ~Dimension() {}
  const utens_t _value;

  DISABLE_COPY_AND_ASSIGN(Dimension);
};

class DimensionHandle {
 public:
  DimensionHandle() {}
  bool same_handle(DimensionHandle &d) const { return _ptr == d._ptr; }
  std::size_t handle() const { return reinterpret_cast<std::size_t>(_ptr); }

 private:
  DimensionHandle(const Dimension *dim) { _ptr = dim; }
  const Dimension *operator->() const { return _ptr; }
  bool is_set() const { return _ptr != nullptr; }
  
  const Dimension *_ptr;
};

class Shape {
 private:
  Shape();

}; // class Shape

} // namespace chime
#endif // CHIME_CORE_FRAMEWORK_SHAPE_HPP_
