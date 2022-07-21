# Copyright by 2022.7 chime. All rights reserved.

# cc_library(
#     name = "memory_pool",
#     srcs = ["memory_pool.cc"],
#     hdrs = ["memory_pool.hpp"],
#     deps = ["//chime/core/framework:common"],
#     visibility = [
#         "//visibility:public",
#     ]  
# )

# cc_library(
#     name = "mem_optimizer",
#     hdrs = ["mem_optimizer.h"],
#     srcs = ["mem_optimizer.cc"],
#     deps = ["//chime/core/framework:types",
#             "//chime/core/platform:logging",
#             "//chime/core/platform:macros"],
#     visibility = [
#         "//visibility:public",
#     ]  
# )

# cc_test(
#     name = "mem_optimizer_test",
#     srcs = ["mem_optimizer_test.cc"],
#     deps = [":mem_optimizer",
#             "//chime/core/platform:test"]
# )

# cc_library(
#     name = "pool",
#     srcs = ["pool.cc"],
#     hdrs = ["pool.hpp"],
#     deps = [":mem_optimizer"],
#     visibility = [
#         "//visibility:public",
#     ]  
# )
# cc_test(
#     name = "pool_test",
#     srcs = ["pool_test.cc"],
#     size = "small",
#     deps = [":pool"]
# )

# cc_test(
#     name = "memory_pool_test",
#     srcs = ["memory_pool_test.cc"],
#     size = "small",
#     deps = [":memory_pool"]
# )

cc_library(
    name = "mem",
    hdrs = ["mem.h"],
    visibility = [
        "//visibility:public",
    ]
)

cc_test(
    name = "mem_test",
    srcs = ["mem_test.cc"],
    size = "small",
    deps = [":mem",
            "//chime/core/platform:test",
            "//chime/core/platform/default:port"]
)

cc_library(
    name = "allocator_lib",
    srcs = ["allocator.h",
            "allocator.cc",
            "allocator_registry.h",
            "allocator_registry.cc",
            "tracking_allocator.h",
            "tracking_allocator.cc",
            "cpu_allocator_impl.cc",
            "typed_allocator.h"],
    deps = ["//chime/core/util:optional",
            "//chime/core/platform:macros",
            "//chime/core/platform:logging",
            "//chime/core/framework:types",
            "//chime/core/platform:numa",
            "//chime/core/platform:thread_annotations",
            "//chime/core/platform:mutex",
            "//chime/core/platform:env",
            "//chime/core/memory:mem",
            "//chime/core/framework:type_traits"],
    visibility = [
        "//visibility:public",
    ]
)

cc_test(
    name = "tracking_allocator_test",
    srcs = ["tracking_allocator_test.cc"],
    deps = [":allocator_lib",
            "//chime/core/platform:test",
            "//chime/core/platform/default:env",
            "//chime/core/platform/default:port"]
)

cc_test(
    name = "allocator_test",
    srcs = ["allocator_test.cc"],
    deps = [":allocator_lib",
            "//chime/core/platform:test",
            "//chime/core/platform/default:port",
            "//chime/core/platform/default:env",]
)

