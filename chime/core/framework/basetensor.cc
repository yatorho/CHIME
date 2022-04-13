// Copyright by 2022.4 chime
// author: yatorho

#include "chime/core/framework/basetensor.hpp"

#include "chime/core/framework/math_functions.hpp"

namespace chime {

template<typename Dtype>
BaseTensor<Dtype>::BaseTensor() : _capacity(0ul) {}

template BaseTensor<int8>::BaseTensor();

template BaseTensor<int16>::BaseTensor();

template BaseTensor<int32>::BaseTensor();

template BaseTensor<int64>::BaseTensor();

template BaseTensor<uint8>::BaseTensor();

template BaseTensor<uint16>::BaseTensor();

template BaseTensor<uint32>::BaseTensor();

template BaseTensor<uint64>::BaseTensor();

template BaseTensor<float32>::BaseTensor();

template BaseTensor<float64>::BaseTensor();

template BaseTensor<float128>::BaseTensor();

template<typename Dtype>
BaseTensor<Dtype>::BaseTensor(const std::vector<utens_t> &shape)
    : _capacity(0ul) {
  reshape(shape);
}

template BaseTensor<int8>::BaseTensor(const std::vector<utens_t> &shape);

template BaseTensor<int16>::BaseTensor(const std::vector<utens_t> &shape);

template BaseTensor<int32>::BaseTensor(const std::vector<utens_t> &shape);

template BaseTensor<int64>::BaseTensor(const std::vector<utens_t> &shape);

template BaseTensor<uint8>::BaseTensor(const std::vector<utens_t> &shape);

template BaseTensor<uint16>::BaseTensor(const std::vector<utens_t> &shape);

template BaseTensor<uint32>::BaseTensor(const std::vector<utens_t> &shape);

template BaseTensor<uint64>::BaseTensor(const std::vector<utens_t> &shape);

template BaseTensor<float32>::BaseTensor(const std::vector<utens_t> &shape);

template BaseTensor<float64>::BaseTensor(const std::vector<utens_t> &shape);

template BaseTensor<float128>::BaseTensor(const std::vector<utens_t> &shape);

template<typename Dtype>
BaseTensor<Dtype>::BaseTensor(utens_t nums, utens_t channels, utens_t height,
                              utens_t width)
    : _capacity(0ul) {
  reshape(std::vector<utens_t>({nums, channels, height, width}));
}

template BaseTensor<int8>::BaseTensor(utens_t nums, utens_t channels,
                                      utens_t height, utens_t width);

template BaseTensor<int16>::BaseTensor(utens_t nums, utens_t channels,
                                       utens_t height, utens_t width);

template BaseTensor<int32>::BaseTensor(utens_t nums, utens_t channels,
                                       utens_t height, utens_t width);

template BaseTensor<int64>::BaseTensor(utens_t nums, utens_t channels,
                                       utens_t height, utens_t width);

template BaseTensor<uint8>::BaseTensor(utens_t nums, utens_t channels,
                                       utens_t height, utens_t width);

template BaseTensor<uint16>::BaseTensor(utens_t nums, utens_t channels,
                                        utens_t height, utens_t width);

template BaseTensor<uint32>::BaseTensor(utens_t nums, utens_t channels,
                                        utens_t height, utens_t width);

template BaseTensor<uint64>::BaseTensor(utens_t nums, utens_t channels,
                                        utens_t height, utens_t width);

template BaseTensor<float32>::BaseTensor(utens_t nums, utens_t channels,
                                         utens_t height, utens_t width);

template BaseTensor<float64>::BaseTensor(utens_t nums, utens_t channels,
                                         utens_t height, utens_t width);

template BaseTensor<float128>::BaseTensor(utens_t nums, utens_t channels,
                                          utens_t height, utens_t width);

template<typename Dtype>
const Dtype *BaseTensor<Dtype>::cpu_data() const {
  DCHECK(_memory);
  return static_cast<const Dtype *>(_memory->cpu_mem());
}

template const int8 *BaseTensor<int8>::cpu_data() const;

template const int16 *BaseTensor<int16>::cpu_data() const;

template const int32 *BaseTensor<int32>::cpu_data() const;

template const int64 *BaseTensor<int64>::cpu_data() const;

template const uint8 *BaseTensor<uint8>::cpu_data() const;

template const uint16 *BaseTensor<uint16>::cpu_data() const;

template const uint32 *BaseTensor<uint32>::cpu_data() const;

template const uint64 *BaseTensor<uint64>::cpu_data() const;

template const float32 *BaseTensor<float32>::cpu_data() const;

template const float64 *BaseTensor<float64>::cpu_data() const;

template const float128 *BaseTensor<float128>::cpu_data() const;

template<typename Dtype>
void BaseTensor<Dtype>::reshape_like(const BaseTensor<Dtype> &other) {
  reshape(other.shape());
}

template void BaseTensor<int8>::reshape_like(const BaseTensor<int8> &other);

template void BaseTensor<int16>::reshape_like(const BaseTensor<int16> &other);

template void BaseTensor<int32>::reshape_like(const BaseTensor<int32> &other);

template void BaseTensor<int64>::reshape_like(const BaseTensor<int64> &other);

template void BaseTensor<uint8>::reshape_like(const BaseTensor<uint8> &other);

template void BaseTensor<uint16>::reshape_like(const BaseTensor<uint16> &other);

template void BaseTensor<uint32>::reshape_like(const BaseTensor<uint32> &other);

template void BaseTensor<uint64>::reshape_like(const BaseTensor<uint64> &other);

template void
BaseTensor<float32>::reshape_like(const BaseTensor<float32> &other);

template void
BaseTensor<float64>::reshape_like(const BaseTensor<float64> &other);

template void
BaseTensor<float128>::reshape_like(const BaseTensor<float128> &other);

template<typename Dtype>
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

template void BaseTensor<int8>::reshape(const std::vector<utens_t> &shape);

template void BaseTensor<int16>::reshape(const std::vector<utens_t> &shape);

template void BaseTensor<int32>::reshape(const std::vector<utens_t> &shape);

template void BaseTensor<int64>::reshape(const std::vector<utens_t> &shape);

template void BaseTensor<uint8>::reshape(const std::vector<utens_t> &shape);

template void BaseTensor<uint16>::reshape(const std::vector<utens_t> &shape);

template void BaseTensor<uint32>::reshape(const std::vector<utens_t> &shape);

template void BaseTensor<uint64>::reshape(const std::vector<utens_t> &shape);

template void BaseTensor<float32>::reshape(const std::vector<utens_t> &shape);

template void BaseTensor<float64>::reshape(const std::vector<utens_t> &shape);

template void BaseTensor<float128>::reshape(const std::vector<utens_t> &shape);

template<typename Dtype>
Dtype BaseTensor<Dtype>::asum_data() const {
  if (!_memory) { return 0; }
  switch (_memory->head()) {
    case SyncedMemory::HEAD_AT_CPU: return chime_cpu_asum(_count, cpu_data());
    case SyncedMemory::SYNCED: break;
    case SyncedMemory::UNINITIALIZED: return static_cast<Dtype>(0);
    default:
      LOG(FATAL) << "Unknown SyncedMemory head state: " << _memory->head();
  }
  return 0;
}

template float32 BaseTensor<float32>::asum_data() const;

template float64 BaseTensor<float64>::asum_data() const;

template float128 BaseTensor<float128>::asum_data() const;

template<typename Dtype>
Dtype BaseTensor<Dtype>::asumq_data() const {
  Dtype sumsq;
  const Dtype *data;
  if (!_memory) { return 0; }
  switch (_memory->head()) {
    case SyncedMemory::HEAD_AT_CPU:
      data = cpu_data();
      sumsq = chime_cpu_dot(_count, data, data);
      break;
    case SyncedMemory::SYNCED: break;
    case SyncedMemory::UNINITIALIZED: return 0;
    default:
      LOG(FATAL) << "Unknown SyncedMemory head state: " << _memory->head();
  }
  return sumsq;
}

template float32 BaseTensor<float32>::asumq_data() const;

template float64 BaseTensor<float64>::asumq_data() const;

template float128 BaseTensor<float128>::asumq_data() const;

template<typename Dtype>
void BaseTensor<Dtype>::scale_data(Dtype scale_factor) {
  chime_cpu_scal(_count, scale_factor,
                 static_cast<Dtype *>(_memory->mutable_cpu_mem()));
}

template void BaseTensor<float32>::scale_data(float32 scale_factor);

template void BaseTensor<float64>::scale_data(float64 scale_factor);

template void BaseTensor<float128>::scale_data(float128 scale_factor);

template<typename Dtype>
void BaseTensor<Dtype>::set_cpu_data(Dtype *data) {
  DCHECK(_memory);
  utens_t size = _count * sizeof(Dtype);
  if (_memory->size() != size) { _memory.reset(new SyncedMemory(size)); }
  _memory->set_cpu_mem(data);
}

template void BaseTensor<float32>::set_cpu_data(float32 *data);

template void BaseTensor<float64>::set_cpu_data(float64 *data);

template void BaseTensor<float128>::set_cpu_data(float128 *data);

template<typename Dtype>
Dtype *BaseTensor<Dtype>::mutable_cpu_data() {
  DCHECK(_memory);
  return static_cast<Dtype *>(_memory->mutable_cpu_mem());
}

template float32 *BaseTensor<float32>::mutable_cpu_data();

template float64 *BaseTensor<float64>::mutable_cpu_data();

template float128 *BaseTensor<float128>::mutable_cpu_data();

template<typename Dtype>
void BaseTensor<Dtype>::share_data(const BaseTensor<Dtype> &other) {
  DCHECK_EQ(_count, other.count());
  _memory = other._memory;
}

} // namespace chime
