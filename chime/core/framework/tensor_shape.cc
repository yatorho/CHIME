// Copyright 2022.8 chime. All rights reserved.
// author: yatorho

#include "chime/core/framework/tensor_shape.h"

#include "chime/core/framework/shape_vec.h"
#include "chime/core/platform/logging.hpp"
#include "chime/core/util/overflow.h"

namespace chime {
namespace core {

namespace {

template <class T>
struct ProtoHelper {
  static bool IsValid(const T &proto) {
    if (proto.dims().size() > TensorShape::MAX_TENSOR_SHAPE_DIMS) return false;
    int64_t num_elements = 1;
    for (auto &d : proto.dims()) {
      if (num_elements > 0) {
        num_elements = MultiplyWithoutOverflow(num_elements, d.size());
        if (num_elements <= 0) return false;
      }
    }
    return true;
  }
};

}  // namespace

void TensorShape::UpdateElemcntAndLegality() {
  DCHECK_LE(NumDims(), MAX_TENSOR_SHAPE_DIMS);

  _legality = true;
  int64_t elem_cnt = 1;
  for (size_t s : _dim_vec) {
    if (s == 0) _legality = false;
    elem_cnt = MultiplyWithoutOverflow(elem_cnt, s);
    if (elem_cnt < 0)
      LOG(FATAL) << "results in overflow when computing number of elements";
  }
  _elem_cnt = (uint64_t)elem_cnt;
}

bool TensorShape::IsSameShape(const TensorShape &other) const {
  return _dim_vec == other._dim_vec;
}

TensorShape::TensorShape() { UpdateElemcntAndLegality(); }

TensorShape::TensorShape(const DimVector &dim_vec) : _dim_vec(dim_vec) {
  UpdateElemcntAndLegality();
}

TensorShape::TensorShape(DimVector &&dim_vec) : _dim_vec(std::move(dim_vec)) {
  UpdateElemcntAndLegality();
}

TensorShape::TensorShape(const TensorShape &other) : _dim_vec(other._dim_vec) {
  if (_dim_vec.data() != nullptr)
    DCHECK_NE(_dim_vec.data(), other._dim_vec.data());
  UpdateElemcntAndLegality();
}

TensorShape::TensorShape(TensorShape &&other)
    : _dim_vec(std::move(other._dim_vec)) {
  UpdateElemcntAndLegality();
}

TensorShape &TensorShape::operator=(const TensorShape &shape) {
  _dim_vec = shape._dim_vec;
  UpdateElemcntAndLegality();
  return *this;
}

TensorShape &TensorShape::operator=(TensorShape &&shape) {
  _dim_vec = std::move(shape._dim_vec);
  UpdateElemcntAndLegality();
  return *this;
}

bool TensorShape::operator==(const TensorShape &rhs) const {
  return IsSameShape(rhs);
}

void TensorShape::InsertDim(uint8_t d, int64_t size) {
  DCHECK_LE(NumDims(), MAX_TENSOR_SHAPE_DIMS);
  DCHECK_LE(d, NumDims());
  _dim_vec.insert(_dim_vec.begin() + d, size);
  UpdateElemcntAndLegality();
}

void TensorShape::AddDim(int64_t size) {
  DCHECK_LE(NumDims(), MAX_TENSOR_SHAPE_DIMS);
  _dim_vec.push_back(size);
  UpdateElemcntAndLegality();
}

std::string TensorShape::ShapeString() const {
  std::stringstream ss;
  ss << "TensorShape({";
  for (size_t i = 0; i < _dim_vec.size(); ++i) {
    if (i > 0) ss << ", ";
    ss << _dim_vec[i];
  }
  ss << "})";
  return ss.str();
}

void TensorShape::SetDim(uint8_t d, int64_t size) {
  DCHECK_LE(d, NumDims());
  _dim_vec[d] = size;
  UpdateElemcntAndLegality();
}

void TensorShape::AppendShape(const TensorShape &other) {
  DCHECK_LE(NumDims() + other.NumDims(), MAX_TENSOR_SHAPE_DIMS);
  _dim_vec.insert(_dim_vec.end(), other._dim_vec.begin(), other._dim_vec.end());
  UpdateElemcntAndLegality();
}

void TensorShape::AppendShape(TensorShape &&other) {
  DCHECK_LE(NumDims() + other.NumDims(), MAX_TENSOR_SHAPE_DIMS);
  _dim_vec.insert(_dim_vec.end(), other._dim_vec.begin(), other._dim_vec.end());
  UpdateElemcntAndLegality();
}

void TensorShape::RemoveDimRange(uint8_t begin, uint8_t end) {
  DCHECK_LE(begin, end);
  DCHECK_LE(end, NumDims());
  _dim_vec.erase(_dim_vec.begin() + begin, _dim_vec.begin() + end);
  UpdateElemcntAndLegality();
}

bool TensorShape::FromProto(const TensorShapeProto &proto) {
  if (!ProtoHelper<TensorShapeProto>::IsValid(proto)) return false;
  DimVector dim_vec;
  for (const auto &d : proto.dims()) dim_vec.push_back(d.size());

  _dim_vec = std::move(dim_vec);
  UpdateElemcntAndLegality();
  return true;
}

bool TensorShape::ToProto(TensorShapeProto *proto) const {
  proto->Clear();
  if (!CheckLegality()) return false;
  for (size_t i = 0; i < NumDims(); ++i) {
    proto->add_dims()->set_size(At(i));
  }
  return true;
}

}  // namespace core
}  // namespace chime
