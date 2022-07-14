# Copyright by 2022.7 chime. All rights reserved.

workspace(name = "org_chime")


load("@//chime:workspace0.bzl", "ce_workspace0")  # workspace0 must be loaded first

ce_workspace0()

load("@//chime:workspace1.bzl", "ce_workspace1")

ce_workspace1()

load("@//chime:workspace2.bzl", "ce_workspace2")  # workspace2 must before workspace3

ce_workspace2()

load("@//chime:workspace3.bzl", "ce_workspace3")

ce_workspace3()
