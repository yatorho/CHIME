// Copyright 2022.5 chime
// author: yatorho

#include "chime/core/framework/tensor_shape.hpp"

#include "chime/core/framework/shape_vec.hpp"

namespace chime {

class TensorShapeTest : public ::testing::Test {};

TEST_F(TensorShapeTest, TestConstruct) {
  { // default constructor
    TensorShape ts;
    EXPECT_EQ(ts.num_elements(), 1);
    EXPECT_EQ(ts.dims(), 0);
  }
  { // TensorShape(const DimVector &dim_vec)
    DimVector dim_vec({1, 2, 3});
    TensorShape ts(dim_vec);
    EXPECT_EQ(ts.num_elements(), 6);
    EXPECT_EQ(ts.dims(), 3);
    EXPECT_EQ(ts.dim_size(1), 2);
  }
  { // TensorShape(DimVector &&dim_vec)
    TensorShape ts(DimVector({3, 4, 5, 6}));
    EXPECT_EQ(ts.num_elements(), 3 * 4 * 5 * 6);
    EXPECT_EQ(ts.dims(), 4);
    EXPECT_EQ(ts.dim_size(2), 5);
  }
  { // TensorShape(const TensorShape &other);
    DimVector dim_vec({1, 2, 3});
    TensorShape ts1(dim_vec);

    TensorShape ts2(ts1);
    EXPECT_EQ(ts2.num_elements(), 6);
    EXPECT_EQ(ts2.dims(), 3);
    EXPECT_EQ(ts1.dim_size(1), 2);
  } 
  { // TensorShape(TensorShape &&other)
    TensorShape ts1((TensorShape &&)TensorShape({1, 2, 3, 4}));
    EXPECT_EQ(ts1.num_elements(), 24);
    EXPECT_EQ(ts1.dims(), 4);
    EXPECT_EQ(ts1.dim_size(3), 4);
  }
}

} // namespace chime