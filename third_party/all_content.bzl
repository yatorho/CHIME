# Copyright by 2022.7 chime. All rights reserved.

# Group the sources of the library so that CMake or Make rules have access to it
all_content = """filegroup(name = "all", srcs = glob(["**"]), visibility = ["//visibility:public"])"""
