// Copyright 2022.5 chime
// author: yatorho

#include "chime/core/framework/tensor_shape.hpp"

#include <sstream>

#include "chime/core/framework/common.hpp"
#include "chime/core/framework/shape_vec.hpp"

namespace chime {

void TensorShape::_update_elemcnt() {
  DCHECK_LE(dims(), Max_Tensor_Shape_Axes);
  utens_t elem_cnt = 1;
  for (utens_t s : _dim_vec) elem_cnt *= s;
  _elem_cnt = elem_cnt;
}

bool TensorShape::operator==(const TensorShape &rhs) const {
  return is_same_shape(rhs);
}

TensorShape &TensorShape::operator=(const TensorShape &shape) {
  _dim_vec = shape._dim_vec;
  _update_elemcnt();
  return *this;
}

TensorShape::TensorShape() : _dim_vec(DimVector()) { _update_elemcnt(); }

TensorShape::TensorShape(const DimVector &dim_vec) : _dim_vec(dim_vec) {
  _update_elemcnt();
}

TensorShape::TensorShape(DimVector &&dim_vec) : _dim_vec(std::move(dim_vec)) {
  _update_elemcnt();
}

TensorShape::TensorShape(const TensorShape &other) : _dim_vec(other._dim_vec) {
  _update_elemcnt();
}

TensorShape::TensorShape(TensorShape &&other)
    : _dim_vec(std::move(other._dim_vec)) {
  _update_elemcnt();
}

void TensorShape::add_dim(utens_t size) {
  _dim_vec.push_back(size);
  _update_elemcnt();
}

void TensorShape::append_shape(const TensorShape &shape) {
  _dim_vec.insert(_dim_vec.end(), shape._dim_vec.begin(), shape._dim_vec.end());
  _update_elemcnt();
}

void TensorShape::append_shape(TensorShape &&shape) {
  _dim_vec.insert(_dim_vec.end(), std::move(shape)._dim_vec.begin(),
                  std::move(shape)._dim_vec.begin());
  _update_elemcnt();
}

void TensorShape::insert_dim(uint32_t d, uint32_t size) {
  _dim_vec.insert(_dim_vec.begin() + d, size);
  _update_elemcnt();
}

void TensorShape::set_dim(uint32_t d, uint32_t size) {
  _dim_vec[d] = size;
  _update_elemcnt();
}

void TensorShape::remove_dim_range(uint32_t begin, uint32_t end) {
  DCHECK_LE(begin, end);
  DCHECK_GE(begin, 0ull);
  DCHECK_GE(end, 0ull);
  DCHECK_LT(end, dims());
  _dim_vec.erase(_dim_vec.begin() + begin, _dim_vec.begin() + end);
  _update_elemcnt();
}

utens_t TensorShape::dim_size(utens_t d) const {
  DCHECK_GE(d, 0ul);
  DCHECK_LT(d, dims());
  return _dim_vec.at(d);
}

bool TensorShape::is_same_shape(const TensorShape &other) const {
  return _dim_vec == other._dim_vec;
}

string TensorShape::shape_string() const {
  std::ostringstream stream;
  for (size_t i = 0; i < dims(); i++) stream << _dim_vec[i] << " ";
  stream << "(" << _elem_cnt << ")";
  return stream.str();
}

} // namespace chime
