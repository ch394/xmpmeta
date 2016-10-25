# Running tests:
#   blaze test third_party/xmpmeta/... --config=asan --compilation_mode=opt
licenses(["notice"])  # Apache 2.0

exports_files(["LICENSE"])

package(
    default_hdrs_check = "loose",
    default_visibility = ["//third_party/xmpmeta:xmpmeta_pkg"],
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

# External-facing targets.

cc_library(
    name = "xmpmeta",
    hdrs = ["//third_party/xmpmeta/includes:xmpmeta-headers"],
    visibility = ["//visibility:public"],
    deps = [
        ":pano_meta_data",
        ":xmpmeta_internal",
    ],
)

# Target for only includes/xmpmeta/pano_meta_data.h
cc_library(
    name = "pano_meta_data",
    hdrs = ["//third_party/xmpmeta/includes:pano_meta_data_header"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "jpeg_io",
    srcs = ["internal/xmpmeta/jpeg_io.cc"],
    hdrs = ["//third_party/xmpmeta/includes:jpeg_io_header"],
    includes = [
        "google3",
        "includes",
    ],
    visibility = [
        "//cityblock/geometry/video",
        "//vr/photos:__subpackages__",
    ],
    deps = [
        "//base",
    ],
)

cc_library(
    name = "xdmlib",
    hdrs = ["//third_party/xmpmeta/includes:xdmlib-headers"],
    visibility = ["//visibility:public"],
    deps = [":xdmlib_internal"],
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

# Package-private, per-library targets.
# There is one target each for xmpmeta, xmpmeta's XML library, and xdmlib.

COMMON_DEPS = [
    "//base",
    "//strings",
    "//third_party/libxml",
]

COMMON_INCLUDES = [
    "google3",
    "includes",
    "internal",
]

# xmpmeta build target
XMPMETA_INTERNAL_SRCS = [
    "internal/xmpmeta/file.cc",
    "internal/xmpmeta/gaudio.cc",
    "internal/xmpmeta/gimage.cc",
    "internal/xmpmeta/gpano.cc",
    "internal/xmpmeta/photo_sphere_writer.cc",
    "internal/xmpmeta/vr_photo_writer.cc",
]

XMPMETA_INTERNAL_HDRS = [
    "internal/xmpmeta/base64.h",
    "internal/xmpmeta/file.h",
]

XMPMETA_INTERNAL_XML_SRCS = [
    "internal/xmpmeta/xml/const.cc",
    "internal/xmpmeta/xml/deserializer_impl.cc",
    "internal/xmpmeta/xml/search.cc",
    "internal/xmpmeta/xml/serializer_impl.cc",
    "internal/xmpmeta/xml/utils.cc",
]

XMPMETA_INTERNAL_XML_HDRS = [
    "internal/xmpmeta/xml/const.h",
    "internal/xmpmeta/xml/deserializer.h",
    "internal/xmpmeta/xml/deserializer_impl.h",
    "internal/xmpmeta/xml/search.h",
    "internal/xmpmeta/xml/serializer.h",
    "internal/xmpmeta/xml/serializer_impl.h",
    "internal/xmpmeta/xml/utils.h",
]

cc_library(
    name = "xmpmeta_internal",
    srcs = XMPMETA_INTERNAL_SRCS,
    hdrs = XMPMETA_INTERNAL_HDRS + [
        "//third_party/xmpmeta/includes:xmpmeta-headers",
    ],
    includes = COMMON_INCLUDES,
    deps = COMMON_DEPS + [":xmpmeta_xml"],
)

# The following standalone targets avoid a dependency cycle
# in the xmpmeta_xml target.
cc_library(
    name = "xmpmeta_base64",
    srcs = ["internal/xmpmeta/base64.cc"],
    hdrs = ["internal/xmpmeta/base64.h"],
    includes = COMMON_INCLUDES,
    deps = COMMON_DEPS,
)

cc_library(
    name = "xmpmeta_file",
    srcs = ["internal/xmpmeta/file.cc"],
    hdrs = ["internal/xmpmeta/file.h"],
    includes = COMMON_INCLUDES,
    deps = COMMON_DEPS,
)

cc_library(
    name = "xmpmeta_xmp_const",
    srcs = ["internal/xmpmeta/xmp_const.cc"],
    hdrs = ["//third_party/xmpmeta/includes:xmpmeta-headers"],
    includes = COMMON_INCLUDES,
)

cc_library(
    name = "xmpmeta_xmp_data",
    srcs = ["internal/xmpmeta/xmp_data.cc"],
    hdrs = ["//third_party/xmpmeta/includes:xmpmeta-headers"],
    includes = COMMON_INCLUDES,
    deps = ["//third_party/libxml"],
)

cc_library(
    name = "xmpmeta_xmp_parser",
    srcs = ["internal/xmpmeta/xmp_parser.cc"],
    hdrs = ["//third_party/xmpmeta/includes:xmpmeta-headers"],
    includes = COMMON_INCLUDES,
    deps = COMMON_DEPS + [
        ":jpeg_io",
        ":xmpmeta_xmp_const",
        ":xmpmeta_xmp_data",
    ],
)

cc_library(
    name = "xmpmeta_xmp_writer",
    srcs = [
        "internal/xmpmeta/md5.cc",
        "internal/xmpmeta/xmp_writer.cc",
    ],
    hdrs = ["//third_party/xmpmeta/includes:xmpmeta-headers"],
    includes = COMMON_INCLUDES,
    deps = COMMON_DEPS + [
        ":xmpmeta_xmp_const",
        ":xmpmeta_xmp_data",
    ],
)

# xmpmeta's internal XML library.
cc_library(
    name = "xmpmeta_xml",
    srcs = XMPMETA_INTERNAL_XML_SRCS,
    hdrs = XMPMETA_INTERNAL_XML_HDRS,
    includes = COMMON_INCLUDES,
    visibility = [
        "//third_party/xmpmeta:xmpmeta_pkg",
        "//vr/geo/earth:__subpackages__",
    ],
    deps = COMMON_DEPS + [
        ":logging",
        ":xmpmeta_base64",
        ":xmpmeta_xmp_parser",
        ":xmpmeta_xmp_writer",
    ],
)

# xdmlib target.
XDMLIB_INTERNAL_SRCS = [
    "internal/xdmlib/audio.cc",
    "internal/xdmlib/camera.cc",
    "internal/xdmlib/camera_pose.cc",
    "internal/xdmlib/cameras.cc",
    "internal/xdmlib/const.cc",
    "internal/xdmlib/device.cc",
    "internal/xdmlib/device_pose.cc",
    "internal/xdmlib/equirect_model.cc",
    "internal/xdmlib/image.cc",
    "internal/xdmlib/point_cloud.cc",
    "internal/xdmlib/profile.cc",
    "internal/xdmlib/profiles.cc",
]

XDMLIB_INTERNAL_HDRS = [
    "internal/xdmlib/const.h",
    "internal/xdmlib/dimension.h",
    "internal/xdmlib/element.h",
    "internal/xdmlib/point.h",
]

cc_library(
    name = "xdmlib_internal",
    srcs = XDMLIB_INTERNAL_SRCS,
    hdrs = XDMLIB_INTERNAL_HDRS + [
        "//third_party/xmpmeta/includes:xdmlib-headers",
    ],
    includes = COMMON_INCLUDES,
    deps = COMMON_DEPS + [
        ":xmpmeta_xml",
    ],
)

# Tests ===============================

# Test helper target.
cc_library(
    name = "test_util",
    testonly = 1,
    srcs = [
        "internal/xmpmeta/test_util.cc",
        "internal/xmpmeta/test_xmp_creator.cc",
    ],
    hdrs = [
        "internal/xmpmeta/test_util.h",
        "internal/xmpmeta/test_xmp_creator.h",
    ],
    copts = ["-DXMPMETA_TEST_SRCDIR_SUFFIX=\\\"/google3/third_party/xmpmeta/testdata/\\\" "],
    includes = COMMON_INCLUDES,
    deps = COMMON_DEPS + [
        ":xmpmeta_file",
        "//testing/base/public:gunit_main",
    ],
)

# Test data.
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

# xmpmeta tests.
[cc_test(
    name = test_name,
    size = "small",
    srcs = ["internal/xmpmeta/" + test_name + ".cc"],
    data = [":xmpmeta-testdata"],
    deps = COMMON_DEPS + [
        ":test_util",
        ":xmpmeta_internal",
    ],
) for test_name in [
    "gaudio_test",
    "gimage_test",
    "gpano_test",
    "jpeg_io_test",
    "photo_sphere_writer_test",
    "vr_photo_writer_test",
    "xmp_parser_test",
    "xmp_writer_test",
]]

# xmpmeta XML tests
[cc_test(
    name = test_name,
    size = "small",
    srcs = ["internal/xmpmeta/xml/" + test_name + ".cc"],
    deps = COMMON_DEPS + [
        ":test_util",
        ":xmpmeta_internal",
        ":xmpmeta_xml",
    ],
) for test_name in [
    "deserializer_impl_test",
    "search_test",
    "serializer_impl_test",
    "utils_test",
]]

# xdmlib tests.
[cc_test(
    name = test_name,
    size = "small",
    srcs = ["internal/xdmlib/" + test_name + ".cc"],
    data = [":xdm_testdata"],
    deps = COMMON_DEPS + [
        ":test_util",
        ":xdmlib_internal",
        ":xmpmeta_internal",
        ":xmpmeta_xml",
    ],
) for test_name in [
    "audio_test",
    "camera_test",
    "camera_pose_test",
    "cameras_test",
    "device_test",
    "device_pose_test",
    "equirect_model_test",
    "image_test",
    "point_cloud_test",
    "profile_test",
    "profiles_test",
]]

# CLIs for XMP parser and writer.
cc_binary(
    name = "xmp_parser_cli",
    srcs = XMPMETA_INTERNAL_SRCS + [
        "internal/xmpmeta/xmp_parser_cli.cc",
    ],
    includes = COMMON_INCLUDES,
    deps = [
        ":xmpmeta_base64",
        ":xmpmeta_internal",
        ":xmpmeta_xml",
        ":xmpmeta_xmp_parser",
    ],
)

cc_binary(
    name = "xmp_writer_cli",
    srcs = XMPMETA_INTERNAL_SRCS + [
        "internal/xmpmeta/xmp_writer_cli.cc",
    ],
    includes = COMMON_INCLUDES,
    deps = COMMON_DEPS + [
        ":xmpmeta_internal",
        ":xmpmeta_xml",
        ":xmpmeta_xmp_writer",
        "//file/base",
        "//file/localfile",
        "//image/wimage",
        "//image/wimage:wimage_file_io",
    ],
)

# Fuzzing support for google3, see go/google3-fuzzing.
load("//security/fuzzing/blaze:cc_fuzz_target.bzl", "cc_fuzz_target")

cc_fuzz_target(
    name = "xmp_parser_fuzzer",
    srcs = XMPMETA_INTERNAL_SRCS + ["internal/xmpmeta/xmp_parser_fuzzer.cc"],
    componentid = 136189,  # VR > Teleportation > Cyclops > Security
    corpus = glob(["testdata/*.jpg"]),
    corpus_cns = ["/namespace/security-fuzz-testing/corpora/xmpmeta_xmp_jpg.ear"],
    deps = COMMON_DEPS + [
        ":xmpmeta_internal",
        ":xmpmeta_xml",
        ":xmpmeta_xmp_parser",
    ],
)
