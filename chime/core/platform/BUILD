# Copyright by 2022.7 chime. All rights reserved.

load("@rules_cc//cc:defs.bzl", "cc_proto_library")
load("@rules_proto//proto:defs.bzl", "proto_library")

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
    deps = [":types",
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

cc_proto_library(
    name = "types_cc_proto",
    deps = [":types_proto"],
    visibility = ["//visibility:public"],
)

proto_library(
    name = "types_proto",
    srcs = ["types.proto"],
)

cc_library(
    name = "types",
    srcs = ["types.h",
            "type_traits.h"],
    deps = [":types_cc_proto",
            "@half//:half",
            "@FP16//:FP16"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "types_test",
    srcs = ["types_test.cc"],
    deps = [":types",
            "//chime/core/platform:test"]
)

proto_library(
    name = "error_codes_proto",
    srcs = ["error_codes.proto"],
)

cc_proto_library(
    name = "error_codes_cc_proto",
    deps = [":error_codes_proto"],
    visibility = ["//visibility:public"],
)
