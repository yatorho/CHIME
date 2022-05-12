// Copyright by 2022.5 chime
// author: yatorho

#include "chime/core/framework/tensor.h"

#include "chime/core/framework/common.hpp"
#include "chime/core/framework/syncedmem.hpp"
#include "chime/core/framework/tensor_shape.hpp"
#include "chime/core/framework/type.hpp"
#include "chime/core/memory/mem_optimizer.h"

namespace chime {

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

Tensor::Tensor(const Tensor &other) { NOT_IMPLEMENTED; }

Tensor::Tensor(Tensor &&other) { NOT_IMPLEMENTED; }

Tensor &Tensor::operator=(const Tensor &other) { NOT_IMPLEMENTED; }

Tensor &Tensor::operator=(Tensor &&other) { NOT_IMPLEMENTED; }

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

} // namespace chime
