load("@rules_foreign_cc//tools/build_defs:configure.bzl", "configure_make")

configure_make(
  name = "hwloc",
  lib_source = "@hwloc//:all",
  static_libraries = ["libhwloc.a"],
  autogen = True,
  configure_options = [" --enable-static --disable-shared LDFLAGS='-static'"],
  visibility = ["//visibility:public"],
)

