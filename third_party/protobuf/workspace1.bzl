# Copyright by 2022.7 chime. All rights reserved.

load("@rules_proto//proto:repositories.bzl", "rules_proto_dependencies", "rules_proto_toolchains")

def repo():
    # Deprecated: use grpc protobuf instead.
    rules_proto_dependencies()
    rules_proto_toolchains()
