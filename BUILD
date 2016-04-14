licenses(["notice"])  # BSD

exports_files(["LICENSE"])

package(
    default_hdrs_check = "loose",
    default_visibility = ["//visibility:public"],
    features = [
        "-layering_check",
        "-parse_headers",
    ],
)

package_group(
    name = "xmpmeta_pkg",
    packages = [
        "//third_party/xmpmeta/...",
    ],
)

filegroup(
    name = "srcs",
    srcs = glob([
        "*.mk",
        "*.cc",
        "*.h",
    ]),
)

filegroup(
    name = "xmpmeta-testdata",
    srcs = glob(["testdata/*.*"]),
)

filegroup(
    name = "xdm_testdata",
    srcs = glob([
        "testdata/xdm/*.txt",
    ]),
)

# xmpmeta headers are accessible via
# #include "third_party/xmpmeta/<header-name>.h"
cc_inc_library(
    name = "xmpmeta",
    hdrs = ["//third_party/xmpmeta/includes:xmpmeta-headers"],
    visibility = ["//visibility:public"],
    deps = [
        ":pano_meta_data",
        "//third_party/xmpmeta/internal/xmpmeta:base64",
        "//third_party/xmpmeta/internal/xmpmeta:gaudio",
        "//third_party/xmpmeta/internal/xmpmeta:gimage",
        "//third_party/xmpmeta/internal/xmpmeta:gpano",
        "//third_party/xmpmeta/internal/xmpmeta:md5",
        "//third_party/xmpmeta/internal/xmpmeta:vr_photo_writer",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_const",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_data",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_parser",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_writer",
    ],
)

# Target for only includes/xmpmeta/pano_meta_data.h
cc_library(
    name = "pano_meta_data",
    hdrs = ["//third_party/xmpmeta/includes:pano_meta_data_header"],
    visibility = ["//visibility:public"],
)

# xdmlib headers are accessible via
# #include "third_party/xmpmeta/xdm/<header-name>.h"
cc_inc_library(
    name = "xdmlib",
    hdrs = ["//third_party/xmpmeta/includes:xdmlib-headers"],
    visibility = ["//visibility:public"],
    deps = ["//third_party/xmpmeta/internal/xdmlib"],
)

cc_library(
    name = "integral_types",
    hdrs = ["google3/base/integral_types.h"],
    deps = [
        "//base",
    ],
)

cc_library(
    name = "logging",
    hdrs = ["google3/glog/logging.h"],
    deps = [
        "//base",
    ],
)
