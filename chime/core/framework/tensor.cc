// Copyright by 2022.5 chime
// author: yatorho

#include "chime/core/framework/tensor.h"

#include "chime/core/framework/common.hpp"
#include "chime/core/framework/syncedmem.hpp"
#include "chime/core/framework/tensor_shape.hpp"
#include "chime/core/framework/type.hpp"
#include "chime/core/memory/mem_optimizer.h"
#include "chime/core/schema/tensor.pb.h"
#include "chime/core/schema/types.pb.h"

namespace chime {

template<typename T>
struct Helper {
  static_assert(IsValidDataType<T>::value, "T is not a simple type.");

  static Tensor::DataPtr decode(Tensor::MemOp &mem_op, const TensorProto &proto,
                                utens_t n) {
    Tensor::DataPtr buf;
    NOT_IMPLEMENTED;
    return buf;
  }
};

Tensor::Tensor(MemOp &mem_op, DataType dtype, const TensorShape &shape,
               DeviceName d_name)
    : _dtype(dtype),
      _shape(shape),
      _dname(d_name) {
  _buffer.reset(
    new SyncedMemory(mem_op, _shape.num_elements() * dtype_size(dtype)));
}

Tensor::Tensor(MemOp &mem_op, DataType dtype, const TensorShape &shape)
    : _dtype(dtype),
      _shape(shape),
      _dname(GRAPHICS_PROCESSING_UNIT) {
  _buffer.reset(
    new SyncedMemory(mem_op, _shape.num_elements() * dtype_size(dtype)));
}

Tensor::Tensor(DataType dtype, const TensorShape &shape)
    : _dtype(dtype),
      _shape(shape),
      _dname(GRAPHICS_PROCESSING_UNIT) {
  _buffer.reset(new SyncedMemory(memory::default_allocator,
                                 _shape.num_elements() * dtype_size(dtype)));
}

Tensor::Tensor(DataType dtype)
    : _dtype(dtype),
      _shape(std::move(TensorShape())),
      _dname(GRAPHICS_PROCESSING_UNIT) {
  _buffer.reset(new SyncedMemory(memory::default_allocator,
                                 _shape.num_elements() * dtype_size(dtype)));
}

Tensor::Tensor()
    : _dtype(DT_INVALID),
      _shape(std::move(TensorShape())),
      _dname(GRAPHICS_PROCESSING_UNIT) {
  _buffer.reset(new SyncedMemory(memory::default_allocator, 0ul));
}

Tensor::Tensor(const Tensor &other)
    : _dtype(other.dtype()),
      _shape(other.shape()),
      _dname(other.device_name()) {
  _buffer = other._buffer;
}

Tensor::Tensor(Tensor &&other)
    : _dtype(other.dtype()),
      _shape(std::move(other.shape())),
      _dname(other.device_name()) {
  _buffer = other._buffer;
}

Tensor &Tensor::operator=(const Tensor &other) {
  _dtype = other.dtype();
  _shape = other.shape();
  _dname = other.device_name();
  _buffer = other._buffer;
  return *this;
}

Tensor &Tensor::operator=(Tensor &&other) {
  _dtype = other.dtype();
  _shape = TensorShape(std::move(other.shape()));
  _dname = other.device_name();
  _buffer = other._buffer;
  return *this;
}

Tensor::~Tensor() {}

bool Tensor::operator==(const Tensor &other) { NOT_IMPLEMENTED; }

bool Tensor::operator==(Tensor &&other) { NOT_IMPLEMENTED; }

utens_t Tensor::operator[](utens_t d) const { NOT_IMPLEMENTED; }

bool Tensor::is_initialized() const {
  return head() != SyncedMemory::UNINITIALIZED;
}

mems_t Tensor::total_bytes() const {
  return _shape.num_elements() * dtype_size(_dtype);
}

mems_t Tensor::allocated_bytes() const {
  switch (head()) {
    case SyncedMemory::UNINITIALIZED: return 0ull;
    case SyncedMemory::HEAD_AT_HOST:
      return _buffer->own_host_mem() ? _buffer->size() : 0ull;
    case SyncedMemory::HEAD_AT_DEVICE:
      return _buffer->own_device_mem() ? _buffer->size() : 0ull;
    case SyncedMemory::SYNCED: return 2ull * _buffer->size();
    default: LOG(FATAL) << "Unknown SyncedHead status!";
  }
}

void *Tensor::buffer(OperateFrom of) {
  return of == HOST ? _buffer->mutable_host_mem()
                    : _buffer->mutable_device_mem(device_name());
}

bool Tensor::is_legal_shape() const { return _shape.check_legality(); }

bool Tensor::is_scalar() const { return _shape.dims() == 0 && check_dtype(); }

bool Tensor::check_dtype() const { return _dtype != DT_INVALID; }

utens_t Tensor::ref_count() const { return _buffer.use_count(); }

#define SINGLE_ARG(...) __VA_ARGS__
#define CASE(TYPE, STMTS)             \
  case DataTypeToEnum<TYPE>::value: { \
    typedef TYPE T;                   \
    STMTS;                            \
    break;                            \
  }
#define CASES_WITH_DEFAULT(TYPE_ENUM, STMTS, INVALID, DEFAULT) \
  switch (TYPE_ENUM) {                                         \
    CASE(float32, SINGLE_ARG(STMTS))                           \
    CASE(float64, SINGLE_ARG(STMTS))                           \
    CASE(int32, SINGLE_ARG(STMTS))                             \
    CASE(uint8, SINGLE_ARG(STMTS))                             \
    CASE(uint16, SINGLE_ARG(STMTS))                            \
    CASE(uint32, SINGLE_ARG(STMTS))                            \
    CASE(uint64, SINGLE_ARG(STMTS))                            \
    CASE(int16, SINGLE_ARG(STMTS))                             \
    CASE(int8, SINGLE_ARG(STMTS))                              \
    CASE(string, SINGLE_ARG(STMTS))                            \
    CASE(complex64, SINGLE_ARG(STMTS))                         \
    CASE(complex128, SINGLE_ARG(STMTS))                        \
    CASE(int64, SINGLE_ARG(STMTS))                             \
    CASE(bool, SINGLE_ARG(STMTS))                              \
    case DT_INVALID: INVALID; break;                           \
    default: DEFAULT; break;                                   \
  }

bool Tensor::from_proto(const TensorProto &proto) {
  if (!TensorShape::is_valid(proto.tensor_shape())) return false;
  if (proto.dtype() == DT_INVALID) return false;

  TensorShape shape;
  shape.from_proto(proto.tensor_shape());
  const utens_t N = shape.num_elements();

  DataPtr buffer;

  if (N > 0 && proto.dtype()) {
    bool dtype_error = false;
    CASES_WITH_DEFAULT(
      proto.dtype(),
      buffer = Helper<T>::decode(memory::default_allocator, proto, N),
      dtype_error = true, dtype_error = true);
    if (dtype_error || buffer) return false;
  } else {
    bool dtype_error = false;
    CASES_WITH_DEFAULT(proto.dtype(), break, dtype_error = true,
                       dtype_error = true);
    if (dtype_error) return false;
    return false;
  }

  _shape = shape;
  _set_dtype(proto.dtype());
  _buffer = buffer;
  return true;
}

}  // namespace chime
