// Copyright 2022.5 chime
// author: yatorho

#include "chime/core/framework/tensor_shape.hpp"

#include "chime/core/framework/common.hpp"
#include "chime/core/framework/shape_vec.hpp"
#include "chime/core/schema/tensor_shape.pb.h"

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
    EXPECT_EQ(ts.num_elements(), 1);
    EXPECT_EQ(ts.dims(), 0);
  }
  {  // TensorShape(const DimVector &dim_vec)
    DimVector dim_vec({1, 2, 3});
    TensorShape ts(dim_vec);
    EXPECT_EQ(ts.num_elements(), 6);
    EXPECT_EQ(ts.dims(), 3);
    EXPECT_EQ(ts.dim_size(1), 2 * sizeof(utens_t));
  }
  {  // TensorShape(DimVector &&dim_vec)
    TensorShape ts(DimVector({3, 4, 5, 6}));
    EXPECT_EQ(ts.num_elements(), 3 * 4 * 5 * 6);
    EXPECT_EQ(ts.dims(), 4);
    EXPECT_EQ(ts.dim_size(2), 5 * sizeof(utens_t));
  }
  {  // TensorShape(const TensorShape &other);
    DimVector dim_vec({1, 2, 3});
    TensorShape ts1(dim_vec);

    TensorShape ts2(ts1);
    EXPECT_EQ(ts2.num_elements(), 6);
    EXPECT_EQ(ts2.dims(), 3);
    EXPECT_EQ(ts1.dim_size(1), 2 * sizeof(utens_t));
  }
  {  // TensorShape(TensorShape &&other)
    TensorShape ts1((TensorShape &&) TensorShape({1, 2, 3, 4}));
    EXPECT_EQ(ts1.num_elements(), 24);
    EXPECT_EQ(ts1.dims(), 4);
    EXPECT_EQ(ts1.dim_size(3), 4 * sizeof(utens_t));
  }
}

TEST_F(TensorShapeTest, ShapeAddDim) {
  TensorShape ts;
  EXPECT_EQ(ts.num_elements(), 1);
  EXPECT_EQ(ts.dims(), 0);

  ts.add_dim(10);
  EXPECT_EQ(ts.dims(), 1);
  EXPECT_EQ(ts.num_elements(), 10);

  TensorShape ts1({5, 3, 10});
  ts1.add_dim(6);
  EXPECT_EQ(ts1.dims(), 4);
  EXPECT_EQ(ts1.num_elements(), 5 * 3 * 10 * 6);
  EXPECT_EQ(ts1.dim_at(2), 10);
  EXPECT_EQ(ts1.dim_size(1), 3 * sizeof(utens_t));
}

TEST_F(TensorShapeTest, ShapeAppendShape) {
  TensorShape ts1;
  TensorShape ts2(DimVector({2, 3, 4}));

  ts1.append_shape(ts2);
  EXPECT_EQ(ts1.dims(), 3);
  EXPECT_EQ(ts1.num_elements(), 2 * 3 * 4);
  EXPECT_EQ(ts1.dim_at(2), 4);

  ts1.append_shape(TensorShape({4}));
  EXPECT_EQ(ts1.dims(), 4);
  EXPECT_EQ(ts1.num_elements(), 2 * 3 * 4 * 4);
  EXPECT_EQ(ts1.dim_at(3), 4);
}

TEST_F(TensorShapeTest, ShapeInsertDim) {
  TensorShape ts;
  ts.insert_dim(0, 10);
  EXPECT_EQ(ts.dims(), 1);
  EXPECT_EQ(ts.num_elements(), 10);

  ts.insert_dim(0, 5);
  EXPECT_EQ(ts.dims(), 2);
  EXPECT_EQ(ts.num_elements(), 5 * 10);

  ts.insert_dim(2, 2);
  EXPECT_EQ(ts.dims(), 3);
  EXPECT_EQ(ts.num_elements(), 5 * 10 * 2);

  ts.insert_dim(1, 4);
  EXPECT_EQ(ts.dims(), 4);
  EXPECT_EQ(ts.num_elements(), 5 * 4 * 10 * 2);
}

TEST_F(TensorShapeTest, ShapeSetDim) {
  TensorShape ts({4, 5, 6});
  ts.set_dim(0, 6);
  ts.set_dim(1, 4);
  EXPECT_EQ(ts.dims(), 3);
  EXPECT_EQ(ts.num_elements(), 6 * 4 * 6);
}

TEST_F(TensorShapeTest, ShapeRemoveDimRange) {
  TensorShape ts(DimVector({3, 4, 5, 6, 7, 8}));
  EXPECT_EQ(ts.dims(), 6);
  EXPECT_EQ(ts.num_elements(), 3 * 4 * 5 * 6 * 7 * 8);

  ts.remove_dim_range(2, 4);
  EXPECT_EQ(ts.dims(), 4);
  EXPECT_EQ(ts.num_elements(), 3 * 4 * 7 * 8);
  EXPECT_EQ(ts.dim_at(2), 7);
}

TEST_F(TensorShapeTest, ShapeRemoveLastDim) {
  TensorShape ts(DimVector({3, 4, 5, 6, 7, 8}));
  EXPECT_EQ(ts.dims(), 6);
  EXPECT_EQ(ts.num_elements(), 3 * 4 * 5 * 6 * 7 * 8);

  ts.remove_last_dims(2);
  EXPECT_EQ(ts.dims(), 4);
  EXPECT_EQ(ts.num_elements(), 3 * 4 * 5 * 6);
  EXPECT_EQ(ts.dim_at(2), 5);
}

TEST_F(TensorShapeTest, ShapeRemoveDim) {
  TensorShape ts(DimVector({3, 4, 5, 6, 7, 8}));
  EXPECT_EQ(ts.dims(), 6);
  EXPECT_EQ(ts.num_elements(), 3 * 4 * 5 * 6 * 7 * 8);

  ts.remove_dim(2);
  EXPECT_EQ(ts.dims(), 5);
  EXPECT_EQ(ts.num_elements(), 3 * 4 * 6 * 7 * 8);
  EXPECT_EQ(ts.dim_at(2), 6);
  EXPECT_EQ(ts[1], 4);
}

TEST_F(TensorShapeTest, ShapeOffSet) {
  TensorShape ts({3, 4});
  EXPECT_EQ(ts.offset(TensorShape({1, 3})), 7);
  EXPECT_EQ(ts.offset(TensorShape({0, 2})), 2);
}

TEST_F(TensorShapeTest, ShapeCheckLegality) {
  TensorShape ts0(DimVector({3, 4, 5, 6}));
  EXPECT_TRUE(ts0.check_legality());
  ts0.set_dim(2, 0);
  EXPECT_FALSE(ts0.check_legality());
  ts0.set_dim(2, 1);
  EXPECT_TRUE(ts0.check_legality());
  ts0.add_dim(0);
  EXPECT_FALSE(ts0.check_legality());
  ts0.set_dim(4, 2);
  EXPECT_TRUE(ts0.check_legality());
  ts0.append_shape(TensorShape({0, 3}));
  EXPECT_FALSE(ts0.check_legality());
  ts0.set_dim(5, 10);
  EXPECT_TRUE(ts0.check_legality());

  TensorShape ts1({0, 1, 2});
  EXPECT_FALSE(ts1.check_legality());
  ts1.set_dim(0, 4);
  EXPECT_TRUE(ts1.check_legality());
}

TEST_F(TensorShapeTest, TestOperatorAssign) {
  TensorShapeTest::test_tensor_shape_operator_assign();
}

TEST_F(TensorShapeTest, TestAsProto) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  TensorShape ts({10, 0, 12});
  TensorShapeProto proto;
  ts.as_proto(&proto);
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

    ts.from_proto(t_proto);
    EXPECT_EQ(ts.dim_at(1), 30);
    EXPECT_EQ(ts.num_elements(), 20 * 30 * 40);
    EXPECT_TRUE(ts.check_legality());
  }
  {
    TensorShape ts;
    TensorShapeProto proto;
    ts.from_proto(proto);
    EXPECT_TRUE(ts.check_legality());
    EXPECT_EQ(ts.num_elements(), 1);
  }
  { /*******  Test as&from proto  *******/

    TensorShape ts({10, 11, 12});
    TensorShapeProto proto;
    ts.as_proto(&proto);

    TensorShape ts1;
    ts1.from_proto(proto);

    EXPECT_TRUE(ts == ts1);
  }

  google::protobuf::ShutdownProtobufLibrary();
}

}  // namespace chime