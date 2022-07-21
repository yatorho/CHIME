// Copyright 2022.4 chime
// author: yatorho

#ifndef CHIME_CORE_FRAMEWORK_TYPES_HPP_
#define CHIME_CORE_FRAMEWORK_TYPES_HPP_

#include <complex>

#include "chime/core/platform/logging.hpp"
#include "chime/core/schema/types.pb.h"

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

typedef uint64_t mems_t;
typedef int64_t tens_t;
typedef uint64_t utens_t;

static const size_t DT_BOOL_SIZE = sizeof(bool);
static const size_t DT_INT8_SIZE = sizeof(int8);
static const size_t DT_UINT8_SIZE = sizeof(uint8);
static const size_t DT_INT16_SIZE = sizeof(int16);
static const size_t DT_UINT16_SIZE = sizeof(uint16);
static const size_t DT_INT32_SIZE = sizeof(int32);
static const size_t DT_UINT32_SIZE = sizeof(uint32);
static const size_t DT_INT64_SIZE = sizeof(int64);
static const size_t DT_UINT64_SIZE = sizeof(uint64);
static const size_t DT_HALF_SIZE = static_cast<size_t>(2ull);
static const size_t DT_FLOAT32_SIZE = sizeof(float32);
static const size_t DT_FLOAT64_SIZE = sizeof(float64);
static const size_t DT_FLOAT128_SIZE = sizeof(float128);
static const size_t DT_COMPLEX64_SIZE = sizeof(complex64);
static const size_t DT_COMPLEX128_SIZE = sizeof(complex128);

#define DT_FLOAT DT_FLOAT32
#define DT_DOUBLE DT_FLOAT64

template <class T>
struct IsValidDataType;

template <class T>
struct DataTypeToEnum {
  static_assert(IsValidDataType<T>::value, "Specified Data Type not supported");
};

template <DataType T>
struct EnumToDataType {};

template <DataType T>
struct EnumHasSize;

template <DataType T>
struct DtypeSize {
  static_assert(EnumHasSize<T>::value,
                "Specified Data Type Enum not supported");
};

#define MATCH_TYPE_AND_ENUM(TYPE, ENUM)         \
  template <>                                   \
  struct DataTypeToEnum<TYPE> {                 \
    static DataType v() { return ENUM; }        \
    static constexpr DataType value = ENUM;     \
  };                                            \
  template <>                                   \
  struct IsValidDataType<TYPE> {                \
    static constexpr bool value = true;         \
  };                                            \
  template <>                                   \
  struct EnumToDataType<ENUM> {                 \
    typedef TYPE type;                          \
  };                                            \
  template <>                                   \
  struct EnumHasSize<ENUM> {            \
    static constexpr bool value = true;         \
  };                                            \
  template <>                                   \
  struct DtypeSize<ENUM> {                   \
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
MATCH_TYPE_AND_ENUM(float32, DT_FLOAT32);
MATCH_TYPE_AND_ENUM(float64, DT_FLOAT64);
MATCH_TYPE_AND_ENUM(float128, DT_FLOAT128);
MATCH_TYPE_AND_ENUM(complex64, DT_COMPLEX64);
MATCH_TYPE_AND_ENUM(complex128, DT_COMPLEX128);

template <>
struct DataTypeToEnum<string> {
  static DataType v() { return DT_STRING; }
  static constexpr DataType value = DT_STRING;
};

template <>
struct IsValidDataType<string> {
  static constexpr bool value = true;
};

template <>
struct EnumToDataType<DT_STRING> {
  typedef string type;
};

template <>
struct EnumHasSize<DT_STRING> {
  static constexpr bool value = false;
};

template <>
struct EnumHasSize<DT_HALF> {
  static constexpr bool value = true;
};

template <>
struct DtypeSize<DT_HALF> {
  static constexpr size_t size = DT_HALF_SIZE;
};

template <class T>
struct IsValidDataType {
  static constexpr bool value = false;
};

#undef MATCH_TYPE_AND_ENUM

/// Get datatype size in the runtime.
inline size_t GetDtypeSize(DataType dtype) {
  switch (dtype) {
    case DT_BOOL:
      return sizeof(bool);
    case DT_INT8:
      return sizeof(int8);
    case DT_UINT8:
      return sizeof(uint8);
    case DT_INT16:
      return sizeof(int16);
    case DT_UINT16:
      return sizeof(uint16);
    case DT_INT32:
      return sizeof(int32);
    case DT_UINT32:
      return sizeof(uint32);
    case DT_INT64:
      return sizeof(int64);
    case DT_UINT64:
      return sizeof(uint64);
    case DT_HALF:
      CHIME_NOT_IMPLEMENTED;
    case DT_FLOAT32:
      return sizeof(float32);
    case DT_FLOAT64:
      return sizeof(float64);
    case DT_FLOAT128:
      CHIME_NOT_IMPLEMENTED;
    case DT_COMPLEX64:
      return sizeof(complex64);
    case DT_COMPLEX128:
      return sizeof(complex128);
    case DT_STRING:
      CHIME_NOT_IMPLEMENTED;
    case DT_INVALID:
      return 0ull;
    default:
      CHIME_NOT_IMPLEMENTED;
  }
}

static const uint8 kUINT8_MAX = static_cast<uint8>(0xFF);
static const uint16 kUINT16_MAX = static_cast<uint16>(0xFFFF);
static const uint32 kUINT32_MAX = static_cast<uint32>(0xFFFFFFFF);
static const uint64 kUINT64_MAX = static_cast<uint64>(0xFFFFFFFFFFFFFFFF);
static const int8 kINT8_MIN = static_cast<int8>(~0x7F);
static const int8 kINT8_MAX = static_cast<int8>(0x7F);
static const int16 kINT16_MIN = static_cast<int16>(~0x7FFF);
static const int16 kINT16_MAX = static_cast<int16>(0x7FFF);
static const int32 kINT32_MIN = static_cast<int32>(~0x7FFFFFFF);
static const int32 kINT32_MAX = static_cast<int32>(0x7FFFFFFF);
static const int64 kINT64_MIN = static_cast<int64>(~0x7FFFFFFFFFFFFFFF);
static const int64 kINT64_MAX = static_cast<int64>(0x7FFFFFFFFFFFFFFF);

}  // namespace chime
#endif  // CHIME_CORE_FRAMEWORK_TYPES_HPP_
