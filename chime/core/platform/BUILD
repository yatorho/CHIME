cc_library(
  name = "threadpool",
  hdrs = ["threadpool.h"],
  srcs = ["threadpool.cc"],
  deps = ["//chime/core/framework:common",
          ":ring_buffer",
          "//chime/core/util:optional",
          ":task"],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "ring_buffer",
  hdrs = ["ring_buffer.hpp"],
  srcs = ["ring_buffer.cc"],
  deps = [":fixed_queue",
          ":env",
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

cc_library(
  name = "env",
  hdrs = ["env.hpp"],
  srcs = ["env.cc"],
  deps = ["//chime/core/framework:common"],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "task",
  hdrs = ["task.hpp"],
  visibility = ["//visibility:public"]
)

