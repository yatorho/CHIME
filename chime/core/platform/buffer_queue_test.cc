// Copyright by 2022.6 chime
// author: yatorho

#include "chime/core/platform/buffer_queue.hpp"


namespace chime {
namespace platform {

TEST(Buffer, Test) {
  Buffer<int> buffer;
  EXPECT_EQ(buffer.size(), 0);
  EXPECT_EQ(buffer.is_empty(), true);
  buffer.add_element(new int(1));
  EXPECT_EQ(buffer.size(), 1);
  EXPECT_EQ(buffer.is_empty(), false);
  int *element;
  EXPECT_EQ(buffer.get_element(&element), true);
  EXPECT_EQ(*element, 1);
  EXPECT_EQ(buffer.size(), 0);
  EXPECT_EQ(buffer.is_empty(), true);
}

}  // namespace platform
}  // namespace chime
