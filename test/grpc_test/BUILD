load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_proto_library")
load("@com_github_grpc_grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")


proto_library(
  name = "hello_proto",
  srcs = ["hello.proto"],
)

cc_proto_library(
  name = "hello_cc_proto",
  deps = [":hello_proto"],
)

cc_grpc_library(
  name = "hello_cc_grpc",
  srcs = [":hello_proto"],
  grpc_only = True,
  deps = [":hello_cc_proto"],
)

cc_binary(
  name = "greeter_client",
  srcs = ["greeter_client.cc"],
  defines = ["BAZEL_BUILD"],
  deps = [":hello_cc_grpc",
          "@com_github_grpc_grpc//:grpc++"]
)

cc_binary(
  name = "greeter_server",
  srcs = ["greeter_server.cc"],
  defines = ["BAZEL_BUILD"],
  deps = [":hello_cc_grpc",
          "@com_github_grpc_grpc//:grpc++"]
)
