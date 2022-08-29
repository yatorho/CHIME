# Copyright by 2022.7 chime. All rights reserved.

load("//third_party:repo.bzl", "ce_http_archive")
load("//third_party/hwloc:workspace.bzl", hwloc = "repo")
load("//third_party/openblas:workspace.bzl", openblas = "repo")
load("//third_party/gtest:workspace.bzl", gtest = "repo")
load("//third_party/gflags:workspace.bzl", gflags = "repo")
load("//third_party/glog:workspace.bzl", glog = "repo")
# load("//third_party/protobuf:workspace0.bzl", protobuf = "repo")
load("//third_party/grpc:workspace1.bzl", grpc_deps_1 = "repo")
load("//third_party/pybind11:workspace0.bzl", pybind11 = "repo")

def workspace():
    hwloc()
    openblas()
    gtest()
    gflags()
    glog()

###############################################################################
    # Deprecated: use grpc protobuf instead.
    # protobuf()  # Make sure protobuf before protobuf_deps in workspace3.
###############################################################################
    grpc_deps_1()  # Make sure grpc_deps_1 before grpc_deps_2 in workspace3.
    pybind11()

ce_workspace2 = workspace
chime_extra_deps1 = workspace
