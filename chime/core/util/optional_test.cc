// Copyright by 2022.6 chime
// author: yatorho

#include "chime/core/util/optional.hpp"

#include "chime/core/platform/test.hpp"

namespace chime {

struct Object {
  int x;
  int y;
  int z;
};

TEST(Optional, TestOperatorBool) {
  Optional<int> opt_int;
  EXPECT_FALSE(opt_int);
  opt_int = 2;
  EXPECT_TRUE(opt_int);
}

TEST(Optional, TestDereference) {
  Optional<Object> opt;
  EXPECT_FALSE(opt);
  Object obj = {3, 4, 5};
  opt = obj;
  EXPECT_TRUE(opt);
  (*opt).x = 6;
  EXPECT_EQ(opt.value().x, 6);
  EXPECT_EQ(opt->x, 6);
}

TEST(Optional, TestMoveConstructor) {
  Optional<Object> opt(Object({2, 3, 4}));
  EXPECT_EQ(opt->x, 2);
  opt = Object({3, 4, 5});
  EXPECT_EQ(opt->x, 3);
  Object obj = Object({4, 5, 6});
  opt = std::move(obj);
  EXPECT_EQ(opt->x, 4);
}

TEST(Optional, TestOperatorAssign) {
  Optional<Object> opt1(Object({2, 3, 4}));

  auto opt2 = opt1;
  EXPECT_EQ(opt2->x, 2);
  opt1->x = 3;
  opt2 = std::move(opt1);
  EXPECT_EQ(opt2->x, 3);
}

TEST(Optional, TestNullOpt) {
  Optional<int> opt = nullopt;
  EXPECT_FALSE(opt);

  Optional<int> opt2 = 3;
  EXPECT_TRUE(opt2);
  opt2 = nullopt;
  EXPECT_FALSE(opt2);
}

}  // namespace chime
