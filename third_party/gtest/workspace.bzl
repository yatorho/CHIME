load("//third_party:repo.bzl", "ce_http_archive")

def repo():
    ce_http_archive(
        name = "com_google_googletest",
        urls = ["https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip"],
        strip_prefix = "googletest-609281088cfefc76f9d0ce82e1ff6c30cc3591e5",
    )
