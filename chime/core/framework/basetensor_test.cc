// Copyright by 2022.4 chime
// author: yatorho

#include "chime/core/framework/basetensor.hpp"

namespace chime {

class BaseTensorTest : public ::testing::Test {};

TEST_F(BaseTensorTest, TestInitialization) {
  BaseTensor<float32> tensor1;
  EXPECT_EQ(tensor1.capacity(), 0ul);

  BaseTensor<float32> tensor2(std::vector<utens_t>({2, 3, 4, 5}));
  EXPECT_EQ(tensor2.count(), 2 * 3 * 4 * 5);
  EXPECT_EQ(tensor2.capacity(), tensor2.count());
  EXPECT_EQ(tensor2.num(), 2);
  EXPECT_EQ(tensor2.channels(), 3);
  EXPECT_EQ(tensor2.height(), 4);
  EXPECT_EQ(tensor2.width(), 5);
  EXPECT_EQ(tensor2.offset(std::vector<utens_t>({0, 0, 2, 4})), 14);
  EXPECT_EQ(tensor2.shape(-1), 5);
  EXPECT_EQ(tensor2.shape()[1], 3);
  EXPECT_EQ(tensor2.head(), SyncedMemory::UNINITIALIZED);
}

TEST_F(BaseTensorTest, TestMemoryInitialization) {
  std::vector<utens_t> shape1({4, 5});
  BaseTensor<float64> tensor1(shape1);
  EXPECT_EQ(tensor1.count(), 4 * 5);
  EXPECT_EQ(tensor1.shape()[1], shape1[1]);
  EXPECT_EQ(tensor1.head(), SyncedMemory::UNINITIALIZED);
  const float64 *data = tensor1.cpu_data();
  EXPECT_EQ(tensor1.head(), SyncedMemory::HEAD_AT_CPU);
  for (utens_t i = 0; i < tensor1.count(); i++) { EXPECT_EQ(data[i], 0); }
}

} // namespace chime
