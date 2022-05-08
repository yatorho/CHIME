// Copyright 2022.4 chime
// author: yatorho

#include "chime/core/framework/type.hpp"

namespace chime {
class TypeTest : public ::testing::Test {};

TEST_F(TypeTest, TestDataType) {

  EXPECT_TRUE(DataTypeToEnum<int32>::value == DT_INT32);
  EXPECT_TRUE(DataTypeToEnum<int>::v() == DT_INT32);

  EXPECT_EQ(sizeof(EnumToDataType<DT_FLOAT64>::type), 8);
}

TEST_F(TypeTest, TestDtypSize) {
  EXPECT_EQ(dtype_size(DT_INT32), 4);
  EXPECT_EQ(dtype_size(DT_FLOAT64), 8);
  EXPECT_EQ(dtype_size(DT_UINT16), 2);
  EXPECT_EQ(dtype_size(DT_INT8), 1);
}

} // namespace chime
