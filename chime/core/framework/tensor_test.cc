// Copyright by 2022.5 chime
// author: yatorho

#include "chime/core/framework/tensor.h"

#include "chime/core/framework/syncedmem.hpp"
#include "chime/core/framework/tensor_shape.hpp"
#include "chime/core/framework/type.hpp"

namespace chime {
class TensorTest : public ::testing::Test {
 public:
  static void test_set_host_data() {

  }
};

TEST_F(TensorTest, TestSetHostData) {
  TensorShape ts({4, 5});
  Tensor t(DT_INT32, ts);
  EXPECT_EQ(t.total_bytes(), 4 * 5 * dtype_size(DT_INT32));
  EXPECT_EQ(t.head(), SyncedMemory::UNINITIALIZED);
  EXPECT_EQ(t.allocated_bytes(), 0ull);

  EnumToDataType<DT_INT32>::type *buf;
  buf = (int32 *) (malloc(4 * 5 * sizeof(int32)));

  for (utens_t i = 0; i < 4 * 5; i++) buf[i] = i;

  t.set_host_data<DT_INT32>(buf);
  EXPECT_EQ(t.total_bytes(), 4 * 5 * dtype_size(DT_INT32));
  EXPECT_EQ(t.allocated_bytes(), 0ull);
  EXPECT_EQ(t.head(), SyncedMemory::HEAD_AT_HOST);

  for (utens_t i = 0; i < 4 * 5; i++) {
    EXPECT_EQ(t.host_data<DT_INT32>()[i], i);
  }
  free(buf);
}
} // namespace chime