// Copyright by 2022.5 chime
// author: yatorho

#include "chime/core/framework/tensor.h"

#include <cmath>
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

TEST_F(TensorTest, TestConstructor) { /// only host!
  Tensor ts1(DT_INT32);
  EXPECT_TRUE(ts1.is_legal_shape());
  EXPECT_FALSE(ts1.is_initialized());
  EXPECT_TRUE(ts1.is_same_shape(Tensor()));
  EXPECT_TRUE(ts1.is_scalar());
  EXPECT_TRUE(ts1.check_dtype());
  EXPECT_EQ(ts1.ref_count(), 1ull);

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

TEST_F(TensorTest, TestConstructorWithMemOper) { /// only host!
  using memory::ChimeMemoryPool;

  ChimeMemoryPool mo(ChimeMemoryPool::CPU_MEMORY_TYPE, 200ul);
  // mo.init();

  Tensor ts1(mo, DT_FLOAT32, TensorShape());
  EXPECT_TRUE(ts1.is_scalar());
  EXPECT_EQ(ts1.num_elements(), 1ull);

  Tensor ts2(mo, DT_FLOAT64, TensorShape({3, 4}));
  EXPECT_EQ(ts2.total_bytes(), 3 * 4 * dtype_size(DT_FLOAT64));
}

TEST_F(TensorTest, TestWrite) {
  Tensor ts(DT_INT32, TensorShape({5, 4}));
  EXPECT_FALSE(ts.is_initialized());
  auto ptr1 = ts.mutable_data<DT_INT32>(Tensor::HOST);
  EXPECT_TRUE(ts.is_initialized());

  for (utens_t i = 0; i < ts.num_elements(); i++) {
    EXPECT_EQ(ptr1[i], 0); // SyncedMemory::init_set_zero(init = true)
    ptr1[i] = i;
    // ts.set<DT_INT32>(TensorShape({i / 4, i % 4}), i, Tensor::HOST);
  }
  auto ptr2 = ts.data<DT_INT32>(Tensor::HOST);
  for (utens_t i = 0; i < ts.num_elements(); i++) {
    EXPECT_EQ(ptr2[i], i);
    EXPECT_EQ(ts.at<DT_INT32>(TensorShape({i / 4, i % 4}), Tensor::HOST), i);
  }
}

TEST_F(TensorTest, TestSingleWriteWithMemPool) { /// only host!
  using MemPool = memory::ChimeMemoryPool;
  MemPool mp(MemPool::CPU_MEMORY_TYPE, 4000);
  mp.init();

  Tensor ts(mp, DT_FLOAT32, TensorShape({10, 10, 10}),
            GRAPHICS_PROCESSING_UNIT);
  EXPECT_FALSE(ts.is_initialized());
  EXPECT_EQ(ts.total_bytes(), 10 * 10 * 10 * dtype_size(DT_FLOAT32));
  EXPECT_EQ(ts.allocated_bytes(), 0ull);

  for (utens_t i = 0; i < ts.dim_at(0); i++) {
    for (utens_t j = 0; j < ts.dim_at(1); j++) {
      for (utens_t k = 0; k < ts.dim_at(2); k++) {
        ts.set<DT_FLOAT32>(DimVector({i, j, k}), std::sqrt(i + j + k),
                           Tensor::HOST);
      }
    }
  }

  EXPECT_EQ(ts.total_bytes(), ts.allocated_bytes());

  float32 err = 1e-5;
  for (utens_t i = 0; i < ts.dim_at(0); i++) {
    for (utens_t j = 0; j < ts.dim_at(1); j++) {
      for (utens_t k = 0; k < ts.dim_at(2); k++) {
        EXPECT_LT(
          std::fabs(ts.at<DT_FLOAT32>(TensorShape({i, j, k}), Tensor::HOST)
                    - std::sqrt(i + j + k)),
          err);
      }
    }
  }
}

TEST_F(TensorTest, TestMultipleWriteWithMemPool) { /// only host!
  using MemPool = memory::ChimeMemoryPool;

  mems_t size1 = 800ull, size2 = 1000ull, size3 = 2000ull;
  MemPool mp(MemPool::CPU_MEMORY_TYPE, size1 * size2 * size3);
  mp.init();

  Tensor ts1(mp, DT_FLOAT64, TensorShape({4, 25}));
  Tensor ts2(mp, DT_FLOAT32, TensorShape({25, 10}));
  Tensor ts3(mp, DT_INT64, TensorShape({250}));

  EXPECT_EQ(ts1.device_name(), DeviceSupported::GRAPHICS_PROCESSING_UNIT);
  EXPECT_EQ(ts3.dims(), 1);

  for (utens_t i = 0; i < ts1.dim_at(0); i++) {
    for (utens_t j = 0; j < ts1.dim_at(1); j++) {
      ts1.set<DT_FLOAT64>(TensorShape({i, j}), i + j, Tensor::HOST);
    }
  }

  for (utens_t i = 0; i < ts2.dim_at(0); i++) {
    for (utens_t j = 0; j < ts2.dim_at(1); j++) {
      ts2.set<DT_FLOAT32>(TensorShape({i, j}), i * j, Tensor::HOST);
    }
  }

  for (utens_t i = 0; i < ts3.dim_at(0); i++) {
    ts3.set<DT_INT64>(TensorShape({i}), i, Tensor::HOST);
  }
  EXPECT_EQ(ts2.head(), SyncedMemory::HEAD_AT_HOST);

  float err = 1e-6;
  for (utens_t i = 0; i < ts1.dim_at(0); i++) {
    for (utens_t j = 0; j < ts1.dim_at(1); j++) {
      EXPECT_LT(std::abs(ts1.at<DT_FLOAT64>(TensorShape({i, j}), Tensor::HOST)
                         - (i + j)),
                err);
    }
  }

  for (utens_t i = 0; i < ts2.dim_at(0); i++) {
    for (utens_t j = 0; j < ts2.dim_at(1); j++) {
      EXPECT_LT(std::abs(ts2.at<DT_FLOAT32>(TensorShape({i, j}), Tensor::HOST)
                         - (i * j)),
                err);
    }
  }

  auto ptr = ts3.host_data<DT_INT64>();
  for (utens_t i = 0; i < ts3.dim_at(0); i++) { EXPECT_EQ(ptr[i], i); }
}

TEST_F(TensorTest, TestSetHostData) { /// only host!
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

TEST_F(TensorTest, TestCopyFrom) { /// only host!!!
  Tensor tensor1(DT_INT32, TensorShape({3, 4}));
  Tensor tensor2(DT_INT32, TensorShape({4, 3}));

  for (utens_t i = 0; i < tensor1.num_elements(); i++) {
    tensor1.set<DT_INT32>(
      TensorShape({i / tensor1.dim_at(1), i % tensor1.dim_at(1)}), i,
      Tensor::HOST);
    EXPECT_EQ(tensor1.at<DT_INT32>(
                TensorShape({i / tensor1.dim_at(1), i % tensor1.dim_at(1)}),
                Tensor::HOST),
              i);
  }

  tensor2.copy_from(tensor1, TensorShape({2, 6}));
  EXPECT_NE(tensor2.shape(), tensor1.shape());

  for (utens_t i = 0; i < tensor2.num_elements(); i++) {
    EXPECT_EQ(tensor2.at<DT_INT32>(
                TensorShape({i / tensor2.dim_at(1), i % tensor2.dim_at(1)}),
                Tensor::HOST),
              i);
  }
}

} // namespace chime