load("//third_party:repo.bzl", "ce_http_archive")
load("//third_party/hwloc:workspace.bzl", hwloc = "repo")
load("//third_party/openblas:workspace.bzl", openblas = "repo")
load("//third_party/gtest:workspace.bzl", gtest = "repo")
load("//third_party/gflags:workspace.bzl", gflags = "repo")
load("//third_party/glog:workspace.bzl", glog = "repo")
load("//third_party/protobuf:workspace0.bzl", protobuf = "repo")

def workspace():
    hwloc()
    openblas()
    gtest()
    gflags()
    glog()
    protobuf()  # Make sure protobuf before protobuf_deps in workspace3

ce_workspace2 = workspace
