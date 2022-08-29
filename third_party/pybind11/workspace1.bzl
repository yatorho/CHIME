# Copyright by 2022.8 chime. All rights reserved.

load("@pybind11_bazel//:python_configure.bzl", "python_configure")

def repo():
    python_configure(name = "local_config_python")
