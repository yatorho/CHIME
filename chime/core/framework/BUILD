# Copyright by 2022.7 chime. All rights reserved.

# cc_library(
#     name = "common",
#     srcs = ["common.cc"],
#     hdrs = ["common.hpp"],
#     deps = ["@com_google_googletest//:gtest_main",
#             "@com_github_google_glog//:glog"],
#     visibility = [
#         "//visibility:public",
#     ]
# )

# cc_library(
#     name = "syncedmem",
#     srcs = ["syncedmem.cc"],
#     hdrs = ["syncedmem.hpp"],
#     deps = [":common",
#             "//chime/core/memory:mem_optimizer"],
# )

# cc_library(
#     name = "basetensor",
#     srcs = ["basetensor.cc"],
#     hdrs = ["basetensor.hpp"],
#     deps = [":common",
#             ":syncedmem",
#             ":math_functions"]
# )

# cc_library(
#     name = "math_functions",
#     srcs = ["math_functions.cc"],
#     hdrs = ["math_functions.hpp"],
#     visibility = ["//visibility:public"],
#     deps = ["//third_party/openblas:openblas",
#             ":common"]
# )

# cc_library(
#     name = "shape",
#     srcs = ["shape.cc"],
#     hdrs = ["shape.hpp"],
#     deps = [":common"]
# )

# cc_test(
#     name = "shape_test",
#     srcs = ["shape_test.cc"],
#     deps = [":shape"]
# )

cc_library(
    name = "types",
    hdrs = ["types.hpp"],
    deps = ["//chime/core/schema:types_cc_proto",
            "//chime/core/platform:logging"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "type_traits",
    srcs = ["type_traits.h"],
    deps = [":types"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "types_test",
    srcs = ["types_test.cc"],
    deps = [":types",
            "//chime/core/platform:test"]
)

# cc_test(
#     name = "math_functions_test",
#     size = "small",
#     srcs = ["math_functions_test.cc"],
#     deps = ["math_functions"],
# )

# cc_test(
#     name = "syncedmem_test",
#     size = "small",
#     srcs = ["syncedmem_test.cc"],
#     deps = [":syncedmem",
#             "//chime/core/memory:pool"],
# )

# cc_test(
#     name = "basetensor_test",
#     size = "small",
#     srcs = ["basetensor_test.cc"],
#     deps = [":basetensor"]
# )

cc_library(
    name = "shape_vec",
    hdrs = ["shape_vec.hpp"],
    deps = [":types",]
)

cc_library(
    name = "tensor_shape",
    hdrs = ["tensor_shape.hpp"],
    srcs = ["tensor_shape.cc"],
    deps = [":shape_vec",
            "//chime/core/util:overflow",
            "//chime/core/schema:tensor_shape_cc_proto",
            "//chime/core/platform:logging"],
)

cc_test(
    name = "tensor_shape_test",
    size = "small",
    srcs = ["tensor_shape_test.cc"],
    deps = [":tensor_shape",
            "//chime/core/platform:test"]
)

# cc_library(
#     name = "tensor",
#     hdrs = ["tensor.h"],
#     srcs = ["tensor.cc"],
#     deps = [":syncedmem",
#             ":tensor_shape",
#             "//chime/core/memory:mem_optimizer",
#             ":type",
#             "//chime/core/schema:tensor_cc_proto"],
#     visibility = ["//visibility:public"],
# )

# cc_test(
#     name = "tensor_test",
#     srcs = ["tensor_test.cc"],
#     size = "small",
#     deps = [":tensor",
#             "//chime/core/memory:pool"]
# )

cc_library(
    name = "device_types",
    srcs = ["device_types.h"],
    visibility = ["//visibility:public"],
)
