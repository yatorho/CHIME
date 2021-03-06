# Copyright by 2022.7 chime. All rights reserved.

cc_library(
    name = "threadpool",
    hdrs = ["threadpool.h"],
    srcs = ["threadpool.cc"],
    deps = ["//chime/core/util:optional",
            ":task",
            ":threadpool_impl"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "threadpool_test",
    srcs = ["threadpool_test.cc"],
    deps = [":threadpool",
            "//chime/core/platform/default:env",
            "//chime/core/platform:test"],
    size = "small",
)

cc_library(
    name = "env",
    hdrs = ["env.hpp", "env_time.h"],
    srcs = ["env.cc"],
    deps = ["//chime/core/platform:macros"],
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
            ":mutex",
            "//chime/core/util:fixed_queue",
            "//chime/core/util:algorithm",
            "//chime/core/platform:logging"],
)

cc_test(
    name = "threadpool_impl_test",
    srcs = ["threadpool_impl_test.cc"],
    deps = [":threadpool_impl",
            "//chime/core/platform/default:env",
            ":task",
            "//chime/core/platform:test"],
)

cc_library(
    name = "test",
    hdrs = ["test.hpp"],
    deps = ["@com_google_googletest//:gtest_main"],
    visibility = ["//visibility:public"]
)

cc_library(
    name = "logging",
    hdrs = ["logging.hpp"],
    deps = ["@com_github_google_glog//:glog"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "macros",
    hdrs = ["macros.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "thread_annotations",
    srcs = ["thread_annotations.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "numa",
    srcs = ["numa.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "mutex",
    srcs = ["mutex.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "strcat",
    srcs = ["strcat.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "cpu_info",
    srcs = ["cpu_info.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "sys_info",
    srcs = ["sys_info.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "refcount",
    srcs = ["refcount.h"],
    deps = ["//chime/core/framework:types",
            ":logging",
            ":macros",
            ":mutex",
            ":thread_annotations"],
)

cc_test(
    name = "refcount_test",
    srcs = ["refcount_test.cc"],
    deps = [":refcount",
            ":test",
            "//chime/core/platform/default:env"],
    size = "small",
    visibility = ["//visibility:public"],
)
