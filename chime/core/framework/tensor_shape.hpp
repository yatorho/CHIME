// Copyright 2022.5 chime
// author: yatorho

#ifndef CHIME_CORE_FRAMEWORK_TENSOR_SHAPE_HPP_
#define CHIME_CORE_FRAMEWORK_TENSOR_SHAPE_HPP_

#include "chime/core/framework/common.hpp"
#include "chime/core/framework/shape_vec.hpp"
#include "chime/core/schema/tensor_shape.pb.h"

namespace chime {

class TensorShape {
 public:
  const static uint8 Max_Tensor_Shape_Dims = 254;

  TensorShape();
  explicit TensorShape(const DimVector &dim_vec);
  explicit TensorShape(DimVector &&dim_vec);

  explicit TensorShape(const TensorShape &other);
  explicit TensorShape(TensorShape &&other);

  TensorShape &operator=(const TensorShape &shape);
  TensorShape &operator=(TensorShape &&shape);

  bool operator==(const TensorShape &rhs) const;
  bool operator!=(const TensorShape &rhs) const { return !(*this == rhs); };

  utens_t operator[](utens_t d) const { return dim_at(d); }

  void add_dim(utens_t size);

  void append_shape(const TensorShape &shape);

  void append_shape(TensorShape &&shape);

  void insert_dim(utens_t d, utens_t size);

  void set_dim(utens_t d, utens_t size);

  void remove_dim_range(utens_t begin, utens_t end);

  void remove_last_dims(utens_t n) {
    DCHECK_LE(n, dims());
    remove_dim_range(dims() - n, dims());
  }

  void remove_dim(utens_t d) {
    DCHECK_GE(d, 0u);
    remove_dim_range(d, d + 1);
  }

  inline utens_t dim_at(utens_t index) const { return _dim_vec.at(index); }

  inline size_t dims() const { return _dim_vec.size(); }

  inline utens_t num_elements() const { return _elem_cnt; }

  // Return one dimension's size, not elements count
  utens_t dim_size(utens_t d) const;

  bool is_same_shape(const TensorShape &other) const;

  bool is_same_shape(TensorShape &&other) const;

  string shape_string() const;

  inline utens_t offset(const TensorShape &shape) const {
    DCHECK_LE(shape.dims(), dims());
    utens_t offset = 0;
    for (size_t i = 0; i < dims(); i++) {
      offset *= _dim_vec[i];
      if (shape.dims() > i) {
        DCHECK_LT(shape.dim_at(i), _dim_vec[i]);
        offset += shape.dim_at(i);
      }
    }
    return offset;
  }

  // check whether there is 0 size dimension in shape
  bool check_legality() const { return _legality; }

  bool from_proto(const TensorShapeProto &proto);

  friend class TensorShapeTest;

 private:
  void _update_elemcnt();
  // DimVector to be update!
  DimVector _dim_vec;
  utens_t _elem_cnt;

  // record whether there are 0 size dimension in shape
  bool _legality;

 public:
  static bool is_valid(const TensorShapeProto &proto);
};
} // namespace chime

#endif // CHIME_CORE_FRAMEWORK_TENSOR_SHAPE_HPP_
