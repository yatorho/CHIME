// Copyright by 2022.4 chime
// author: yatorho

#ifndef CHIME_CORE_FRAMEWORK_COMMON_HPP_
#define CHIME_CORE_FRAMEWORK_COMMON_HPP_

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>

#include <cmath>
#include <complex>
#include <cstdint>

#ifndef GFLAGS_GFLAGS_H_
namespace gflags = google;
#endif  // GFLAGS_GFLAGS_H_

#define USE_BLAS_LIB OPEN_BLAS

#define MemoryOptimizationOption CHIME_MEMORY_POOL

#define nan NAN

#define DISABLE_COPY_AND_ASSIGN(classname) \
 private:                                  \
  classname(const classname &);            \
  classname &operator=(const classname &)

#define INSTANTIATE_CLASS(classname) \
  template class classname<float32>; \
  template class classname<float64>

#ifndef CHIME_USE_32BIT_MEMORY_INT
#define mems_t uint64_t
#else
#define mems_t uint32_t
#endif

typedef mems_t *mems_ptr;

#define CHIME_USE_64BIT_TENSOR_INT

#ifdef CHIME_USE_64BIT_TENSOR_INT

#define OPENBLAS_USE64BITINT
typedef uint64_t utens_t;
#define UTENS_MAX UINT64_MAX

typedef int64_t tens_t;
#define TENS_MAX INT64MAX
#else  // CHIME_USE_64BIT_TENSOR_INT

typedef uint32_t utens_t;
#define UTENS_MAX UINT32_MAX

typedef int32_t tens_t;
#define TENS_MAX INT32_MAX

#endif  // CHIME_USE_64BIT_TENSOR_INT

typedef utens_t *utens_ptr;
typedef tens_t *tens_ptr;

#define NOT_IMPLEMENTED LOG(FATAL) << "Not Implemented Yet"

namespace chime {

typedef enum {
  OPEN_BLAS,
  EIGEN,
} BlasLib;

typedef enum {
  GRAPHICS_PROCESSING_UNIT,      // referring GPU
  TENSOR_PROCESSING_UNIT,        // referring TPU
  FIELD_PROGRAMMABLE_GATE_ARRAY  // referring FPGA
} DeviceSupported;

typedef enum {
  NO_OPTIMIZATION,
  CHIME_MEMORY_POOL,
  CHIME_ALLOCATOR
} MemoryOptimization;

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

typedef std::complex<float> complex64;
typedef std::complex<double> complex128;

using std::string;

typedef void *void_ptr;

typedef uint8 *uint8_ptr;
typedef int8 *int8_ptr;
typedef uint16 *uint16_ptr;
typedef int16 *int16_ptr;
typedef uint32 *uint32_ptr;
typedef int32 *int32_tpr;
typedef uint64 *uint64_ptr;
typedef int64 *int64_ptr;

typedef float32 *float32_ptr;
typedef float64 *float64_ptr;
typedef float128 *float128_ptr;

typedef std::string string;

static const uint8 uint8max = static_cast<uint8>(0xFF);
static const uint16 uint16max = static_cast<uint16>(0xFFFF);
static const uint32 uint32max = static_cast<uint32>(0xFFFFFFFF);
static const uint64 uint64max = static_cast<uint64>(0xFFFFFFFFFFFFFFFFull);
static const int8_t int8min = static_cast<int8>(~0x7F);
static const int8_t int8max = static_cast<int8>(0x7F);
static const int16_t int16min = static_cast<int16>(~0x7FFF);
static const int16_t int16max = static_cast<int16>(0x7FFF);
static const int32_t int32min = static_cast<int32>(~0x7FFFFFFF);
static const int32_t int32max = static_cast<int32>(0x7FFFFFFF);
static const int64_t int64min = static_cast<int64_t>(~0x7FFFFFFFFFFFFFFFll);
static const int64_t int64max = static_cast<int64_t>(0x7FFFFFFFFFFFFFFFll);

}  // namespace chime

#endif  // CHIME_CORE_FRAMEWORK_COMMON_HPP_
