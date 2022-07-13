
load("@rules_foreign_cc//tools/build_defs:cmake.bzl", "cmake_external")
load("@rules_foreign_cc//tools/build_defs:configure.bzl", "configure_make")


cmake_external(
  name = "openblas",
  cache_entries = {
    "NOFORTRAN": "on",
    "BUILD_WITHOUT_LAPACK": "no",
  },
  lib_source = "@openblas//:all",
  static_libraries = ["libopenblas.a"],
  visibility = ["//visibility:public"],
)

configure_make(
  name = "hwloc",
  lib_source = "@hwloc//:all",
  static_libraries = ["libhwloc.a"],
  autogen = True,
  configure_options = [" --enable-static --disable-shared LDFLAGS='-static'"],
  visibility = ["//visibility:public"],
)

cmake_external(
   name = "eigen",
   # These options help CMake to find prebuilt OpenBLAS, which will be copied into
   # $EXT_BUILD_DEPS/openblas by the cmake_external script
   cache_entries = {
       "BLA_VENDOR": "OpenBLAS",
       "BLAS_LIBRARIES": "$EXT_BUILD_DEPS/openblas/lib/libopenblas.a",
   },
   headers_only = True,
   lib_source = "@eigen//:all",
   # Dependency on other cmake_external rule; can also depend on cc_import, cc_library rules
   deps = [":openblas"],
)

