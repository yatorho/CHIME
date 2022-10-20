// Copyright 2022.8 chime. All rights reserved.
// author: yatorho

#ifndef CHIME_CORE_FRAMEWORK_TENSOR_SHAPE_H_
#define CHIME_CORE_FRAMEWORK_TENSOR_SHAPE_H_

#include <cstdint>

#include "chime/core/framework/shape_vec.h"
#include "chime/core/framework/tensor_shape.pb.h"
#include "chime/core/platform/logging.hpp"
#include "chime/core/platform/types.h"

namespace chime {
namespace core {

class TensorShape {
 public:
  static constexpr uint8_t MAX_TENSOR_SHAPE_DIMS = ShapeRep::MaxDimensions();

  TensorShape();

  TensorShape(const DimVector &dim_vec);
  TensorShape(DimVector &&dim_vec);

  TensorShape(const TensorShape &other);
  TensorShape(TensorShape &&other);

  TensorShape &operator=(const TensorShape &shape);
  TensorShape &operator=(TensorShape &&shape);

  bool operator==(const TensorShape &rhs) const;
  bool operator!=(const TensorShape &rhs) const { return !(*this == rhs); };

  uint64_t operator[](uint8_t d) const { return At(d); }
  uint64_t operator()(uint8_t d) const { return At(d); }

  size_t NumDims() const { return _dim_vec.size(); }
  void AddDim(int64_t size);
  size_t At(uint8_t index) const { return _dim_vec.at(index); }

  void AppendShape(const TensorShape &shape);
  void AppendShape(TensorShape &&shape);

  void InsertDim(uint8_t d, int64_t size);
  void SetDim(uint8_t d, int64_t size);

  void RemoveDimRange(uint8_t begin, uint8_t end);
  void RemoveLastDims(uint8_t n) {
    DCHECK_LE(n, NumDims());
    RemoveDimRange(NumDims() - n, NumDims());
  }
  void RemoveDim(uint8_t d) {
    DCHECK_GE(d, 0u);
    RemoveDimRange(d, d + 1);
  }

  size_t NumElements() const { return _elem_cnt; }

  bool IsSameShape(const TensorShape &other) const;

  std::string ShapeString() const;

  inline uint64_t Offset(const TensorShape &shape) const {
    DCHECK_LE(shape.NumDims(), NumDims());
    uint64_t offset = 0;
    for (size_t i = 0; i < NumDims(); ++i) {
      offset *= _dim_vec[i];
      if (shape.NumDims() > i) {
        DCHECK_LT(shape.At(i), _dim_vec[i]);
        offset += shape.At(i);
      }
    }
    return offset;
  }

  /// See field `_legality`
  bool CheckLegality() const { return _legality; }

  bool FromProto(const TensorShapeProto &proto);

  bool ToProto(TensorShapeProto *proto) const;

  friend class TensorShapeTest;

 protected:
  void UpdateElemcntAndLegality();

  DimVector _dim_vec;
  uint64_t _elem_cnt;

  /// Flag to indicate whether there is any zero dimension in shape.
  /// If `CheckLegality()` returns false, it means there is at least one zero
  /// dimension in shape.
  bool _legality;
};

}  // namespace core
}  // namespace chime

#endif  // CHIME_CORE_FRAMEWORK_TENSOR_SHAPE_H_
