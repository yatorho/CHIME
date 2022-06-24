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

