# Copyright by 2022.7 chime. All rights reserved.

load("//third_party/protobuf:workspace1.bzl", protobuf_deps = "repo")

def workspace():
    protobuf_deps()

ce_workspace3 = workspace
