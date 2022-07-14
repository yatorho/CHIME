# Copyright by 2022.7 chime. All rights reserved.

load("//third_party:repo.bzl", "ce_http_archive")

def repo():
    ce_http_archive(
        name = "com_google_googletest",
        urls = ["https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip"],
        strip_prefix = "googletest-609281088cfefc76f9d0ce82e1ff6c30cc3591e5",
        sha256 = "5cf189eb6847b4f8fc603a3ffff3b0771c08eec7dd4bd961bfd45477dd13eb73",
    )
