load("//third_party/protobuf:workspace1.bzl", protobuf_deps = "repo")

def workspace():
    protobuf_deps()

ce_workspace3 = workspace