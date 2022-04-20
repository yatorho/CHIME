cc_library(
  name = "memory_pool",
  srcs = ["memory_pool.cc"],
  hdrs = ["memory_pool.hpp"],
  deps = ["//chime/core/framework:common"],
  visibility = [
      "//visibility:public",
  ]  
)

cc_library(
  name = "mem_optimizer",
  hdrs = ["mem_optimizer.h"],
  srcs = ["mem_optimizer.cc"],
  deps = ["//chime/core/framework:common"],
  visibility = [
      "//visibility:public",
  ]  
)

cc_library(
  name = "pool",
  srcs = ["pool.cc"],
  hdrs = ["pool.hpp"],
  deps = [":mem_optimizer"],
  visibility = [
      "//visibility:public",
  ]  
)
cc_test(
  name = "pool_test",
  srcs = ["pool_test.cc"],
  size = "small",
  deps = [":pool"]
)

cc_test(
  name = "memory_pool_test",
  srcs = ["memory_pool_test.cc"],
  size = "small",
  deps = [":memory_pool"]
)
