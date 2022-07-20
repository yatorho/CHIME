// Copyright 2022.5 chime
// author: yatorho

#include "chime/core/framework/tensor_shape.hpp"

#include "chime/core/framework/shape_vec.hpp"
#include "chime/core/schema/tensor_shape.pb.h"
#include "chime/core/platform/test.hpp"

namespace chime {

class TensorShapeTest : public ::testing::Test {
 public:
  static void test_tensor_shape_operator_assign() {
    TensorShape ts1(DimVector({6, 4}));
    TensorShape ts2(DimVector({3, 4, 4, 3, 2}));
    DimVector *ptr1 = &ts1._dim_vec;
    DimVector *ptr2 = &ts2._dim_vec;
    EXPECT_NE(&ts1._dim_vec, &ts2._dim_vec);

    ts2 = ts1;
    EXPECT_EQ(ptr1, &ts1._dim_vec);
    EXPECT_EQ(ptr2, &ts2._dim_vec);
    EXPECT_NE(ptr1, ptr2);
  }
};

TEST_F(TensorShapeTest, TestConstruct) {
  {  // default constructor
    TensorShape ts;
    EXPECT_EQ(ts.NumElements(), 1);
    EXPECT_EQ(ts.Dims(), 0);
  }
  {  // TensorShape(const DimVector &dim_vec)
    DimVector dim_vec({1, 2, 3});
    TensorShape ts(dim_vec);
    EXPECT_EQ(ts.NumElements(), 6);
    EXPECT_EQ(ts.Dims(), 3);
    EXPECT_EQ(ts.DimSize(1), 2 * sizeof(utens_t));
  }
  {  // TensorShape(DimVector &&dim_vec)
    TensorShape ts(DimVector({3, 4, 5, 6}));
    EXPECT_EQ(ts.NumElements(), 3 * 4 * 5 * 6);
    EXPECT_EQ(ts.Dims(), 4);
    EXPECT_EQ(ts.DimSize(2), 5 * sizeof(utens_t));
  }
  {  // TensorShape(const TensorShape &other);
    DimVector dim_vec({1, 2, 3});
    TensorShape ts1(dim_vec);

    TensorShape ts2(ts1);
    EXPECT_EQ(ts2.NumElements(), 6);
    EXPECT_EQ(ts2.Dims(), 3);
    EXPECT_EQ(ts1.DimSize(1), 2 * sizeof(utens_t));
  }
  {  // TensorShape(TensorShape &&other)
    TensorShape ts1((TensorShape &&) TensorShape({1, 2, 3, 4}));
    EXPECT_EQ(ts1.NumElements(), 24);
    EXPECT_EQ(ts1.Dims(), 4);
    EXPECT_EQ(ts1.DimSize(3), 4 * sizeof(utens_t));
  }
}

TEST_F(TensorShapeTest, ShapeAddDim) {
  TensorShape ts;
  EXPECT_EQ(ts.NumElements(), 1);
  EXPECT_EQ(ts.Dims(), 0);

  ts.AddDim(10);
  EXPECT_EQ(ts.Dims(), 1);
  EXPECT_EQ(ts.NumElements(), 10);

  TensorShape ts1({5, 3, 10});
  ts1.AddDim(6);
  EXPECT_EQ(ts1.Dims(), 4);
  EXPECT_EQ(ts1.NumElements(), 5 * 3 * 10 * 6);
  EXPECT_EQ(ts1.DimAt(2), 10);
  EXPECT_EQ(ts1.DimSize(1), 3 * sizeof(utens_t));
}

TEST_F(TensorShapeTest, ShapeAppendShape) {
  TensorShape ts1;
  TensorShape ts2(DimVector({2, 3, 4}));

  ts1.AppendShape(ts2);
  EXPECT_EQ(ts1.Dims(), 3);
  EXPECT_EQ(ts1.NumElements(), 2 * 3 * 4);
  EXPECT_EQ(ts1.DimAt(2), 4);

  ts1.AppendShape(TensorShape({4}));
  EXPECT_EQ(ts1.Dims(), 4);
  EXPECT_EQ(ts1.NumElements(), 2 * 3 * 4 * 4);
  EXPECT_EQ(ts1.DimAt(3), 4);
}

TEST_F(TensorShapeTest, ShapeInsertDim) {
  TensorShape ts;
  ts.InsertDim(0, 10);
  EXPECT_EQ(ts.Dims(), 1);
  EXPECT_EQ(ts.NumElements(), 10);

  ts.InsertDim(0, 5);
  EXPECT_EQ(ts.Dims(), 2);
  EXPECT_EQ(ts.NumElements(), 5 * 10);

  ts.InsertDim(2, 2);
  EXPECT_EQ(ts.Dims(), 3);
  EXPECT_EQ(ts.NumElements(), 5 * 10 * 2);

  ts.InsertDim(1, 4);
  EXPECT_EQ(ts.Dims(), 4);
  EXPECT_EQ(ts.NumElements(), 5 * 4 * 10 * 2);
}

TEST_F(TensorShapeTest, ShapeSetDim) {
  TensorShape ts({4, 5, 6});
  ts.SetDim(0, 6);
  ts.SetDim(1, 4);
  EXPECT_EQ(ts.Dims(), 3);
  EXPECT_EQ(ts.NumElements(), 6 * 4 * 6);
}

TEST_F(TensorShapeTest, ShapeRemoveDimRange) {
  TensorShape ts(DimVector({3, 4, 5, 6, 7, 8}));
  EXPECT_EQ(ts.Dims(), 6);
  EXPECT_EQ(ts.NumElements(), 3 * 4 * 5 * 6 * 7 * 8);

  ts.RemoveDimRange(2, 4);
  EXPECT_EQ(ts.Dims(), 4);
  EXPECT_EQ(ts.NumElements(), 3 * 4 * 7 * 8);
  EXPECT_EQ(ts.DimAt(2), 7);
}

TEST_F(TensorShapeTest, ShapeRemoveLastDim) {
  TensorShape ts(DimVector({3, 4, 5, 6, 7, 8}));
  EXPECT_EQ(ts.Dims(), 6);
  EXPECT_EQ(ts.NumElements(), 3 * 4 * 5 * 6 * 7 * 8);

  ts.RemoveLastDims(2);
  EXPECT_EQ(ts.Dims(), 4);
  EXPECT_EQ(ts.NumElements(), 3 * 4 * 5 * 6);
  EXPECT_EQ(ts.DimAt(2), 5);
}

TEST_F(TensorShapeTest, ShapeRemoveDim) {
  TensorShape ts(DimVector({3, 4, 5, 6, 7, 8}));
  EXPECT_EQ(ts.Dims(), 6);
  EXPECT_EQ(ts.NumElements(), 3 * 4 * 5 * 6 * 7 * 8);

  ts.RemoveDim(2);
  EXPECT_EQ(ts.Dims(), 5);
  EXPECT_EQ(ts.NumElements(), 3 * 4 * 6 * 7 * 8);
  EXPECT_EQ(ts.DimAt(2), 6);
  EXPECT_EQ(ts[1], 4);
}

TEST_F(TensorShapeTest, ShapeOffSet) {
  TensorShape ts({3, 4});
  EXPECT_EQ(ts.Offset(TensorShape({1, 3})), 7);
  EXPECT_EQ(ts.Offset(TensorShape({0, 2})), 2);
}

TEST_F(TensorShapeTest, ShapeCheckLegality) {
  TensorShape ts0(DimVector({3, 4, 5, 6}));
  EXPECT_TRUE(ts0.CheckLegality());
  ts0.SetDim(2, 0);
  EXPECT_FALSE(ts0.CheckLegality());
  ts0.SetDim(2, 1);
  EXPECT_TRUE(ts0.CheckLegality());
  ts0.AddDim(0);
  EXPECT_FALSE(ts0.CheckLegality());
  ts0.SetDim(4, 2);
  EXPECT_TRUE(ts0.CheckLegality());
  ts0.AppendShape(TensorShape({0, 3}));
  EXPECT_FALSE(ts0.CheckLegality());
  ts0.SetDim(5, 10);
  EXPECT_TRUE(ts0.CheckLegality());

  TensorShape ts1({0, 1, 2});
  EXPECT_FALSE(ts1.CheckLegality());
  ts1.SetDim(0, 4);
  EXPECT_TRUE(ts1.CheckLegality());
}

TEST_F(TensorShapeTest, TestOperatorAssign) {
  TensorShapeTest::test_tensor_shape_operator_assign();
}

TEST_F(TensorShapeTest, TestAsProto) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  TensorShape ts({10, 0, 12});
  TensorShapeProto proto;
  ts.AsProto(&proto);
  EXPECT_EQ(proto.dims(2).size(), 12);

  google::protobuf::ShutdownProtobufLibrary();
}

TEST_F(TensorShapeTest, TestFromProto) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  {
    TensorShape ts;

    TensorShapeProto t_proto;
    TensorShapeProto::Dim *dim = t_proto.add_dims();
    dim->set_size(20);

    dim = t_proto.add_dims();
    dim->set_size(30);

    dim = t_proto.add_dims();
    dim->set_size(40);

    ts.FromProto(t_proto);
    EXPECT_EQ(ts.DimAt(1), 30);
    EXPECT_EQ(ts.NumElements(), 20 * 30 * 40);
    EXPECT_TRUE(ts.CheckLegality());
  }
  {
    TensorShape ts;
    TensorShapeProto proto;
    ts.FromProto(proto);
    EXPECT_TRUE(ts.CheckLegality());
    EXPECT_EQ(ts.NumElements(), 1);
  }
  { /*******  Test as&from proto  *******/

    TensorShape ts({10, 11, 12});
    TensorShapeProto proto;
    ts.AsProto(&proto);

    TensorShape ts1;
    ts1.FromProto(proto);

    EXPECT_TRUE(ts == ts1);
  }

  google::protobuf::ShutdownProtobufLibrary();
}

}  // namespace chime