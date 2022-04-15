cc_library(
  name = "common",
  srcs = ["common.cc"],
  hdrs = ["common.hpp"],
  deps = ["@com_google_googletest//:gtest_main",
          "@com_github_google_glog//:glog"],
  visibility = [
      "//visibility:public",
  ]
)

cc_library(
  name = "syncedmem",
  srcs = ["syncedmem.cc"],
  hdrs = ["syncedmem.hpp"],
  deps = [":common"],
)

cc_library(
  name = "basetensor",
  srcs = ["basetensor.cc"],
  hdrs = ["basetensor.hpp"],
  deps = [":common",
          ":syncedmem",
          ":math_functions"]
)

cc_library(
  name = "math_functions",
  srcs = ["math_functions.cc"],
  hdrs = ["math_functions.hpp"],
  visibility = ["//visibility:public"],
  deps = ["//:openblas",
          ":common"]
)

cc_test(
  name = "math_functions_test",
  size = "small",
  srcs = ["math_functions_test.cc"],
  deps = ["math_functions"],
)

cc_test(
  name = "syncedmem_test",
  size = "small",
  srcs = ["syncedmem_test.cc"],
  deps = [":syncedmem"],
)

cc_test(
  name = "basetensor_test",
  size = "small",
  srcs = ["basetensor_test.cc"],
  deps = [":basetensor"]
)

