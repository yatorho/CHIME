# Copyright by 2022.7 chime. All rights reserved.

load("@rules_foreign_cc//:workspace_definitions.bzl", "rules_foreign_cc_dependencies")
load("@rules_cc//cc:repositories.bzl", "rules_cc_dependencies")


def workspace():
    rules_foreign_cc_dependencies()
    rules_cc_dependencies()
    
ce_workspace1 = workspace
