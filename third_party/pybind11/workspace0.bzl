# Copyright by 2022.8 chime. All rights reserved.

load("//third_party:repo.bzl", "ce_http_archive")

def repo():
    ce_http_archive(
        name = "pybind11_bazel",
        strip_prefix = "pybind11_bazel-72cbbf1fbc830e487e3012862b7b720001b70672",
        sha256 = "fec6281e4109115c5157ca720b8fe20c8f655f773172290b03f57353c11869c2",
        urls = ["https://github.com/pybind/pybind11_bazel/archive/72cbbf1fbc830e487e3012862b7b720001b70672.zip"],
    )

    ce_http_archive(
        name = "pybind11",
        build_file = "@pybind11_bazel//:pybind11.BUILD",
        strip_prefix = "pybind11-2.6.2",
        sha256 = "8ff2fff22df038f5cd02cea8af56622bc67f5b64534f1b83b9f133b8366acff2",
        urls = ["https://github.com/pybind/pybind11/archive/v2.6.2.tar.gz"],
    )
