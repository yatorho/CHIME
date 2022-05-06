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
  using DeviceName = DeviceSupported;

  typedef enum {
    HOST = 0, // normally refering cpu.
    DEVICE = 1
  } OperateFrom;

 public:
  Tensor();
  explicit Tensor(DataType dtype);
  explicit Tensor(DataType dtype, const TensorShape &shape);
  explicit Tensor(MemOp &mem_op, DataType dtype, const TensorShape &shape);

  Tensor(const Tensor &other);
  Tensor(Tensor &&other);

  Tensor &operator=(const Tensor &other);
  Tensor &operator=(Tensor &&other);

  bool operator==(const Tensor &other);
  bool operator==(Tensor &&other);

  utens_t operator[](utens_t d) const;

 private:
  DataType _dtype;
  TensorShape _shape;

  DataPtr _buffer;

  DeviceName _d_at;

 public:
  ~Tensor();
  DataType dtype() const { return _dtype; }

  const TensorShape &shape() const { return _shape; }

  size_t dims() const { return _shape.dims(); }

  utens_t dims_size(utens_t d) const { return _shape.dim_size(d); };

  bool is_same_shape(const Tensor &other) const {
    return _shape.is_same_shape(other._shape);
  }

  bool is_same_shape(Tensor &&other) const {
    return _shape.is_same_shape(std::move(other._shape));
  }

  utens_t num_elements() const { return _shape.num_elements(); }

  inline SyncedMemory::SyncedHead head() const {
    DCHECK(_buffer);
    return _buffer->head();
  }

  bool is_initialized() const;

  mems_t total_bytes() const;

  mems_t allocated_bytes() const;

  bool copy_from(const Tensor &other, const TensorShape &shape) {
    if (other.num_elements() != shape.num_elements()) return false;
    _copy_from_internal(other, shape);
    return true;
  }

  template<DataType T>
  const typename EnumToDataType<T>::type *host_data();

  template<DataType T>
  typename EnumToDataType<T>::type *mutable_host_data();

  template<DataType T>
  void set_host_data(typename EnumToDataType<T>::type *buffer);

  template<DataType T>
  const typename EnumToDataType<T>::type *data(OperateFrom of);

  template<DataType T>
  typename EnumToDataType<T>::type *muatable_data(OperateFrom of) const;

  template<DataType T>
  void set_data(OperateFrom of);

 private:
  inline void _copy_from_internal(const Tensor &other,
                                  const TensorShape &shape) {
    DCHECK_EQ(shape.num_elements(), other.num_elements());
    DataType other_dtype = other.dtype();
    _shape = shape;
    _set_dtype(other_dtype);
    if (_buffer != other._buffer) { NOT_IMPLEMENTED; }
  }

  void _set_dtype(DataType t) { _dtype = t; }
};
} // namespace chime

#endif // CHIME_CORE_FRAMEWORK_TENSOR_HPP_