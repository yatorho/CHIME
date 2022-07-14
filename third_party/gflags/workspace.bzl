# Copyright by 2022.7 chime. All rights reserved.

load("//third_party:repo.bzl", "ce_git_archive")

def repo():
    ce_git_archive(
        name = "com_github_gflags_gflags",
        remote = "https://github.com/gflags/gflags.git",
        commit = "e171aa2d15ed9eb17054558e0b3a6a413bb01067", 
        shallow_since = "1541971260 +0000"
    )
