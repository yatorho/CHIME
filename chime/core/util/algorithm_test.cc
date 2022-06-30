// Copyright by 2022.6 chime
// author: yatorho

#include "chime/core/util/algorithm.hpp"

#include "chime/core/platform/test.hpp"

namespace chime {
namespace algorithm {

TEST(Algorithm, TestAccumulate) {
  std::vector<int> v = {1, 2, 3, 4, 5};
  EXPECT_EQ(accumulate<int>(v.begin(), v.end(), 0), 15);
}

TEST(Algorithm, TestAccumulateWithIntPointer) {
  std::vector<int> v = {1, 2, 3, 4, 5};
  int *p = &v[0];
  EXPECT_EQ(accumulate<int>(p, p + v.size(), 0), 15);
}

}  // namespace algorithm
}  // namespace chime
