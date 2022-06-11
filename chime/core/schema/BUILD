load("@rules_cc//cc:defs.bzl", "cc_proto_library", "cc_binary")
load("@rules_proto//proto:defs.bzl", "proto_library")


cc_proto_library(
  name = "types_cc_proto",
  deps = [":types_proto"],
  visibility = ["//visibility:public"],
)

proto_library(
  name = "types_proto",
  srcs = ["types.proto"],
)

proto_library(
  name = "tensor_shape_proto",
  srcs = ["tensor_shape.proto"]
)

cc_proto_library(
  name = "tensor_shape_cc_proto",
  deps = [":tensor_shape_proto"],
  visibility = ["//visibility:public"],
)
