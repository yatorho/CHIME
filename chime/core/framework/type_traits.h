// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

#ifndef CHIME_CORE_FRAMEWORK_TYPE_TRAITS_H_
#define CHIME_CORE_FRAMEWORK_TYPE_TRAITS_H_

#include <type_traits>

#include "chime/core/framework/types.hpp"

namespace chime {

/// TODO(yatorho): API for quantized type to support.
/// Functions to define quantization attribute of types.
struct true_type {
  static constexpr bool value = true;
};

struct false_type {
  static constexpr bool value = false;
};

/// Default is_quantized is false.
template <typename T>
struct is_quantized : false_type {};

template <typename T>
struct is_complex : false_type {};

template <>
struct is_complex<std::complex<float>> : true_type {};

template <>
struct is_complex<std::complex<double>> : true_type {};

/// is_simple_type<T>::value if T[] can be safely constructed and destructed
/// without running T() and ~T(). We do not use std::is_trivial<T>
/// directly because std::complex<float> and std::complex<double> are
/// not trivial, but their arrays can be constructed and destructed
/// without running their default ctors and dtors.
template <typename T>
struct is_simple_type {
  static constexpr bool value = std::is_trivial<T>::value ||
                                std::is_same<T, complex64>::value ||
                                std::is_same<T, complex128>::value;
};

}  // namespace chime

#endif  // CHIME_CORE_FRAMEWORK_TYPE_TRAITS_H_
