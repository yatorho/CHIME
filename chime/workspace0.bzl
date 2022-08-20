# Copyright by 2022.7 chime. All rights reserved.

load("//third_party:repo.bzl", "ce_http_archive")

def workspace():
    ce_http_archive(
        name = "rules_foreign_cc",
        strip_prefix = "rules_foreign_cc-0.0.9",
        sha256 = "2ee852bed780ac1bf1fb2a4fb845e3705f53b0182d8d3fdb3351bf80a5e3f002",
        url = "https://github.com/bazelbuild/rules_foreign_cc/archive/0.0.9.tar.gz",
    )

    ce_http_archive(
        name = "rules_cc",
        sha256 = "35f2fb4ea0b3e61ad64a369de284e4fbbdcdba71836a5555abb5e194cf119509",
        strip_prefix = "rules_cc-624b5d59dfb45672d4239422fa1e3de1822ee110",
        urls = [
            "https://mirror.bazel.build/github.com/bazelbuild/rules_cc/archive/624b5d59dfb45672d4239422fa1e3de1822ee110.tar.gz",
            "https://github.com/bazelbuild/rules_cc/archive/624b5d59dfb45672d4239422fa1e3de1822ee110.tar.gz",
        ],
    )

ce_workspace0 = workspace
chime_deps = workspace
