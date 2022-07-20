# Copyright by 2022.7 chime. All rights reserved.

cc_library(
    name = "env",
    srcs = ["env.cc", "env_time.cc"],
    deps = ["//chime/core/platform:env",
            "//chime/core/platform:logging"],
    visibility = ["//visibility:public"]
)

cc_test(
    name = "env_test",
    srcs = ["env_test.cc"],
    deps = [":env",
            "//chime/core/platform:task",
            "//chime/core/platform:test",
            "//chime/core/framework:types"],
    size = "small",
)

cc_library(
    name = "port",
    srcs = ["port.cc"],
    deps = ["//chime/core/platform:cpu_info",
            "//chime/core/platform:logging",
            "//chime/core/platform:numa",
            "//chime/core/platform:sys_info",
            "//chime/core/memory:mem",
            "//third_party/hwloc:hwloc"],
    visibility = ["//visibility:public"]
)

cc_test(
    name = "port_test",
    srcs = ["port_test.cc"],
    deps = [":port",
            "//chime/core/platform:test"]
)

