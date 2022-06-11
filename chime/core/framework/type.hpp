// Copyright 2022.4 chime
// author: yatorho

#ifndef CHIME_CORE_FRAMEWORK_TYPE_HPP_
#define CHIME_CORE_FRAMEWORK_TYPE_HPP_

#include "chime/core/framework/common.hpp"
#include "chime/core/schema/types.pb.h"

namespace chime {

/*
typedef enum {
  DT_BOOL = 0,
  DT_INT8 = 1,
  DT_UINT8 = 2,
  DT_INT16 = 3,
  DT_UINT16 = 4,
  DT_INT32 = 5,
  DT_UINT32 = 6,
  DT_INT64 = 7,
  DT_UINT64 = 8,
  DT_HALF = 9,
  DT_FLOAT32 = 10,
  DT_FLOAT64 = 11,
  DT_FLOAT128 = 12,
  DT_COMPLEX64 = 13,
  DT_COMPLEX128 = 14,
  DT_STRING = 15,
  DT_INVALID = 16
} DataType;
*/

#define DT_FLOAT DT_FLOAT32
#define DT_DOUBLE DT_FLOAT64

template<class T>
struct IsValidDataType;

template<class T>
struct DataTypeToEnum {
  static_assert(IsValidDataType<T>::value, "Specified Data Type not supported");
};

template<DataType T>
struct EnumToDataType {};

#define MATCH_TYPE_AND_ENUM(TYPE, ENUM)     \
  template<>                                \
  struct DataTypeToEnum<TYPE> {             \
    static DataType v() { return ENUM; }    \
    static constexpr DataType value = ENUM; \
  };                                        \
  template<>                                \
  struct IsValidDataType<TYPE> {            \
    static constexpr bool value = true;     \
  };                                        \
  template<>                                \
  struct EnumToDataType<ENUM> {             \
    typedef TYPE type;                      \
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
MATCH_TYPE_AND_ENUM(string, DT_STRING);

template<>
struct DataTypeToEnum<long> {
  static DataType v() { return value; }
  static constexpr DataType value = sizeof(long) == 4 ? DT_INT32 : DT_INT64;
};

template<>
struct IsValidDataType<long> {
  static constexpr bool value = true;
};

template<>
struct DataTypeToEnum<unsigned long> {
  static DataType v() { return value; }
  static constexpr DataType value =
    sizeof(unsigned long) == 4 ? DT_UINT32 : DT_UINT64;
};

template<>
struct IsValidDataType<unsigned long> {
  static constexpr bool value = true;
};

template<class T>
struct IsValidDataType {
  static constexpr bool value = false;
};

#undef MATCH_TYPE_AND_ENUM

inline size_t dtype_size(DataType dtype) {
  switch (dtype) {
    case DT_BOOL: return sizeof(bool);
    case DT_INT8: return sizeof(int8);
    case DT_UINT8: return sizeof(uint8);
    case DT_INT16: return sizeof(int16);
    case DT_UINT16: return sizeof(uint16);
    case DT_INT32: return sizeof(int32);
    case DT_UINT32: return sizeof(uint32);
    case DT_INT64: return sizeof(int64);
    case DT_UINT64: return sizeof(uint64);
    case DT_HALF: NOT_IMPLEMENTED;
    case DT_FLOAT32: return sizeof(float32);
    case DT_FLOAT64: return sizeof(float64);
    case DT_FLOAT128: NOT_IMPLEMENTED;
    case DT_COMPLEX64: return sizeof(complex64);
    case DT_COMPLEX128: return sizeof(complex128);
    case DT_STRING: NOT_IMPLEMENTED;
    case DT_INVALID: return 0ull;
    default: NOT_IMPLEMENTED;
  }
}
} // namespace chime
#endif // CHIME_CORE_FRAMEWORK_TYPE_HPP_
