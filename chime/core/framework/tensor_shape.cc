// Copyright 2022.5 chime
// author: yatorho

#include "chime/core/framework/tensor_shape.hpp"

#include "chime/core/framework/common.hpp"
#include "chime/core/framework/shape_vec.hpp"

namespace chime {

void TensorShape::_update_elemcnt() {
  utens_t elem_cnt = 1;
  for (utens_t s : _dim_vec) elem_cnt *= s;
  _elem_cnt = elem_cnt;
}

bool TensorShape::operator==(const TensorShape &rhs) const {
  return _dim_vec == rhs._dim_vec;
}

TensorShape &TensorShape::operator=(const TensorShape &shape) {
  _dim_vec = shape._dim_vec;
  _update_elemcnt();
  return *this;
}

TensorShape::TensorShape(const DimVector& dim_vec) : _dim_vec(dim_vec) {
  _update_elemcnt();
}

TensorShape::TensorShape(DimVector&& dim_vec) : _dim_vec(std::move(dim_vec)) {
  _update_elemcnt();
}



} // namespace chime
