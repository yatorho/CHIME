cc_library(
  name = "memory_pool",
  srcs = ["memory_pool.cc"],
  hdrs = ["memory_pool.hpp"],
  deps = ["//chime/core/framework:common"],
  visibility = [
      "//visibility:public",
  ]  
)

cc_test(
  name = "memory_pool_test",
  srcs = ["memory_pool_test.cc"],
  size = "small",
  deps = [":memory_pool"]
)
