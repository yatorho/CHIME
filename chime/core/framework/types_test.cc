// Copyright 2022.4 chime
// author: yatorho

#include "chime/core/framework/types.hpp"
#include "chime/core/platform/test.hpp"

namespace chime {
class TypeTest : public ::testing::Test {};

TEST_F(TypeTest, TestDataType) {

  EXPECT_TRUE(DataTypeToEnum<int32>::value == DT_INT32);
  EXPECT_TRUE(DataTypeToEnum<int>::v() == DT_INT32);

  EXPECT_EQ(sizeof(EnumToDataType<DT_FLOAT64>::type), 8);
}

TEST_F(TypeTest, TestDtypSize) {
  EXPECT_TRUE(DtypeSize<DT_INT32>::size == 4);
  EXPECT_TRUE(DtypeSize<DT_FLOAT64>::size == 8);
  EXPECT_TRUE(DtypeSize<DT_UINT16>::size == 2);
  EXPECT_TRUE(DtypeSize<DT_INT8>::size == 1);
  EXPECT_TRUE(DtypeSize<DT_INT32>::size == 4);
}

}  // namespace chime
