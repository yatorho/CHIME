// Copyright by 2022.4 chime
// author: yatorho

#ifndef CHIME_CORE_FRAMEWORK_COMMON_HPP_
#define CHIME_CORE_FRAMEWORK_COMMON_HPP_

#include <cstdint>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>

#include <cmath>

#ifndef GFLAGS_GFLAGS_H_
namespace gflags = google;
#endif // GFLAGS_GFLAGS_H_

typedef enum {
  OPEN_BLAS,
  EIGEN,
} BLAS_LIB;

#define USE_BLAS_LIB OPEN_BLAS

#define nan NAN

#define DISABLE_COPY_AND_ASSIGN(classname)                                     \
private:                                                                       \
  classname(const classname &);                                                \
  classname &operator=(const classname &)

#define INSTANTIATE_CLASS(classname)                                           \
  template class classname<float>;                                             \
  template class classname<float>

// memory size type
#define mems_t uint64_t


#ifdef CHIME_USE_61BIY_TENSOR_INT

#define OPENBLAS_USE64BITINT
#define utens_t uint64_t
#define UTENS_MAX UINT64_MAX

#define tenst_t int64_t
#define TENS_MAX INT64MAX
#else // CHIME_USE_61BIY_TENSOR_INT

#define utens_t uint32_t
#define UTENS_MAX UINT32_MAX

#define tens_t int32_t
#define TENS_MAX INT32_MAX

#endif // CHIME_USE_61BIY_TENSOR_INT

#define NOT_IMPLEMENTED LOG(FATAL) << "Not Implemented Yet";

namespace chime {

typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned short int uint16;
typedef signed short int int16;
typedef unsigned int uint32;
typedef signed int int32;
typedef unsigned long long int uint64;
typedef signed long long int int64;

typedef float float32;
typedef double float64;
typedef long double float128;

} // namespace chime

#endif // CHIME_CORE_FRAMEWORK_COMMON_HPP_
