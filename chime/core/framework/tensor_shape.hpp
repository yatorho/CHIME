// Copyright 2022.5 chime
// author: yatorho

#ifndef CHIME_CORE_FRAMEWORK_TENSOR_SHAPE_HPP_
#define CHIME_CORE_FRAMEWORK_TENSOR_SHAPE_HPP_

#include "chime/core/framework/common.hpp"
#include "chime/core/framework/shape_vec.hpp"

namespace chime {

class TensorShape {
 public:
  const static uint8 Max_Tensor_Shape_Axes = 254;

  TensorShape();
  explicit TensorShape(const DimVector &dim_vec);
  explicit TensorShape(DimVector &&dim_vec);

  explicit TensorShape(const TensorShape &other);
  explicit TensorShape(TensorShape &&other);

  TensorShape &operator=(const TensorShape &shape);
  TensorShape &operator=(TensorShape &&shape);

  bool operator==(const TensorShape &rhs) const;
  bool operator!=(const TensorShape &rhs) const { return !(*this == rhs); };

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

  utens_t dim_size(utens_t d) const;

  bool is_same_shape(const TensorShape &other) const;

  string shape_string() const;

 private:
  void _update_elemcnt();
  // DimVector to be update!
  DimVector _dim_vec;
  utens_t _elem_cnt;
};
} // namespace chime

#endif // CHIME_CORE_FRAMEWORK_TENSOR_SHAPE_HPP_
