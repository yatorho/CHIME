# Copyright by 2022.7 chime. All rights reserved.

load("@rules_foreign_cc//:workspace_definitions.bzl", "rules_foreign_cc_dependencies")
load("@rules_cc//cc:repositories.bzl", "rules_cc_dependencies")
load("//third_party/grpc:workspace0.bzl", grpc = "repo")

def workspace():
    rules_foreign_cc_dependencies()
    rules_cc_dependencies()
    grpc()  # Make sure the grpc before grpc_deps_1 in workspace2.
    
ce_workspace1 = workspace
chime_extra_deps0 = workspace
