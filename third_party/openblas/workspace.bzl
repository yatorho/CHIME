# Copyright by 2022.7 chime. All rights reserved.

load("//third_party:repo.bzl", "ce_http_archive")
load("//third_party:all_content.bzl", "all_content")

def repo():
    ce_http_archive(
        name = "openblas",
        build_file_content = all_content,
        sha256 = "30a99dec977594b387a17f49904523e6bc8dd88bd247266e83485803759e4bbe",
        strip_prefix = "OpenBLAS-0.3.15",
        urls = ["https://github.com/xianyi/OpenBLAS/archive/v0.3.15.tar.gz"],
    )
