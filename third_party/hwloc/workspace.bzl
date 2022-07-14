load("//third_party:repo.bzl", "ce_http_archive")
load("//third_party:all_content.bzl", "all_content")


def repo():
    ce_http_archive(
        name = "hwloc",
        urls = ["https://github.com/open-mpi/hwloc/releases/download/hwloc-2.1.0/hwloc-2.1.0.tar.gz"],
        strip_prefix = "hwloc-2.1.0",
        sha256 = "1fb8cc1438de548e16ec3bb9e4b2abb9f7ce5656f71c0906583819fcfa8c2031",
        build_file_content = all_content
    )
