// Copyright by 2022.5 chime
// author: yatorho

#include "chime/core/framework/tensor.h"

#include <cstddef>

#include "chime/core/framework/common.hpp"
#include "chime/core/framework/shape_vec.hpp"
#include "chime/core/framework/syncedmem.hpp"
#include "chime/core/framework/tensor_shape.hpp"
#include "chime/core/framework/type.hpp"
#include "chime/core/memory/mem_optimizer.h"
#include "chime/core/memory/pool.hpp"

namespace chime {
class TensorTest : public ::testing::Test {
 public:
  static void test_set_host_data() {}
};

TEST_F(TensorTest, TestConstructor) {
  Tensor ts1(DT_INT32);
  EXPECT_TRUE(ts1.is_legal_shape());
  EXPECT_FALSE(ts1.is_initialized());
  EXPECT_TRUE(ts1.is_same_shape(Tensor()));
  EXPECT_TRUE(ts1.is_scalar());
  EXPECT_TRUE(ts1.check_dtype());

  Tensor ts2(DT_INT32, TensorShape({3, 4}));
  EXPECT_TRUE(ts2.is_legal_shape());
  EXPECT_FALSE(ts2.is_initialized());
  EXPECT_TRUE(ts2.is_same_shape(Tensor(DT_INVALID, TensorShape({3, 4}))));
  EXPECT_FALSE(ts2.is_scalar());
  EXPECT_TRUE(ts2.check_dtype());
  EXPECT_EQ(ts2.dims(), 2);
  EXPECT_EQ(ts2.dims_size(1), 4 * sizeof(utens_t));

  Tensor ts3(DT_INVALID, TensorShape({4}));
  EXPECT_EQ(ts3.dims(), 1);
  EXPECT_FALSE(ts3.check_dtype());
  EXPECT_EQ(ts3.dim_at(0), 4);
}

TEST_F(TensorTest, TestConstructorWithMemOper) {
  using memory::ChimeMemoryPool;

  ChimeMemoryPool mo(memory::CPU_MEMORY_TYPE, 200ul);
  // mo.init();

  Tensor ts1(mo, DT_FLOAT32, TensorShape());
  EXPECT_TRUE(ts1.is_scalar());
  EXPECT_EQ(ts1.num_elements(), 1ull);

  Tensor ts2(mo, DT_FLOAT64, TensorShape({3, 4}));
  EXPECT_EQ(ts2.total_bytes(), 3 * 4 * dtype_size(DT_FLOAT64));
}

TEST_F(TensorTest, TestWrite) {
  { /* *************** tensor(4, 3) int32 ****************** */

    Tensor ts(DT_INT32, TensorShape({5, 4}));
    auto ptr1 = ts.mutable_data<DT_INT32>(Tensor::HOST);

    for (utens_t i = 0; i < ts.num_elements(); i++) {
      EXPECT_EQ(ptr1[i], 0); // SyncedMemory::init_set_zero(init = true)
      ptr1[i] = i;
      // ts.set<DT_INT32>(TensorShape({i / 4, i % 4}), i, Tensor::HOST);
    }
    auto ptr2 = ts.data<DT_INT32>(Tensor::HOST);
    for (utens_t i = 0; i < ts.num_elements(); i++) {
      EXPECT_EQ(ptr1[i], i);
      EXPECT_EQ(ts.at<DT_INT32>(TensorShape({i / 4, i % 4}), Tensor::HOST), i);
    }
  }
  { /* *************** tensor(4, 3) int32 ****************** */
  }
}

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

  for (utens_t i = 0; i < 4; i++) {
    for (utens_t j = 0; j < 5; j++) {
      EXPECT_EQ(t.at<DT_INT32>({i, j}, Tensor::HOST), i * 5 + j);
      EXPECT_EQ(t.host_data<DT_INT32>()[i * 5 + j], i * 5 + j);
    }
  }

  free(buf);
}
} // namespace chime