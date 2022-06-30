cc_library(
  name = "overflow",
  hdrs = ["overflow.h"],
  deps = ["//chime/core/framework:types"],
  visibility = ["//visibility:public"]
)

cc_test(
  name = "overflow_test",
  srcs = ["overflow_test.cc"],
  size = "small",
  deps = [":overflow",
          "//chime/core/platform:test"]
)

cc_library(
  name = "optional",
  hdrs = ["optional.hpp"],
  deps = ["//chime/core/platform:logging"],
  visibility = ["//visibility:public"]
)

cc_test(
  name = "optional_test",
  size = "small",
  srcs = ["optional_test.cc"],
  deps = [":optional",
          "//chime/core/platform:test"]
)

cc_library(
  name = "fixed_queue",
  hdrs = ["fixed_queue.hpp"],
  deps = [":container_wrapper"],
  visibility = ["//visibility:public"]
)

cc_test(
  name = "fixed_queue_test",
  size = "small",
  srcs = ["fixed_queue_test.cc"],
  deps = [":fixed_queue",
          "//chime/core/platform:test"])

cc_library(
  name = "container_wrapper",
  hdrs = ["container_wrapper.hpp"],
  visibility = ["//visibility:public"]
)

cc_library(
  name = "ring_queue",
  hdrs = ["ring_queue.hpp"],
  deps = ["fixed_queue"],
  visibility = ["//visibility:public"]
)

cc_test(
  name = "ring_queue_test",
  size = "small",
  srcs = ["ring_queue_test.cc"],
  deps = [":ring_queue",
          "//chime/core/platform/default:env",
          "//chime/core/platform:test"]
)

cc_library(
  name = "algorithm",
  hdrs = ["algorithm.hpp"],
  visibility = ["//visibility:public"]
)

cc_test(
  name = "algorithm_test",
  size = "small",
  srcs = ["algorithm_test.cc"],
  deps = [":algorithm",
          "//chime/core/platform:test"]
)
