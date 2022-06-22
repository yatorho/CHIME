cc_library(
  name = "env",
  srcs = ["env.cc"],
  deps = ["//chime/core/platform:env"]
)

cc_test(
  name = "env_test",
  srcs = ["env_test.cc"],
  deps = [":env",
          "//chime/core/platform:task"],
  size = "small"
)
