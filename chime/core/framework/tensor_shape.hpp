// Copyright 2022.5 chime
// author: yatorho

#ifndef CHIME_CORE_FRAMEWORK_TENSOR_SHAPE_HPP_
#define CHIME_CORE_FRAMEWORK_TENSOR_SHAPE_HPP_

#include "chime/core/framework/shape_vec.hpp"
#include "chime/core/framework/types.hpp"
#include "chime/core/schema/tensor_shape.pb.h"
#include "chime/core/platform/logging.hpp"

namespace chime {

class TensorShape {
 public:
  const static uint8 MAX_TENSOR_SHAPE_DIMS = 254;

  TensorShape();
  explicit TensorShape(const DimVector &dim_vec);
  explicit TensorShape(DimVector &&dim_vec);

  explicit TensorShape(const TensorShape &other);
  explicit TensorShape(TensorShape &&other);

  TensorShape &operator=(const TensorShape &shape);
  TensorShape &operator=(TensorShape &&shape);

  bool operator==(const TensorShape &rhs) const;
  bool operator!=(const TensorShape &rhs) const { return !(*this == rhs); };

  utens_t operator[](utens_t d) const { return DimAt(d); }

  void AddDim(utens_t size);

  void AppendShape(const TensorShape &shape);

  void AppendShape(TensorShape &&shape);

  void InsertDim(utens_t d, utens_t size);

  void SetDim(utens_t d, utens_t size);

  void RemoveDimRange(utens_t begin, utens_t end);

  void RemoveLastDims(utens_t n) {
    DCHECK_LE(n, Dims());
    RemoveDimRange(Dims() - n, Dims());
  }

  void RemoveDim(utens_t d) {
    DCHECK_GE(d, 0u);
    RemoveDimRange(d, d + 1);
  }

  inline utens_t DimAt(utens_t index) const { return _dim_vec.at(index); }

  inline size_t Dims() const { return _dim_vec.size(); }

  inline utens_t NumElements() const { return _elem_cnt; }

  // Return one dimension's size, not elements count
  size_t DimSize(utens_t d) const;

  bool IsSameShape(const TensorShape &other) const;

  bool IsSameShape(TensorShape &&other) const;

  string ShapeString() const;

  inline utens_t Offset(const TensorShape &shape) const {
    DCHECK_LE(shape.Dims(), Dims());
    utens_t offset = 0;
    for (size_t i = 0; i < Dims(); i++) {
      offset *= _dim_vec[i];
      if (shape.Dims() > i) {
        DCHECK_LT(shape.DimAt(i), _dim_vec[i]);
        offset += shape.DimAt(i);
      }
    }
    return offset;
  }

  // check whether there is 0 size dimension in shape
  bool CheckLegality() const { return _legality; }

  bool FromProto(const TensorShapeProto &proto);

  void AsProto(TensorShapeProto *proto) const;

  friend class TensorShapeTest;

 private:
  void _UpdateElemcnt();
  // DimVector to be update!
  DimVector _dim_vec;
  utens_t _elem_cnt;

  // record whether there are 0 size dimension in shape
  bool _legality;

 public:
  static bool IsValid(const TensorShapeProto &proto);
};
}  // namespace chime

#endif  // CHIME_CORE_FRAMEWORK_TENSOR_SHAPE_HPP_
