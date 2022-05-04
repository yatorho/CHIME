// Copyright 2022.5 chime
// author: yatorho

#ifndef CHIME_CORE_FRAMEWORK_TENSOR_HPP_
#define CHIME_CORE_FRAMEWORK_TENSOR_HPP_

#include <memory>

#include "chime/core/framework/common.hpp"
#include "chime/core/framework/syncedmem.hpp"
#include "chime/core/framework/tensor_shape.hpp"
#include "chime/core/framework/type.hpp"
#include "chime/core/memory/mem_optimizer.h"

namespace chime {

class Tensor;

class Tensor {
 public:
  using MemOp = memory::MemoryOptimizer;
  using DataPtr = std::shared_ptr<SyncedMemory>;
  using MemOpPtr = MemOp *;

  Tensor();
  explicit Tensor(DataType dtype);
  explicit Tensor(DataType dtype, const TensorShape &shape);
  explicit Tensor(MemOp &mem_op, DataType dtype, const TensorShape &shape);

  Tensor(const Tensor &other);
  Tensor(Tensor &&other);

  Tensor &operator=(const Tensor &other);
  Tensor &operator=(Tensor &&other);

 private:
  DataType _dtype;
  TensorShape _shape;

  DataPtr _data;

 public:
  ~Tensor();
  DataType dtype() const { return _dtype; }

  const TensorShape &shape() const { return _shape; }

  size_t dims() const { return _shape.dims(); }

  utens_t dims_size(utens_t d) const { return _shape.dim_size(d); };

  utens_t num_elements() const { return _shape.num_elements(); }
};
} // namespace chime

#endif // CHIME_CORE_FRAMEWORK_TENSOR_HPP_