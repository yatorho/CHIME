// Copyright by 2022.5 chime
// author: yatorho

#ifndef CHIME_CORE_FRAMEWORK_FIXED_TENSOR_HPP_
#define CHIME_CORE_FRAMEWORK_FIXED_TENSOR_HPP_

#include "chime/core/framework/common.hpp"

namespace chime {

template<typename T>
class fixed_vector {
 public:
 private:
}; // class fixed_vector

template<typename _Tp, size_t _S>
class fixed_array {
 public:
  using value_type = _Tp;
  using size_type = std::size_t;

  constexpr int size() const { return _S; }
  value_type &operator[](size_type index) { return _data[index]; }
  const value_type &operator[](size_type index) const { return _data[index]; }

  value_type *data() { return _data; }

  const value_type *data() const { return _data; }

 private:
  value_type _data[_S];
};

} // namespace chime

#endif // CHIME_CORE_FRAMEWORK_FIXED_TENSOR_HPP_