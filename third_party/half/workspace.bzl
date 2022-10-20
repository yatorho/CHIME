# Copyright by 2022.8 chime. All rights reserved.

load("//third_party:repo.bzl", "ce_http_archive")

def repo():
    ce_http_archive(
        name = "half",
        urls = ["https://github.com/Oneflow-Inc/half/archive/refs/tags/v2.1.0-fix-cuda-raise.zip"],
        sha256 = "7e6e33c188e63e03b9e137dabec25f9de7460f8684d8dce4d4e5df3aff022c24",
        strip_prefix = "half-2.1.0-fix-cuda-raise",
        build_file = "//third_party/half:half.BUILD",
    )
