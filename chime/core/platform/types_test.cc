// Copyright 2022.4 chime. All rights reserved.
// author: yatorho

#include "chime/core/platform/types.h"

#include "chime/core/platform/test.hpp"
#include "chime/core/platform/types.pb.h"

namespace chime {

TEST(TypeTest, TestDataTypSize) {
  EXPECT_TRUE(DataTypeSize<DT_INT8>::size == 1);
  EXPECT_TRUE(DataTypeSize<DT_UINT8>::size == 1);
  EXPECT_TRUE(DataTypeSize<DT_INT16>::size == 2);
  EXPECT_TRUE(DataTypeSize<DT_UINT16>::size == 2);
  EXPECT_TRUE(DataTypeSize<DT_INT32>::size == 4);
  EXPECT_TRUE(DataTypeSize<DT_UINT32>::size == 4);
  EXPECT_TRUE(DataTypeSize<DT_INT64>::size == 8);
  EXPECT_TRUE(DataTypeSize<DT_UINT64>::size == 8);
  EXPECT_TRUE(DataTypeSize<DT_HALF>::size == 2);
  EXPECT_TRUE(DataTypeSize<DT_FLOAT16>::size == 2);
  EXPECT_TRUE(DataTypeSize<DT_BFLOAT16>::size == 2);
  EXPECT_TRUE(DataTypeSize<DT_FLOAT32>::size == 4);
  EXPECT_TRUE(DataTypeSize<DT_FLOAT64>::size == 8);
  EXPECT_TRUE(DataTypeSize<DT_FLOAT128>::size == 16);
  EXPECT_TRUE(DataTypeSize<DT_COMPLEX32>::size == 4);
  EXPECT_TRUE(DataTypeSize<DT_COMPLEX64>::size == 8);
  EXPECT_TRUE(DataTypeSize<DT_COMPLEX128>::size == 16);
  EXPECT_TRUE(DataTypeSize<DT_BOOL>::size == 1);
  EXPECT_TRUE(DataTypeSize<DT_CHAR>::size == 1);
#ifdef __GNUC__
  EXPECT_TRUE(DataTypeSize<DT_INT128>::size == 16);
  EXPECT_TRUE(DataTypeSize<DT_UINT128>::size == 16);
#endif
}

TEST(TypeTest, TestDataTypeSizeInRunTime) {
  EXPECT_EQ(GetDataTypeSize(DT_INT8), 1);
  EXPECT_EQ(GetDataTypeSize(DT_UINT8), 1);
  EXPECT_EQ(GetDataTypeSize(DT_INT16), 2);
  EXPECT_EQ(GetDataTypeSize(DT_UINT16), 2);
  EXPECT_EQ(GetDataTypeSize(DT_INT32), 4);
  EXPECT_EQ(GetDataTypeSize(DT_UINT32), 4);
  EXPECT_EQ(GetDataTypeSize(DT_CHAR), 1);
  EXPECT_EQ(GetDataTypeSize(DT_INT64), 8);
  EXPECT_EQ(GetDataTypeSize(DT_UINT64), 8);
  EXPECT_EQ(GetDataTypeSize(DT_FLOAT16), 2);
  EXPECT_EQ(GetDataTypeSize(DT_FLOAT32), 4);
  EXPECT_EQ(GetDataTypeSize(DT_FLOAT64), 8);
  EXPECT_EQ(GetDataTypeSize(DT_FLOAT128), 16);
  EXPECT_EQ(GetDataTypeSize(DT_COMPLEX32), 4);
  EXPECT_EQ(GetDataTypeSize(DT_COMPLEX64), 8);
  EXPECT_EQ(GetDataTypeSize(DT_COMPLEX128), 16);
  EXPECT_EQ(GetDataTypeSize(DT_BOOL), 1);
  EXPECT_EQ(GetDataTypeSize(DT_BFLOAT16), 2);
  EXPECT_EQ(GetDataTypeSize(DT_HALF), 2);
#ifdef __GNUC__
  EXPECT_EQ(GetDataTypeSize(DT_INT128), 16);
  EXPECT_EQ(GetDataTypeSize(DT_UINT128), 16);
#endif
}

#define ENUM_TO_DATA_TYPE_TEST_CASE(ENUM, TYPE)                          \
  {                                                                      \
    EnumToDataType<ENUM>::type value(1);                                 \
    static_assert(std::is_same<TYPE, decltype(value)>::value, "Failed"); \
    EXPECT_EQ(sizeof(value), sizeof(TYPE));                              \
    EXPECT_EQ(sizeof(EnumToDataType<ENUM>::type), ENUM##_SIZE);          \
  }

TEST(TypeTest, EnumToDataType) {
  ENUM_TO_DATA_TYPE_TEST_CASE(DT_INT8, int8);
  ENUM_TO_DATA_TYPE_TEST_CASE(DT_UINT8, uint8);
  ENUM_TO_DATA_TYPE_TEST_CASE(DT_INT16, int16);
  ENUM_TO_DATA_TYPE_TEST_CASE(DT_UINT16, uint16);
  ENUM_TO_DATA_TYPE_TEST_CASE(DT_INT32, int32);
  ENUM_TO_DATA_TYPE_TEST_CASE(DT_UINT32, uint32);
  ENUM_TO_DATA_TYPE_TEST_CASE(DT_INT64, int64);
  ENUM_TO_DATA_TYPE_TEST_CASE(DT_UINT64, uint64);
  ENUM_TO_DATA_TYPE_TEST_CASE(DT_FLOAT16, float16);
  ENUM_TO_DATA_TYPE_TEST_CASE(DT_FLOAT32, float32);
  ENUM_TO_DATA_TYPE_TEST_CASE(DT_FLOAT64, float64);
  ENUM_TO_DATA_TYPE_TEST_CASE(DT_FLOAT128, float128);
  ENUM_TO_DATA_TYPE_TEST_CASE(DT_COMPLEX32, complex32);
  ENUM_TO_DATA_TYPE_TEST_CASE(DT_COMPLEX64, complex64);
  ENUM_TO_DATA_TYPE_TEST_CASE(DT_COMPLEX128, complex128);
  ENUM_TO_DATA_TYPE_TEST_CASE(DT_BOOL, bool);
  ENUM_TO_DATA_TYPE_TEST_CASE(DT_CHAR, char);
  ENUM_TO_DATA_TYPE_TEST_CASE(DT_HALF, half);
#ifdef __GNUC__
  ENUM_TO_DATA_TYPE_TEST_CASE(DT_INT128, int128);
  ENUM_TO_DATA_TYPE_TEST_CASE(DT_UINT128, uint128);
#endif
}
#undef ENUM_TO_DATA_TYPE_TEST_CASE

#define DATA_TYPE_TO_ENUM_TEST_CASE(TYPE, ENUM)                         \
  {                                                                     \
    TYPE value(1);                                                      \
    static_assert(DataTypeToEnum<TYPE>::value == ENUM, "Failed");       \
    EXPECT_EQ(DataTypeToEnum<TYPE>::v(), ENUM);                         \
    static_assert(DataTypeSize<ENUM>::size == sizeof(value), "Failed"); \
  }

TEST(TypeTest, DataTypeToEnum) {
  DATA_TYPE_TO_ENUM_TEST_CASE(int8, DT_INT8);
  DATA_TYPE_TO_ENUM_TEST_CASE(uint8, DT_UINT8);
  DATA_TYPE_TO_ENUM_TEST_CASE(int16, DT_INT16);
  DATA_TYPE_TO_ENUM_TEST_CASE(uint16, DT_UINT16);
  DATA_TYPE_TO_ENUM_TEST_CASE(int32, DT_INT32);
  DATA_TYPE_TO_ENUM_TEST_CASE(uint32, DT_UINT32);
  DATA_TYPE_TO_ENUM_TEST_CASE(int64, DT_INT64);
  DATA_TYPE_TO_ENUM_TEST_CASE(uint64, DT_UINT64);
  DATA_TYPE_TO_ENUM_TEST_CASE(float16, DT_HALF);
  DATA_TYPE_TO_ENUM_TEST_CASE(float32, DT_FLOAT32);
  DATA_TYPE_TO_ENUM_TEST_CASE(float64, DT_FLOAT64);
  DATA_TYPE_TO_ENUM_TEST_CASE(float128, DT_FLOAT128);
  DATA_TYPE_TO_ENUM_TEST_CASE(complex32, DT_COMPLEX32);
  DATA_TYPE_TO_ENUM_TEST_CASE(complex64, DT_COMPLEX64);
  DATA_TYPE_TO_ENUM_TEST_CASE(complex128, DT_COMPLEX128);
  DATA_TYPE_TO_ENUM_TEST_CASE(bool, DT_BOOL);
  DATA_TYPE_TO_ENUM_TEST_CASE(char, DT_CHAR);
  DATA_TYPE_TO_ENUM_TEST_CASE(half, DT_HALF);
#ifdef __GNUC__
  DATA_TYPE_TO_ENUM_TEST_CASE(int128, DT_INT128);
  DATA_TYPE_TO_ENUM_TEST_CASE(uint128, DT_UINT128);
#endif
}

#undef DATA_TYPE_TO_ENUM_TEST_CASE

}  // namespace chime
