// Copyright 2022.4 chime
// author: yatorho

#include "chime/core/framework/type.hpp"

namespace chime {
class ShapeTest : public ::testing::Test {};

TEST_F(ShapeTest, TestDataType) {

  EXPECT_TRUE(DataTypeToEnum<int32>::value == DT_INT32);
  EXPECT_TRUE(DataTypeToEnum<int>::v() == DT_INT32);

  EXPECT_EQ(sizeof(EnumToDataType<DT_FLOAT64>::type), 8);
}

} // namespace chime
