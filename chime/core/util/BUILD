cc_library(
  name = "overflow",
  hdrs = ["overflow.h"],
  deps = ["//chime/core/framework:common"],
  visibility = ["//visibility:public"]
)

cc_test(
  name = "overflow_test",
  srcs = ["overflow_test.cc"],
  size = "small",
  deps = [":overflow"]
)

cc_library(
  name = "optional",
  hdrs = ["optional.hpp"],
  deps = ["//chime/core/framework:common"],
  visibility = ["//visibility:public"]
)

cc_test(
  name = "optional_test",
  size = "small",
  srcs = ["optional_test.cc"],
  deps = [":optional"]
)
