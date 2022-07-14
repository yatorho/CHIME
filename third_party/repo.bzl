load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

def ce_http_archive(name,  **kwargs):  # just a wrapper for http_archive.
    http_archive(
      name = name,
      **kwargs,
    )

def ce_git_archive(name,  **kwargs):  # just a wrapper for git_repository.
    git_repository(
      name = name,
      **kwargs,
    )
