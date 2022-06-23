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

cc_library(
  name = "fixed_queue",
  hdrs = ["fixed_queue.hpp"],
  deps = [":container_wrapper"]
)

cc_test(
  name = "fixed_queue_test",
  size = "small",
  srcs = ["fixed_queue_test.cc"],
  deps = [":fixed_queue",
          "//chime/core/framework:common"])

cc_library(
  name = "container_wrapper",
  hdrs = ["container_wrapper.hpp"],
)

cc_library(
  name = "ring_queue",
  hdrs = ["ring_queue.hpp"],
  deps = ["fixed_queue"]
)

cc_test(
  name = "ring_queue_test",
  size = "small",
  srcs = ["ring_queue_test.cc"],
  deps = [":ring_queue",
          "//chime/core/framework:common"]
)
