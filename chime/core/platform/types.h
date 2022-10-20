// Copyright by 2022.9 chime. All rights reserved.

#ifndef CHIME_CORE_PLATFORM_TYPES_H_
#define CHIME_CORE_PLATFORM_TYPES_H_

#include <complex>
#include <cstdint>
#include <half.hpp>

#include "chime/core/platform/types.pb.h"

namespace chime {

typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;
typedef float float32;
typedef double float64;
typedef long double float128;

typedef std::complex<float> complex64;
typedef std::complex<double> complex128;
typedef std::string string;

#ifdef __GNUC__
typedef __int128_t int128;
typedef __uint128_t uint128;
#endif

typedef half_float::half float16;
typedef half_float::half half;
typedef std::complex<uint16> complex32;

static constexpr size_t DT_BOOL_SIZE = sizeof(bool);
static constexpr size_t DT_INT8_SIZE = sizeof(int8);
static constexpr size_t DT_UINT8_SIZE = sizeof(uint8);
static constexpr size_t DT_INT16_SIZE = sizeof(int16);
static constexpr size_t DT_UINT16_SIZE = sizeof(uint16);
static constexpr size_t DT_INT32_SIZE = sizeof(int32);
static constexpr size_t DT_UINT32_SIZE = sizeof(uint32);
static constexpr size_t DT_INT64_SIZE = sizeof(int64);
static constexpr size_t DT_UINT64_SIZE = sizeof(uint64);
static constexpr size_t DT_HALF_SIZE = sizeof(half);
static constexpr size_t DT_FLOAT16_SIZE = sizeof(float16);
static constexpr size_t DT_FLOAT32_SIZE = sizeof(float32);
static constexpr size_t DT_FLOAT64_SIZE = sizeof(float64);
static constexpr size_t DT_FLOAT128_SIZE = sizeof(float128);
static constexpr size_t DT_COMPLEX32_SIZE = sizeof(complex32);
static constexpr size_t DT_COMPLEX64_SIZE = sizeof(complex64);
static constexpr size_t DT_COMPLEX128_SIZE = sizeof(complex128);
static constexpr size_t DT_CHAR_SIZE = sizeof(char);

static constexpr size_t DT_BFLOAT16_SIZE = DT_FLOAT16_SIZE;

#ifdef __GNUC__
static constexpr size_t DT_INT128_SIZE = sizeof(int128);
static constexpr size_t DT_UINT128_SIZE = sizeof(uint128);
#endif

template <DataType T>
struct IsValidDataTypeEnum {
  static constexpr bool value = false;
};

template <class T>
struct IsValidDataType {
  static constexpr bool value = false;
};

template <class T>
struct DataTypeToEnum {
  static_assert(IsValidDataType<T>::value, "Invalid data type");
};

template <DataType T>
struct EnumToDataType {
  static_assert(IsValidDataTypeEnum<T>::value, "Invalid data type enum");
};

template <DataType T>
struct EnumHasSize {
  static_assert(IsValidDataTypeEnum<T>::value, "Invalid data type enum");
  static constexpr bool value = false;
};

template <DataType T>
struct DataTypeSize {
  static_assert(EnumHasSize<T>::value, "Data type enum has not size attribute");
};

#define MATCH_TYPE_AND_ENUM(TYPE, ENUM)         \
  template <>                                   \
  struct IsValidDataType<TYPE> {                \
    static constexpr bool value = true;         \
  };                                            \
  template <>                                   \
  struct IsValidDataTypeEnum<ENUM> {            \
    static constexpr bool value = true;         \
  };                                            \
  template <>                                   \
  struct DataTypeToEnum<TYPE> {                 \
    static constexpr DataType value = ENUM;     \
    static DataType v() { return value; }       \
  };                                            \
  template <>                                   \
  struct EnumToDataType<ENUM> {                 \
    typedef TYPE type;                          \
  };                                            \
  template <>                                   \
  struct EnumHasSize<ENUM> {                    \
    static constexpr bool value = true;         \
  };                                            \
  template <>                                   \
  struct DataTypeSize<ENUM> {                   \
    static constexpr size_t size = ENUM##_SIZE; \
  }

MATCH_TYPE_AND_ENUM(bool, DT_BOOL);
MATCH_TYPE_AND_ENUM(int8, DT_INT8);
MATCH_TYPE_AND_ENUM(uint8, DT_UINT8);
MATCH_TYPE_AND_ENUM(int16, DT_INT16);
MATCH_TYPE_AND_ENUM(uint16, DT_UINT16);
MATCH_TYPE_AND_ENUM(int32, DT_INT32);
MATCH_TYPE_AND_ENUM(uint32, DT_UINT32);
MATCH_TYPE_AND_ENUM(int64, DT_INT64);
MATCH_TYPE_AND_ENUM(uint64, DT_UINT64);
MATCH_TYPE_AND_ENUM(half, DT_HALF);
MATCH_TYPE_AND_ENUM(float32, DT_FLOAT32);
MATCH_TYPE_AND_ENUM(float64, DT_FLOAT64);
MATCH_TYPE_AND_ENUM(float128, DT_FLOAT128);
MATCH_TYPE_AND_ENUM(complex32, DT_COMPLEX32);
MATCH_TYPE_AND_ENUM(complex64, DT_COMPLEX64);
MATCH_TYPE_AND_ENUM(complex128, DT_COMPLEX128);
MATCH_TYPE_AND_ENUM(char, DT_CHAR);

#ifdef __GNUC__
MATCH_TYPE_AND_ENUM(int128, DT_INT128);
MATCH_TYPE_AND_ENUM(uint128, DT_UINT128);
#endif

template <>
struct IsValidDataTypeEnum<DT_FLOAT16> {
  static constexpr bool value = true;
};

template <>
struct IsValidDataTypeEnum<DT_BFLOAT16> {
  static constexpr bool value = true;
};

template <>
struct IsValidDataTypeEnum<DT_STRING> {
  static constexpr bool value = true;
};

template <>
struct DataTypeToEnum<std::string> {
  static constexpr DataType value = DT_STRING;
};

template <>
struct EnumToDataType<DT_STRING> {
  typedef std::string type;
};

template <>
struct EnumToDataType<DT_FLOAT16> {
  typedef float16 type;
};

template <>
struct EnumHasSize<DT_FLOAT16> {
  static constexpr bool value = true;
};

template <>
struct EnumHasSize<DT_BFLOAT16> {
  static constexpr bool value = true;
};

template <>
struct DataTypeSize<DT_FLOAT16> {
  static constexpr size_t size = DT_FLOAT16_SIZE;
};

template <>
struct DataTypeSize<DT_BFLOAT16> {
  static constexpr size_t size = DT_BFLOAT16_SIZE;
};

#undef MATCH_TYPE_AND_ENUM

#define CASE_AND_RETURN(CASE, RETURN_VALUE) \
  case CASE: {                              \
    return RETURN_VALUE;                    \
  }

/// Also provides a way to get the size of a data type enum in the runtime.
static inline size_t GetDataTypeSize(DataType dt) {
  switch (dt) {
    CASE_AND_RETURN(DT_BOOL, DT_BOOL_SIZE);
    CASE_AND_RETURN(DT_INT8, DT_INT8_SIZE);
    CASE_AND_RETURN(DT_UINT8, DT_UINT8_SIZE);
    CASE_AND_RETURN(DT_INT16, DT_INT16_SIZE);
    CASE_AND_RETURN(DT_UINT16, DT_UINT16_SIZE);
    CASE_AND_RETURN(DT_INT32, DT_INT32_SIZE);
    CASE_AND_RETURN(DT_UINT32, DT_UINT32_SIZE);
    CASE_AND_RETURN(DT_INT64, DT_INT64_SIZE);
    CASE_AND_RETURN(DT_UINT64, DT_UINT64_SIZE);
    CASE_AND_RETURN(DT_HALF, DT_HALF_SIZE);
    CASE_AND_RETURN(DT_FLOAT32, DT_FLOAT32_SIZE);
    CASE_AND_RETURN(DT_FLOAT64, DT_FLOAT64_SIZE);
    CASE_AND_RETURN(DT_FLOAT128, DT_FLOAT128_SIZE);
    CASE_AND_RETURN(DT_COMPLEX32, DT_COMPLEX32_SIZE);
    CASE_AND_RETURN(DT_COMPLEX64, DT_COMPLEX64_SIZE);
    CASE_AND_RETURN(DT_COMPLEX128, DT_COMPLEX128_SIZE);
    CASE_AND_RETURN(DT_BFLOAT16, DT_BFLOAT16_SIZE);
    CASE_AND_RETURN(DT_FLOAT16, DT_FLOAT16_SIZE);
    CASE_AND_RETURN(DT_CHAR, DT_CHAR_SIZE);
#ifdef __GNUC__
    CASE_AND_RETURN(DT_INT128, DT_INT128_SIZE);
    CASE_AND_RETURN(DT_UINT128, DT_UINT128_SIZE);
#endif
    default:
      return 0;
  }
}

#undef CASE_AND_RETURN

static constexpr uint8 uint8_max = static_cast<uint8>(0xFF);
static constexpr uint16 uint16_max = static_cast<uint16>(0xFFFF);
static constexpr uint32 uint32_max = static_cast<uint32>(0xFFFFFFFF);
static constexpr uint64 uint64_max = static_cast<uint64>(0xFFFFFFFFFFFFFFFF);
static constexpr int8 int8_min = static_cast<int8>(~0x7F);
static constexpr int8 int8_max = static_cast<int8>(0x7F);
static constexpr int16 int16_min = static_cast<int16>(~0x7FFF);
static constexpr int16 int16_max = static_cast<int16>(0x7FFF);
static constexpr int32 int32_min = static_cast<int32>(~0x7FFFFFFF);
static constexpr int32 int32_max = static_cast<int32>(0x7FFFFFFF);
static constexpr int64 int64_min = static_cast<int64>(~0x7FFFFFFFFFFFFFFF);
static constexpr int64 int64_max = static_cast<int64>(0x7FFFFFFFFFFFFFFF);

#ifdef __GNUC__
static constexpr uint128 uint128_max =
    static_cast<uint128>(static_cast<int128>(-1L));
static constexpr int128 int128_max = uint128_max >> 1;
static constexpr int128 int128_min = -int128_max - 1;
#endif

}  // namespace chime

#endif  // CHIME_CORE_PLATFORM_TYPES_H_
