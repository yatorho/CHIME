# Copyright by 2022.7 chime. All rights reserved.

# load("//third_party/protobuf:workspace1.bzl", protobuf_deps = "repo")
load("//third_party/grpc:workspace2.bzl", grpc_deps_2 = "repo")

def workspace():

###############################################################################
    # Deprecated: use grpc protobuf instead.
    # protobuf_deps()
###############################################################################

    grpc_deps_2()

ce_workspace3 = workspace
chime_extra_deps2 = workspace
