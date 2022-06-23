cc_library(
  name = "env",
  srcs = ["env.cc"],
  deps = ["//chime/core/platform:env"],
  visibility = ["//visibility:public"]
)

cc_test(
  name = "env_test",
  srcs = ["env_test.cc"],
  deps = [":env",
          "//chime/core/platform:task"],
  size = "small"
)
