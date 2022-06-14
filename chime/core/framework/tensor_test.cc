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
#include "chime/core/schema/types.pb.h"

namespace chime {
class TensorTest : public ::testing::Test {
 public:
  static void test_set_host_data() {}
};

TEST_F(TensorTest, TestConstructor) {  /// only host!
  Tensor ts1(DT_INT32);
  EXPECT_TRUE(ts1.is_legal_shape());
  EXPECT_FALSE(ts1.is_initialized());
  EXPECT_TRUE(ts1.is_same_shape(Tensor(DT_INT32, TensorShape())));
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

TEST_F(TensorTest, TestCopyConstructor) {  /// only host!!!
  Tensor ts1(DT_INT32, TensorShape({3, 4}));
  Tensor ts2(ts1);
  EXPECT_FALSE(ts1.is_initialized());
  EXPECT_EQ(ts1.buffer(Tensor::HOST), ts2.buffer(Tensor::HOST));

  auto ptr = ts1.mutable_data<DT_INT32>(Tensor::HOST);
  for (utens_t i = 0; i < 3 * 4; i++) ptr[i] = i;
  EXPECT_TRUE(ts2.is_initialized());
  ptr = ts2.mutable_host_data<DT_INT32>();
  for (utens_t i = 0; i < 3 * 4; i++) EXPECT_EQ(ptr[i], i);
}

TEST_F(TensorTest, TestMovedCopyConstructor) {  // only host!!!
  Tensor ts(Tensor(DT_INT32, TensorShape({4, 3})));
  EXPECT_FALSE(ts.is_initialized());
  auto ptr = ts.mutable_data<DT_INT32>(Tensor::HOST);
  for (utens_t i = 0; i < 4 * 3; i++) ptr[i] = i;

  for (utens_t i = 0; i < 4 * 3; i++) EXPECT_EQ(ptr[i], i);
}

TEST_F(TensorTest, TestConstructorWithMemOper) {  /// only host!
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
    EXPECT_EQ(ptr1[i], 0);  // SyncedMemory::init_set_zero(init = true)
    ptr1[i] = i;
    // ts.set<DT_INT32>(TensorShape({i / 4, i % 4}), i, Tensor::HOST);
  }
  auto ptr2 = ts.data<DT_INT32>(Tensor::HOST);
  for (utens_t i = 0; i < ts.num_elements(); i++) {
    EXPECT_EQ(ptr2[i], i);
    EXPECT_EQ(ts.at<DT_INT32>(TensorShape({i / 4, i % 4}), Tensor::HOST), i);
  }
}

TEST_F(TensorTest, TestSingleWriteWithMemPool) {  /// only host!
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
            std::fabs(ts.at<DT_FLOAT32>(TensorShape({i, j, k}), Tensor::HOST) -
                      std::sqrt(i + j + k)),
            err);
      }
    }
  }
}

TEST_F(TensorTest, TestMultipleWriteWithMemPool) {  /// only host!
  using MemPool = memory::ChimeMemoryPool;

  mems_t size1 = 800ull, size2 = 1000ull, size3 = 2000ull;
  MemPool mp(MemPool::CPU_MEMORY_TYPE, size1 + size2 + size3);
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
      EXPECT_LT(std::abs(ts1.at<DT_FLOAT64>(TensorShape({i, j}), Tensor::HOST) -
                         (i + j)),
                err);
    }
  }

  for (utens_t i = 0; i < ts2.dim_at(0); i++) {
    for (utens_t j = 0; j < ts2.dim_at(1); j++) {
      EXPECT_LT(std::abs(ts2.at<DT_FLOAT32>(TensorShape({i, j}), Tensor::HOST) -
                         (i * j)),
                err);
    }
  }

  auto ptr = ts3.host_data<DT_INT64>();
  for (utens_t i = 0; i < ts3.dim_at(0); i++) {
    EXPECT_EQ(ptr[i], i);
  }
}

TEST_F(TensorTest, TestSetHostData) {  /// only host!
  TensorShape ts({4, 5});
  Tensor t(DT_INT32, ts);
  EXPECT_EQ(t.total_bytes(), 4 * 5 * dtype_size(DT_INT32));
  EXPECT_EQ(t.head(), SyncedMemory::UNINITIALIZED);
  EXPECT_EQ(t.allocated_bytes(), 0ull);

  EnumToDataType<DT_INT32>::type *buf;
  buf = (int32 *)(malloc(4 * 5 * sizeof(int32)));

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

TEST_F(TensorTest, TestCopyFrom) {  /// only host!!!
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

TEST_F(TensorTest, TestCopy) {  /// only host!!!
  Tensor ts1(DT_INT32, TensorShape({3, 4, 5}));
  Tensor ts2(DT_FLOAT32, TensorShape({100}));
  Tensor ts3;
  EXPECT_EQ(ts3.num_elements(), 0);

  EXPECT_EQ(ts1.ref_count(), 1ull);

  ts2 = ts1;
  EXPECT_EQ(ts2.ref_count(), 2ull);
  EXPECT_EQ(ts2.dtype(), DT_INT32);
  EXPECT_EQ(ts2.shape(), TensorShape({3, 4, 5}));

  for (utens_t i = 0; i < 3; i++) {
    for (utens_t j = 0; j < 4; j++) {
      for (utens_t k = 0; k < 5; k++) {
        ts1.set<DT_INT32>(DimVector({i, j, k}), i + j + k, Tensor::HOST);
        EXPECT_EQ(ts2.at<DT_INT32>(TensorShape({i, j, k}), Tensor::HOST),
                  i + j + k);
      }
    }
  }

  ts3 = ts2;
  EXPECT_EQ(ts1.ref_count(), 3ull);
  EXPECT_EQ(ts3.ref_count(), ts1.ref_count());
  EXPECT_EQ(ts3.shape(), TensorShape({3, 4, 5}));
  EXPECT_TRUE(ts3.is_same_buffer(ts2));

  for (utens_t i = 0; i < 3; i++) {
    for (utens_t j = 0; j < 4; j++) {
      for (utens_t k = 0; k < 5; k++) {
        EXPECT_EQ(ts3.at<DT_INT32>(TensorShape({i, j, k}), Tensor::HOST),
                  i + j + k);
      }
    }
  }

  ts3 = Tensor();
  EXPECT_EQ(ts1.ref_count(), 2ull);
  EXPECT_FALSE(ts1.is_same_buffer(ts3));
  ts2 = Tensor(DT_INVALID);
  EXPECT_EQ(ts1.ref_count(), 1ull);
}

TEST_F(TensorTest, TestMovedCopy) {  /// only host!!!
  Tensor ts;
  EXPECT_EQ(ts.dtype(), DT_INVALID);
  EXPECT_EQ(ts.ref_count(), 1ull);
  // auto ptr = ts.data<DT_INT32>(Tensor::HOST);  // Check Failed

  ts = Tensor(DT_INT32, TensorShape({100}));
  EXPECT_EQ(ts.dtype(), DT_INT32);
  EXPECT_EQ(ts.ref_count(), 1ull);
}

TEST(Tensor, TestAsProtoTensorContent) {
  {  /// for DT_INT32
    TensorProto proto;
    Tensor tensor(DT_INT32, TensorShape({3, 4}));

    for (size_t i = 0; i < tensor.dim_at(0); i++) {
      for (size_t j = 0; j < tensor.dim_at(1); j++) {
        tensor.set<DT_INT32>({i, j}, i + j, Tensor::HOST);
      }
    }
    tensor.as_proto_tensor_content(&proto);
    EXPECT_EQ(proto.tensor_content().size(), sizeof(int32) * 3 * 4);

    const int *ptr =
        reinterpret_cast<const int *>(proto.tensor_content().data());
    for (size_t i = 0; i < tensor.dim_at(0); i++) {
      for (size_t j = 0; j < tensor.dim_at(1); j++) {
        EXPECT_EQ(ptr[i * tensor.dim_at(1) + j], i + j);
      }
    }
    EXPECT_EQ(proto.dtype(), DT_INT32);
  }
  {  /// for DT_INT16
    TensorProto proto;
    Tensor tensor(DT_INT16, TensorShape({3, 4}));

    for (size_t i = 0; i < tensor.dim_at(0); i++) {
      for (size_t j = 0; j < tensor.dim_at(1); j++) {
        tensor.set<DT_INT16>({i, j}, i + j, Tensor::HOST);
      }
    }
    tensor.as_proto_tensor_content(&proto);
    EXPECT_EQ(proto.tensor_content().size(), sizeof(int16) * 3 * 4);
    const int16 *ptr =
        reinterpret_cast<const int16 *>(proto.tensor_content().data());
    for (size_t i = 0; i < tensor.dim_at(0); i++) {
      for (size_t j = 0; j < tensor.dim_at(1); j++) {
        EXPECT_EQ(ptr[i * tensor.dim_at(1) + j], i + j);
      }
    }
    EXPECT_EQ(proto.tensor_shape().dims(0).size(), 3);
    EXPECT_EQ(proto.tensor_shape().dims(1).size(), 4);

    EXPECT_TRUE(proto.int_val().empty());
  }
  {  /// for DT_INT8
    using MemPool = memory::ChimeMemoryPool;
    MemPool mp(MemPool::CPU_MEMORY_TYPE, 9 * 10 * 11 * dtype_size(DT_INT8));
    mp.init();
    TensorProto proto;
    Tensor tensor(mp, DT_INT8, TensorShape({9, 10, 11}));

    for (size_t i = 0; i < tensor.dim_at(0); i++) {
      for (size_t j = 0; j < tensor.dim_at(1); j++) {
        for (size_t k = 0; k < tensor.dim_at(2); k++) {
          tensor.set<DT_INT8>({i, j, k}, 100, Tensor::HOST);
        }
      }
    }

    tensor.as_proto_tensor_content(&proto);
    const int8 *ptr =
        reinterpret_cast<const int8 *>(proto.tensor_content().data());
    for (size_t i = 0; i < 9 * 10 * 11; i++) {
      EXPECT_EQ(ptr[i], 100);
    }
    EXPECT_EQ(proto.tensor_shape().dims(0).size(), 9);
    EXPECT_EQ(proto.tensor_shape().dims(2).size(), 11);
  }
}

TEST(Tensor, TestAsProtoField) {
  {  /// for INT64
    Tensor tensor(DT_INT64, TensorShape({100}));
    for (size_t i = 0; i < 100; i++) {
      tensor.set<DT_INT64>({i}, i, Tensor::HOST);
    }

    TensorProto proto;
    tensor.as_proto_field(&proto);
    EXPECT_TRUE(proto.tensor_content().empty());
    EXPECT_FALSE(proto.int64_val().empty());

    for (size_t i = 0; i < 100; i++) {
      EXPECT_EQ(proto.int64_val(i), i);
    }
  }
  {  /// for DT_FLOAT32
    Tensor tensor(DT_FLOAT32, TensorShape({100}));
    for (size_t i = 0; i < 100; i++) {
      tensor.set<DT_FLOAT32>({i}, i, Tensor::HOST);
    }

    TensorProto proto;
    tensor.as_proto_field(&proto);
    EXPECT_TRUE(proto.tensor_content().empty());
    EXPECT_FALSE(proto.float32_val().empty());

    float err = 1e-5;
    for (size_t i = 0; i < 100; i++) {
      EXPECT_LT(std::fabs(proto.float32_val(i) - i), err);
    }
  }
  {  /// for DT_STRING
  }
}

TEST(Tensor, TestFromProto) {
  {  /// for DT_INT32
    TensorProto proto;
    Tensor tensor(DT_INT32, TensorShape({3, 4}));

    for (size_t i = 0; i < tensor.dim_at(0); i++) {
      for (size_t j = 0; j < tensor.dim_at(1); j++) {
        tensor.set<DT_INT32>({i, j}, i + j, Tensor::HOST);
      }
    }
    tensor.as_proto_tensor_content(&proto);

    Tensor test;
    ASSERT_TRUE(test.from_proto(proto));
    EXPECT_EQ(test.shape(), tensor.shape());
    EXPECT_TRUE(test.check_dtype());
    EXPECT_FALSE(test.is_scalar());
    EXPECT_FALSE(test.is_same_buffer(tensor));

    for (size_t i = 0; i < tensor.dim_at(0); i++) {
      for (size_t j = 0; j < tensor.dim_at(1); j++) {
        EXPECT_EQ(test.at<DT_INT32>(TensorShape({i, j}), Tensor::HOST), i + j);
      }
    }
  }
  {  /// for DT_FLOAT64 scalar
    TensorProto proto;
    Tensor tensor(DT_FLOAT64, {});
    tensor.set<DT_FLOAT64>(TensorShape(), 100., Tensor::HOST);
    EXPECT_TRUE(tensor.is_scalar());

    tensor.as_proto_field(&proto);
    EXPECT_TRUE(proto.tensor_content().empty());
    EXPECT_FALSE(proto.float64_val().empty());

    Tensor test;
    test.from_proto(proto);

    EXPECT_TRUE(test.is_scalar());
    double err = 1e-20;
    EXPECT_LT(std::abs(test.at<DT_FLOAT64>(TensorShape(), Tensor::HOST) - 100.),
              err);
  }
}

}  // namespace chime
