// Copyright by 2022.4 chime
// author: yatorho

#include "basetensor.hpp"
#include "common.hpp"
#include "syncedmem.hpp"

namespace chime {

template <typename Dtype> BaseTensor<Dtype>::BaseTensor() : _capacity(0ul) {}

template <typename Dtype>
BaseTensor<Dtype>::BaseTensor(const std::vector<utens_t> &shape) : _capacity(0ul) {
  reshape(shape);
}

template <typename Dtype>
BaseTensor<Dtype>::BaseTensor(utens_t nums, utens_t channels, utens_t height,
                      utens_t width)
    : _capacity(0ul) {
  reshape(std::vector<utens_t>({nums, channels, height, width}));
}

template <typename Dtype> const Dtype *BaseTensor<Dtype>::cpu_data() {
  DCHECK(_memory);
  return const_cast<const Dtype *>(_memory->cpu_mem());
}

template <typename Dtype>
void BaseTensor<Dtype>::reshape_like(const BaseTensor<Dtype> &other) {
  reshape(other.shape());
}

template <typename Dtype>
void BaseTensor<Dtype>::reshape(const std::vector<utens_t> &shape) {
  DCHECK_LE(shape.size(), Max_Tensor_Axes);

  _count = 1ul;
  _shape.resize(shape.size());
  if (!_shape_mem || _shape_mem->size() < shape.size() * sizeof(utens_t)) {
    _shape_mem.reset(
        new SyncedMemory(static_cast<mems_t>(shape.size() * sizeof(utens_t))));
  }
  auto shape_data = static_cast<utens_t *>(_shape_mem->mutable_cpu_mem());
  for (utens_t i = 0; i < shape.size(); i++) {
    if (_count > 0ul) {
      DCHECK_LE(shape[i], UTENS_MAX / _count)
          << "Tensor size exceeds UTENS_MAX " << UTENS_MAX;
    }
    _count *= shape[i];
    _shape[i] = shape[i];
    shape_data[i] = shape[i];
  }

  if (_count > _capacity) {
    _capacity = _count;
    _memory.reset(
        new SyncedMemory(static_cast<mems_t>(_count * sizeof(Dtype))));
  }
}

} // namespace chime
