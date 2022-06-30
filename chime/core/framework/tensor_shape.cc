// Copyright 2022.5 chime
// author: yatorho

#include "chime/core/framework/tensor_shape.hpp"

#include <cstdint>
#include <sstream>

#include "chime/core/framework/shape_vec.hpp"
#include "chime/core/util/overflow.h"

namespace chime {

void TensorShape::_update_elemcnt() {
  DCHECK_LE(dims(), MAX_TENSOR_SHAPE_DIMS);

  _legality = true;
  tens_t elem_cnt = 1;
  for (tens_t s : _dim_vec) {
    if (s == 0) _legality = false;
    elem_cnt = multiply_without_overflow(elem_cnt, s);
    if (elem_cnt < 0)
      LOG(FATAL) << "results in overflow when computing number of elements";
  }
  _elem_cnt = (utens_t)elem_cnt;
}

bool TensorShape::operator==(const TensorShape &rhs) const {
  return is_same_shape(rhs);
}

TensorShape &TensorShape::operator=(const TensorShape &shape) {
  _dim_vec = shape._dim_vec;
  _update_elemcnt();
  return *this;
}

TensorShape &TensorShape::operator=(TensorShape &&shape) {
  _dim_vec = std::move(shape._dim_vec);
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
  if (_dim_vec.data() != nullptr)
    DCHECK_NE(_dim_vec.data(), other._dim_vec.data());
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
  _dim_vec.insert(_dim_vec.end(), std::move(shape._dim_vec.begin()),
                  std::move(shape._dim_vec.end()));
  _update_elemcnt();
}

void TensorShape::insert_dim(utens_t d, utens_t size) {
  _dim_vec.insert(_dim_vec.begin() + d, size);
  _update_elemcnt();
}

void TensorShape::set_dim(utens_t d, utens_t size) {
  _dim_vec[d] = size;
  _update_elemcnt();
}

void TensorShape::remove_dim_range(utens_t begin, utens_t end) {
  DCHECK_LE(begin, end);
  DCHECK_GE(begin, 0ull);
  DCHECK_GE(end, 0ull);
  DCHECK_LE(end, dims());
  _dim_vec.erase(_dim_vec.begin() + begin, _dim_vec.begin() + end);
  _update_elemcnt();
}

utens_t TensorShape::dim_size(utens_t d) const {
  return dim_at(d) * sizeof(utens_t);
}

bool TensorShape::is_same_shape(const TensorShape &other) const {
  return _dim_vec == other._dim_vec;
}

bool TensorShape::is_same_shape(TensorShape &&other) const {
  return _dim_vec == other._dim_vec;
}

string TensorShape::shape_string() const {
  std::ostringstream stream;
  stream << "(";
  for (size_t i = 0; i < dims(); i++)
    i != dims() - 1 ? stream << _dim_vec[i] << ", " : stream << _dim_vec[i];
  stream << ")";
  return stream.str();
}

bool TensorShape::from_proto(const TensorShapeProto &proto) {
  if (!TensorShape::is_valid(proto)) return false;

  DimVector dim_vector;
  for (auto &d : proto.dims()) {
    dim_vector.push_back(d.size());
  }

  _dim_vec = dim_vector;
  _update_elemcnt();
  return true;
}

void TensorShape::as_proto(TensorShapeProto *proto) const {
  proto->Clear();
  if (!check_legality())
    LOG(ERROR) << "Serializing an illegal TensorShape number as proto file.";
  for (size_t i = 0; i < dims(); i++) {
    proto->add_dims()->set_size(dim_at(i));
  }
}

bool TensorShape::is_valid(const TensorShapeProto &proto) {
  if (proto.dims().size() > MAX_TENSOR_SHAPE_DIMS) return false;
  int64_t num_elements = 1;
  for (const auto &d : proto.dims()) {
    if (num_elements > 0) {
      num_elements = multiply_without_overflow(num_elements, d.size());
      if (num_elements < 0) return false;
    }
  }
  return true;
}

}  // namespace chime
