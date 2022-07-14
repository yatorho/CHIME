load("@rules_proto//proto:repositories.bzl", "rules_proto_dependencies", "rules_proto_toolchains")

def repo():
    rules_proto_dependencies()
    rules_proto_toolchains()
