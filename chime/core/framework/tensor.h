// Copyright 2022.5 chime
// author: yatorho

#ifndef CHIME_CORE_FRAMEWORK_TENSOR_HPP_
#define CHIME_CORE_FRAMEWORK_TENSOR_HPP_

#include <memory>

#include "chime/core/framework/common.hpp"
#include "chime/core/framework/shape_vec.hpp"
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
    HOST = 0, // normally referring cpu.
    DEVICE = 1
  } OperateFrom;

 public:
  Tensor();
  explicit Tensor(DataType dtype);
  explicit Tensor(DataType dtype, const TensorShape &shape);
  explicit Tensor(MemOp &mem_op, DataType dtype, const TensorShape &shape);
  explicit Tensor(MemOp &mem_op, DataType dtype, const TensorShape &shape,
                  DeviceName d_name);

  Tensor(const Tensor &other);
  Tensor(Tensor &&other);
  ~Tensor();

  Tensor &operator=(const Tensor &other);
  Tensor &operator=(Tensor &&other);

  bool operator==(const Tensor &other);
  bool operator==(Tensor &&other);

  utens_t operator[](utens_t d) const;

 private:
  DataType _dtype;
  TensorShape _shape;
  DeviceName _dname;

  DataPtr _buffer;

 public:
  DataType dtype() const { return _dtype; }

  DeviceName device_name() const { return _dname; }

  const TensorShape &shape() const { return _shape; }

  size_t dims() const { return _shape.dims(); }

  utens_t dim_at(utens_t index) const { return _shape.dim_at(index); }

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

  bool is_legal_shape() const;

  bool is_scalar() const;

  bool check_dtype() const;

  mems_t total_bytes() const;

  mems_t allocated_bytes() const;

  bool copy_from(const Tensor &other, const TensorShape &shape,
                 bool host_only = true) {
    if (other.num_elements() != shape.num_elements()
        || other.dtype() != dtype())
      return false;
    _copy_from_internal(other, shape, host_only);
    return true;
  }

  template<DataType T>
  const typename EnumToDataType<T>::type *host_data() {
    return data<T>(HOST);
  }

  template<DataType T>
  typename EnumToDataType<T>::type *mutable_host_data() {
    return mutable_data<T>(HOST);
  }

  template<DataType T>
  void set_host_data(typename EnumToDataType<T>::type *data) {
    set_data<T>(data, HOST);
  }

  template<DataType T>
  const typename EnumToDataType<T>::type *device_data() {
    return data<T>(DEVICE);
  }

  template<DataType T>
  typename EnumToDataType<T>::type *mutable_device_data() {
    return mutable_data<T>(DEVICE);
  }

  template<DataType T>
  void set_device_data(typename EnumToDataType<T>::type *data) {
    set_data<T>(data, DEVICE);
  }

  template<DataType T>
  const typename EnumToDataType<T>::type *data(OperateFrom of) {
    return of == HOST ? static_cast<const typename EnumToDataType<T>::type *>(
             _buffer->host_mem())
                      : static_cast<const typename EnumToDataType<T>::type *>(
                        _buffer->device_mem(device_name()));
  }

  template<DataType T>
  typename EnumToDataType<T>::type *mutable_data(OperateFrom of) {
    return static_cast<typename EnumToDataType<T>::type *>(buffer(of));
  }

  template<DataType T>
  void set_data(typename EnumToDataType<T>::type *data, OperateFrom of) {
    if (of == HOST) {
      DCHECK(data);
      _buffer->set_host_mem(static_cast<void *>(data));
    } else {
      NOT_IMPLEMENTED;
    }
  }

  template<DataType T>
  typename EnumToDataType<T>::type at(const TensorShape &shape,
                                      OperateFrom from) {
    if (head() == SyncedMemory::UNINITIALIZED)
      LOG(WARNING)
        << "get value from a tensor whose memory was in uninitialized status";
    return data<T>(from)[_shape.offset(shape)];
  }

  template<DataType T>
  typename EnumToDataType<T>::type at(const DimVector &d_vector,
                                      OperateFrom from) {
    if (head() == SyncedMemory::UNINITIALIZED)
      LOG(WARNING)
        << "get value from a tensor whose memory was in uninitialized status";
    return data<T>(from)[_shape.offset(TensorShape(d_vector))];
  }

  void *buffer(OperateFrom of);

  friend class TensorTest;

 private:
  inline void _copy_from_internal(const Tensor &other, const TensorShape &shape,
                                  bool host_only) {
    DCHECK_EQ(shape.num_elements(), other.num_elements());
    DCHECK_EQ(dtype(), other.dtype());
    DataType other_dtype = other.dtype();
    _shape = shape;
    _set_dtype(other_dtype);
    if (_buffer != other._buffer) {
      DCHECK_EQ(_buffer->size(), other._buffer->size()); /// TO BE REMOVED!
      _buffer->host_mem_cpy(*other._buffer);
      if (!host_only) _buffer->device_mem_cpy(*other._buffer, device_name());
    }
  }

  void _set_dtype(DataType t) { _dtype = t; }
};
} // namespace chime

#endif // CHIME_CORE_FRAMEWORK_TENSOR_HPP_