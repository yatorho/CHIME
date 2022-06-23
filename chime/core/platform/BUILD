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
  deps = [":ring_buffer",
          "//chime/core/platform/default:env"],
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

cc_library(
  name = "buffer_queue",
  hdrs = ["buffer_queue.hpp"],
  deps = ["fixed_queue",
          "//chime/core/framework:common"],
  visibility = ["//visibility:public"]
)

cc_test(
  name = "buffer_queue_test",
  srcs = ["buffer_queue_test.cc"],
  deps = [":buffer_queue"]
)

