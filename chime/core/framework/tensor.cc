// Copyright by 2022.5 chime
// author: yatorho

#include "chime/core/framework/tensor.h"

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
      _shape(shape) {
  _buffer.reset(
    new SyncedMemory(mem_op, _shape.num_elements() * dtype_size(dtype)));
}

Tensor::Tensor(DataType dtype, const TensorShape &shape)
    : _dtype(dtype),
      _shape(shape) {
  _buffer.reset(new SyncedMemory(memory::DefaultAllocator(),
                                 _shape.num_elements() * dtype_size(dtype)));
}

Tensor::Tensor(DataType dtype)
    : _dtype(dtype),
      _shape(std::move(TensorShape())) {
  _buffer.reset(new SyncedMemory(memory::DefaultAllocator(),
                                 _shape.num_elements() * dtype_size(dtype)));
}

Tensor::Tensor(const Tensor &other) { NOT_IMPLEMENTED; }

Tensor::Tensor(Tensor &&other) { NOT_IMPLEMENTED; }

Tensor &Tensor::operator=(const Tensor &other) { NOT_IMPLEMENTED; }

Tensor &Tensor::operator=(Tensor &&other) { NOT_IMPLEMENTED; }

Tensor::~Tensor() {}

} // namespace chime
