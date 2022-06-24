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

cc_library(
  name = "threadpool_impl",
  hdrs = ["threadpool_impl.h"],
  srcs = ["threadpool_impl.cc"],
  deps = ["//chime/core/util:container_wrapper",
          ":env",
          "//chime/core/util:fixed_queue",
          "//chime/core/util:algorithm"],
)

cc_test(
  name = "threadpool_impl_test",
  srcs = ["threadpool_impl_test.cc"],
  deps = [":threadpool_impl",
          "//chime/core/platform/default:env",
          ":task"],
)

