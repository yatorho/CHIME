// Copyright 2022.5 chime
// author: yatorho

#ifndef CHIME_CORE_FRAMEWORK_TENSOR_H_
#define CHIME_CORE_FRAMEWORK_TENSOR_H_

#include <memory>

#include "chime/core/framework/common.hpp"
#include "chime/core/framework/shape_vec.hpp"
#include "chime/core/framework/syncedmem.hpp"
#include "chime/core/framework/tensor_shape.hpp"
#include "chime/core/framework/type.hpp"
#include "chime/core/memory/mem_optimizer.h"
#include "chime/core/schema/tensor.pb.h"

namespace chime {

class Tensor;

#define CHECK_DTYPE_AND_SHAPE(T, dtype)                                        \
  if (T != dtype)                                                              \
    LOG(WARNING) << "Called function with a return value of a different type " \
                    "than tensor may cause some memory size alignment issues"; \
  DCHECK(_shape.check_legality() && check_dtype())

class Tensor {
 public:
  using MemOp = memory::MemoryOptimizer;
  using DataPtr = std::shared_ptr<SyncedMemory>;
  using MemOpPtr = MemOp *;
  using DeviceName = DeviceSupported;

  typedef enum {
    HOST = 0,  // normally referring cpu.
    DEVICE = 1
  } OperateFrom;

 public:
  /// \brief Creates a 1-dimensional, 0-element tensor with invalid data type.
  ///
  /// The returned Tensor is not a scalar (shape{}), but is instead an empty
  /// one-dimensional Tensor (shape{0}, num_elements() == 0). Since it has no
  /// element, it doesn't need to be assigned a value.
  /// If this is undesirable, consider creating a 0-dimensional, one-element
  /// scalar tensor which does require initialization:
  ///
  /// ```c++
  ///
  ///    Tensor(DT_FLOAT32, TensorShape())
  ///
  ///    Tensor(DT_INT32)
  ///
  /// ```
  Tensor();

  /// \brief Creates a 0-dimensional, 1-element scalar Tensor.
  ///
  /// 
  explicit Tensor(DataType dtype);
  explicit Tensor(DataType dtype, const TensorShape &shape);
  explicit Tensor(MemOp &mem_op, DataType dtype, const TensorShape &shape);
  explicit Tensor(MemOp &mem_op, DataType dtype, const TensorShape &shape,
                  DeviceName d_name);

  Tensor(const Tensor &other);
  Tensor(Tensor &&other);
  ~Tensor();

  Tensor &operator=(const Tensor &other);  /// shallow copy
  Tensor &operator=(Tensor &&other);       /// shallow copy

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

  bool is_same_buffer(Tensor &other) const { return _buffer == other._buffer; }

  utens_t num_elements() const { return _shape.num_elements(); }

  inline SyncedMemory::SyncedHead head() const {
    DCHECK(_buffer);
    return _buffer->head();
  }

  utens_t ref_count() const;

  bool is_initialized() const;

  bool is_legal_shape() const;

  bool is_scalar() const;

  bool check_dtype() const;

  mems_t total_bytes() const;

  mems_t allocated_bytes() const;

  bool copy_from(const Tensor &other, const TensorShape &shape,
                 bool host_only = true) {
    if (other.num_elements() != shape.num_elements() ||
        other.dtype() != dtype())
      return false;
    _copy_from_internal(other, shape, host_only);
    return true;
  }

  template <DataType T>
  const typename EnumToDataType<T>::type *host_data() {
    return data<T>(HOST);
  }

  template <DataType T>
  typename EnumToDataType<T>::type *mutable_host_data() {
    return mutable_data<T>(HOST);
  }

  template <DataType T>
  void set_host_data(typename EnumToDataType<T>::type *data) {
    set_data<T>(data, HOST);
  }

  template <DataType T>
  const typename EnumToDataType<T>::type *device_data() {
    return data<T>(DEVICE);
  }

  template <DataType T>
  typename EnumToDataType<T>::type *mutable_device_data() {
    return mutable_data<T>(DEVICE);
  }

  template <DataType T>
  void set_device_data(typename EnumToDataType<T>::type *data) {
    set_data<T>(data, DEVICE);
  }

  template <DataType T>
  const typename EnumToDataType<T>::type *data(OperateFrom of) {
    CHECK_DTYPE_AND_SHAPE(T, _dtype);
    return of == HOST ? static_cast<const typename EnumToDataType<T>::type *>(
                            _buffer->host_mem())
                      : static_cast<const typename EnumToDataType<T>::type *>(
                            _buffer->device_mem(device_name()));
  }

  template <DataType T>
  typename EnumToDataType<T>::type *mutable_data(OperateFrom of) {
    CHECK_DTYPE_AND_SHAPE(T, _dtype);
    return static_cast<typename EnumToDataType<T>::type *>(buffer(of));
  }

  template <DataType T>
  void set_data(typename EnumToDataType<T>::type *data, OperateFrom of) {
    CHECK_DTYPE_AND_SHAPE(T, _dtype);
    if (of == HOST) {
      DCHECK(data);
      _buffer->set_host_mem(static_cast<void *>(data));
    } else {
      NOT_IMPLEMENTED;
    }
  }

  template <DataType T>
  typename EnumToDataType<T>::type at(const TensorShape &shape,
                                      OperateFrom of) {
    if (head() == SyncedMemory::UNINITIALIZED)
      LOG(WARNING)
          << "get value from a tensor whose memory was in uninitialized status";
    return data<T>(of)[_shape.offset(shape)];
  }

  template <DataType T>
  typename EnumToDataType<T>::type at(const DimVector &d_vector,
                                      OperateFrom of) {
    if (head() == SyncedMemory::UNINITIALIZED)
      LOG(WARNING)
          << "get value from a tensor whose memory was in uninitialized status";
    return data<T>(of)[_shape.offset(TensorShape(d_vector))];
  }

  template <DataType T>
  void set(const TensorShape &shape, typename EnumToDataType<T>::type value,
           OperateFrom of) {
    CHECK_DTYPE_AND_SHAPE(T, _dtype);
    mutable_data<T>(of)[_shape.offset(shape)] = value;
  }

  template <DataType T>
  void set(const DimVector &shape, typename EnumToDataType<T>::type value,
           OperateFrom of) {
    CHECK_DTYPE_AND_SHAPE(T, _dtype);
    mutable_data<T>(of)[_shape.offset(TensorShape(shape))] = value;
  }

  void *buffer(OperateFrom of);

  /// \brief Parse `other` and construct the tensor.
  /// Returns `true` if the parsing succeeds.
  bool from_proto(const TensorProto &proto);
  bool from_proto(MemOp &mem_op, const TensorProto &proto);

  /// \brief Fills in `Proto` with `*this` tensor's content.
  ///
  /// `as_proto_field()` fillss in the repeated field for `proto.dtype()`, while
  /// `as_proto_tensor_content()` encodes the content in
  /// `proto.tensor_content()` in a compact form.
  void as_proto_field(TensorProto *proto) const;
  void as_proto_tensor_content(TensorProto *proto) const;

 public:  // friend class and function
  friend class TensorTest;

 private:
  inline void _copy_from_internal(const Tensor &other, const TensorShape &shape,
                                  bool host_only) {
    DCHECK_EQ(shape.num_elements(), other.num_elements());
    DataType other_dtype = other.dtype();
    _set_dtype(other_dtype);
    _shape = shape;
    if (_buffer != other._buffer) {
      // DCHECK_EQ(_buffer->size(), other._buffer->size()); /// TO BE REMOVED!
      _buffer->host_mem_cpy(*other._buffer);
      if (!host_only)
        (_buffer->device_mem_cpy(*other._buffer, device_name()),
         _dname = other._dname);
    }
  }

  void _set_dtype(DataType t) { _dtype = t; }
};

#undef CHECK_DTYPE_AND_SHAPE

}  // namespace chime

#endif  // CHIME_CORE_FRAMEWORK_TENSOR_H_
