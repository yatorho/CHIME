// Copyright 2022.8 chime. All rights reserved.
// author: yatorho

#include "chime/core/framework/tensor_shape.h"

#include <limits>

#include "chime/core/framework/shape_vec.h"
#include "chime/core/framework/tensor_shape.pb.h"
#include "chime/core/platform/test.hpp"
#include "google/protobuf/stubs/common.h"

namespace chime {
namespace core {

class TensorShapeTest : public ::testing::Test {
 public:
  static void TestTensorShapeOperatorAssign() {
    TensorShape ts1({6, 4});
    TensorShape ts2({3, 4, 4, 3, 2});

    DimVector *ptr1 = &ts1._dim_vec;
    DimVector *ptr2 = &ts2._dim_vec;
    EXPECT_NE(&ts1._dim_vec, &ts2._dim_vec);

    ts2 = ts1;

    EXPECT_EQ(ptr1, &ts1._dim_vec);
    EXPECT_EQ(ptr2, &ts2._dim_vec);
    EXPECT_NE(ptr1, ptr2);
  }
};

TEST(TensorShape, TestConstruct) {
  {  // default constructor
    TensorShape ts;
    EXPECT_EQ(ts.NumElements(), 1);
    EXPECT_EQ(ts.NumDims(), 0);
    EXPECT_TRUE(ts.CheckLegality());
  }
  {  // TensorShape(const DimVector &dim_vec)
    DimVector dim_vec({1, 2, 3});
    TensorShape ts(dim_vec);
    EXPECT_EQ(ts.NumElements(), 6);
    EXPECT_EQ(ts.NumDims(), 3);
  }
  {  // TensorShape(DimVector &&dim_vec)
    TensorShape ts(DimVector({3, 4, 5, 6}));
    EXPECT_EQ(ts.NumElements(), 3 * 4 * 5 * 6);
    EXPECT_EQ(ts.NumDims(), 4);
  }
  {  // TensorShape(const TensorShape &other);
    DimVector dim_vec({1, 2, 3});
    TensorShape ts1(dim_vec);

    TensorShape ts2(ts1);
    EXPECT_EQ(ts2.NumElements(), 6);
    EXPECT_EQ(ts2.NumDims(), 3);
  }
  {  // TensorShape(TensorShape &&other)
    TensorShape ts1((TensorShape &&) TensorShape({1, 2, 3, 4}));
    EXPECT_EQ(ts1.NumElements(), 24);
    EXPECT_EQ(ts1.NumDims(), 4);
  }
  {
    TensorShape ts({3, 0, 4});
    EXPECT_EQ(ts.NumElements(), 0);
    EXPECT_EQ(ts.NumDims(), 3);
    EXPECT_FALSE(ts.CheckLegality());
  }
}

TEST(TensorShape, ShapeAddDim) {
  TensorShape ts({1, 2, 3});
  ts.AddDim(4);
  EXPECT_EQ(ts.NumElements(), 4 * 2 * 3);
  EXPECT_EQ(ts.NumDims(), 4);
  EXPECT_TRUE(ts.CheckLegality());

  ts.AddDim(0);
  EXPECT_EQ(ts.NumElements(), 0);
  EXPECT_EQ(ts.NumDims(), 5);
  EXPECT_FALSE(ts.CheckLegality());
}

TEST(TensorShape, ShapeRemoveDim) {
  TensorShape ts({1, 2, 3});
  ts.RemoveDim(2);
  EXPECT_EQ(ts.NumElements(), 1 * 2 * 1);
  EXPECT_EQ(ts.NumDims(), 2);
  EXPECT_TRUE(ts.CheckLegality());

  TensorShape ts1({3, 4, 5});
  EXPECT_EQ(ts1.NumDims(), 3);
  ts1.SetDim(2, 0);
  EXPECT_EQ(ts1.NumElements(), 3 * 4 * 0);
  EXPECT_FALSE(ts1.CheckLegality());

  ts1.RemoveDim(2);
  EXPECT_EQ(ts1.NumDims(), 2);
  EXPECT_EQ(ts1.NumElements(), 3 * 4);
  EXPECT_TRUE(ts1.CheckLegality());
}

TEST(TensorShape, ShapeAppendShape) {
  TensorShape ts1({1, 2, 3});
  TensorShape ts2({4, 5, 6});
  ts1.AppendShape(ts2);
  EXPECT_EQ(ts1.NumElements(), 1 * 2 * 3 * 4 * 5 * 6);
  EXPECT_EQ(ts1.NumDims(), 6);
  EXPECT_TRUE(ts1.CheckLegality());

  ts1.RemoveDimRange(3, 5);
  EXPECT_EQ(ts1.NumDims(), 4);
  EXPECT_EQ(ts1.NumElements(), 1 * 2 * 3 * 6);
  EXPECT_TRUE(ts1.CheckLegality());

  ts1.RemoveLastDims(2);
  EXPECT_EQ(ts1.NumDims(), 2);
  EXPECT_EQ(ts1.NumElements(), 1 * 2);
  EXPECT_TRUE(ts1.CheckLegality());

  // Expect append opeartor does not change ts2.
  EXPECT_EQ(ts2.NumDims(), 3);
  EXPECT_EQ(ts2.NumElements(), 4 * 5 * 6);
  EXPECT_TRUE(ts2.CheckLegality());
}

TEST(TensorShape, ShapeInsertDim) {
  TensorShape ts({1, 2, 3});
  ts.InsertDim(1, 4);
  EXPECT_EQ(ts.NumElements(), 1 * 4 * 2 * 3);
  EXPECT_EQ(ts.NumDims(), 4);

  EXPECT_EQ(ts(1), 4);
  EXPECT_EQ(ts.At(2), 2);

  ts.InsertDim(0, 0);
  EXPECT_EQ(ts.NumElements(), 0 * 1 * 4 * 2 * 3);
  EXPECT_EQ(ts.NumDims(), 5);
  EXPECT_EQ(ts.At(0), 0);
  EXPECT_EQ(ts[1], 1);
  EXPECT_EQ(ts.At(2), 4);
  EXPECT_FALSE(ts.CheckLegality());
}

TEST(TensorShape, ShapeOffset) {
  TensorShape ts({3, 4});
  EXPECT_EQ(ts.Offset(TensorShape({0, 0})), 0);
  EXPECT_EQ(ts.Offset(TensorShape({1, 0})), 4);
  EXPECT_EQ(ts.Offset(TensorShape({2, 0})), 8);
  EXPECT_EQ(ts.Offset(TensorShape({1, 3})), 7);
  EXPECT_EQ(ts.Offset(TensorShape({0, 2})), 2);
  EXPECT_EQ(ts.Offset(TensorShape({1})), 4);
  EXPECT_EQ(ts.Offset(TensorShape({0})), 0);
  EXPECT_EQ(ts.Offset(TensorShape({2})), 8);
}

TEST(TensorShape, TestOverflow) {
  TensorShape ts1({std::numeric_limits<int64_t>::max(), 1});
  EXPECT_EQ(ts1.NumElements(), std::numeric_limits<int64_t>::max());
}

TEST_F(TensorShapeTest, TestTensorShapeOperatorAssign) {
  TensorShapeTest::TestTensorShapeOperatorAssign();
}

TEST(TensorShape, TestToProto) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  TensorShape ts({10, 0, 12});
  EXPECT_FALSE(ts.CheckLegality());

  TensorShapeProto proto;
  bool res = ts.ToProto(&proto);
  EXPECT_FALSE(res);

  ts.SetDim(1, 8);
  EXPECT_TRUE(ts.CheckLegality());
  res = ts.ToProto(&proto);
  EXPECT_TRUE(res);

  EXPECT_EQ(proto.dims(0).size(), 10);
  EXPECT_EQ(proto.dims(1).size(), 8);
  EXPECT_EQ(proto.dims(2).size(), 12);

  google::protobuf::ShutdownProtobufLibrary();
}

TEST(TensorShape, FromProtoWithValid) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  TensorShapeProto proto;
  proto.add_dims()->set_size(10);
  proto.add_dims()->set_size(0);
  proto.add_dims()->set_size(12);

  TensorShape ts;
  bool res = ts.FromProto(proto);
  EXPECT_FALSE(res);
  EXPECT_EQ(ts.NumDims(), 0);
  EXPECT_EQ(ts.NumElements(), 1);

  proto.mutable_dims(1)->set_size(8);
  res = ts.FromProto(proto);
  EXPECT_TRUE(res);
  EXPECT_EQ(ts.NumElements(), 10 * 8 * 12);
  EXPECT_EQ(ts.NumDims(), 3);
  EXPECT_EQ(ts(0), 10);
  EXPECT_EQ(ts.At(1), 8);
  EXPECT_EQ(ts[2], 12);
  EXPECT_TRUE(ts.CheckLegality());
  google::protobuf::ShutdownProtobufLibrary();
}

TEST(TensorShape, FromProtoWithEmpty) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  TensorShapeProto proto;
  TensorShape ts;
  bool res = ts.FromProto(proto);
  EXPECT_TRUE(res);
  EXPECT_EQ(ts.NumDims(), 0);
  EXPECT_EQ(ts.NumElements(), 1);
  EXPECT_TRUE(ts.CheckLegality());
  google::protobuf::ShutdownProtobufLibrary();
}

TEST(TensorShape, ToAndFromProto) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  TensorShape to({10, 11, 12});
  TensorShapeProto proto;
  EXPECT_TRUE(to.ToProto(&proto));

  TensorShape from;
  EXPECT_TRUE(from.FromProto(proto));
  EXPECT_EQ(to, from);

  google::protobuf::ShutdownProtobufLibrary();
}

}  // namespace core
}  // namespace chime
