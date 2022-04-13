// Copyright by 2022.4 chime
// author: yatorho

#ifndef CHIME_CORE_FRAMEWORK_BASETENSOR_HPP_
#define CHIME_CORE_FRAMEWORK_BASETENSOR_HPP_

#include "chime/core/framework/common.hpp"
#include "chime/core/framework/syncedmem.hpp"

#include <memory>
#include <sstream>
#include <vector>

const utens_t Max_Tensor_Axes = 32ul;

namespace chime {

template<typename Dtype>
class BaseTensor {
 public:
  BaseTensor();

  explicit BaseTensor(const std::vector<utens_t> &shape);

  BaseTensor(utens_t nums, utens_t channels, utens_t height, utens_t width);

  inline const std::vector<utens_t> &shape() const { return _shape; }

  void reshape(const std::vector<utens_t> &shape);

  void reshape(const std::vector<utens_t> &shape, BaseTensor<Dtype> &tensor);

  void reshape_like(const BaseTensor<Dtype> &other);

  bool shape_equals(const BaseTensor &other) const;

  inline utens_t shape(tens_t index) const {
    return _shape[canonical_axis_index(index)];
  }

  inline utens_t offset(utens_t nums, utens_t channels, utens_t height,
                        utens_t width) {
    return offset(std::vector<utens_t>({nums, channels, height, width}));
  }

  inline utens_t offset(const std::vector<utens_t> &indices) {
    DCHECK_LE(indices.size(), num_axes());
    utens_t offset = 0ul;
    for (utens_t i = 0; i < num_axes(); i++) {
      offset *= shape(i);
      if (indices.size() > i) {
        DCHECK_LT(indices[i], shape(i));
        offset += indices[i];
      }
    }
    return offset;
  }

  inline std::string shape_string() const {
    std::ostringstream stream;
    for (uint32_t i = 0; i < _shape.size(); i++) { stream << _shape[i] << " "; }
    stream << "(" << _count << ")";
    return stream.str();
  }

  inline utens_t num_axes() const { return _shape.size(); }

  inline utens_t count() const { return _count; }

  inline utens_t count(tens_t start_index, tens_t end_index) const {
    DCHECK_LE(canonical_axis_index(start_index),
              canonical_axis_index(end_index));
    utens_t count = 1ul;
    for (utens_t i = canonical_axis_index(start_index);
         i < canonical_axis_index(end_index); i++) {
      count *= shape(i);
    }
    return count;
  }

  inline utens_t count(tens_t start_index) {
    return count(start_index, num_axes());
  }

  inline utens_t capacity() const { return _capacity; }

  inline utens_t canonical_axis_index(tens_t axis_index) const {
    tens_t num_axes_t = static_cast<tens_t>(num_axes());
    DCHECK_GE(axis_index, -num_axes_t)
        << "axis " << axis_index << " out of range for " << num_axes_t
        << " -D BaseTensor with shape " << shape_string();
    DCHECK_LT(axis_index, num_axes_t)
        << "axis " << axis_index << " out of range for " << num_axes_t
        << " -D BaseTensor with shape " << shape_string();

    if (axis_index < 0) return static_cast<utens_t>(axis_index + num_axes_t);
    return static_cast<utens_t>(axis_index);
  }

  inline const std::shared_ptr<SyncedMemory> &memory() const {
    DCHECK(_memory);
    return _memory;
  }

  inline Dtype cpu_data(const std::vector<utens_t> &index) {
    DCHECK(_memory);
    return cpu_data()[offset(index)];
  }

  inline Dtype cpu_data(utens_t nums, utens_t channels, utens_t height,
                        utens_t width) {
    return cpu_data()[offset(nums, channels, height, width)];
  }

  inline utens_t num() const { return legacy_shape(0); }

  inline utens_t channels() const { return legacy_shape(1); }

  inline utens_t height() const { return legacy_shape(2); }

  inline utens_t width() const { return legacy_shape(3); }

  inline utens_t legacy_shape(tens_t index) const {
    DCHECK_LE(num_axes(), 4ul)
        << "Cannot use legacy accessor on BaseTensor with > 4 axes.";
    DCHECK_LT(index, 4);
    DCHECK_GE(index, -4);

    tens_t num_axes_t = static_cast<tens_t>(num_axes());
    if (index >= num_axes_t || index < -num_axes_t) { return 1ul; }
    return shape(index);
  }

  inline SyncedMemory::SyncedHead head() const {
    DCHECK(_memory);
    return _memory->head();
  }

  const Dtype *cpu_data() const;

  Dtype *mutable_cpu_data();

  void set_cpu_data(Dtype *data);

  Dtype asum_data() const;

  Dtype asumq_data() const;

  void scale_data(Dtype scale_factor);

  void share_data(const BaseTensor &other);

 private:
  std::shared_ptr<SyncedMemory> _memory;
  std::shared_ptr<SyncedMemory> _shape_mem;
  utens_t _count;
  utens_t _capacity;
  std::vector<utens_t> _shape;

  DISABLE_COPY_AND_ASSIGN(BaseTensor);
}; // class BaseTensor
} // namespace chime

#endif // CHIME_CORE_FRAMEWORK_BASETENSOR_HPP_
