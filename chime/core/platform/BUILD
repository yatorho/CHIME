cc_library(
  name = "threadpool",
  hdrs = ["threadpool.h"],
  srcs = ["threadpool.cc"],
  deps = ["//chime/core/framework:common",
          ":ring_buffer"],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "ring_buffer",
  hdrs = ["ring_buffer.hpp"],
  srcs = ["ring_buffer.cc"],
  deps = [":fixed_queue",
          "//chime/core/framework:common"],
)

cc_library(
  name = "fixed_queue",
  hdrs = ["fixed_queue.hpp"]
)

cc_test(
  name = "ring_buffer_test",
  srcs = ["ring_buffer_test.cc"],
  deps = [":ring_buffer"],
  size = "small"
)
