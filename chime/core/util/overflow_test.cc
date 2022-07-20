// Copyright 2022.6 chime
// author: yatorho

#include "chime/core/util/overflow.h"

#include <cstdint>
#include <limits>

#include "chime/core/platform/test.hpp"

namespace chime {

TEST(OverflowTest, Negative) {
  const int64_t negatives[] = {-1, std::numeric_limits<int64_t>::min()};

  for (const int64_t n : negatives) {
    EXPECT_LT(MultiplyWithoutOverflow(n, 0), 0);
    EXPECT_LT(MultiplyWithoutOverflow(0, n), 0);
    EXPECT_LT(MultiplyWithoutOverflow(n, n), 0);
  }

  EXPECT_GT(MultiplyWithoutOverflow(10000, 10000), 0);

  EXPECT_LT(MultiplyWithoutOverflow(2ll << 31, 2ll << 31), 0);
  EXPECT_GT(MultiplyWithoutOverflow(2ll << 30, 2ll << 30), 0);
}
}  // namespace chime
