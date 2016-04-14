licenses(["notice"])  # BSD

package(
    default_visibility = ["//third_party/xmpmeta:xmpmeta_pkg"],
    features = [
        "-parse_headers",
        "no_layering_check",
    ],
)

# Targets

cc_library(
    name = "base64",
    srcs = ["base64.cc"],
    hdrs = ["base64.h"],
    includes = [
        "../",
        "../../google3",
        "../../internal",
    ],
    deps = [
        "//strings",  # For testing in Blaze
        "//third_party/xmpmeta/internal/xmpmeta/external/strings",
    ],
)

cc_library(
    name = "vr_photo_writer",
    srcs = ["vr_photo_writer.cc"],
    hdrs = ["//third_party/xmpmeta/includes:vr_photo_writer_header"],
    includes = [
        "../",
        "../../google3",
        "../../includes",
        "../../internal",
    ],
    deps = [
        ":gaudio",
        ":gimage",
        ":gpano",
        "//base",
        "//third_party/libxml",
        "//third_party/xmpmeta/internal/xmpmeta/xml:const",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)

cc_library(
    name = "file",
    srcs = ["file.cc"],
    hdrs = ["file.h"],
    includes = [
        "../",
        "../../google3",
        "../../internal",
    ],
    deps = [
        "//base",  # for logging
    ],
)

cc_library(
    name = "gaudio",
    srcs = ["gaudio.cc"],
    hdrs = ["//third_party/xmpmeta/includes:gaudio_header"],
    includes = [
        "../",
        "../../google3",
        "../../includes",
        "../../internal",
    ],
    deps = [
        ":base64",
        ":xmp_parser",
        "//base",
        "//third_party/xmpmeta/internal/xmpmeta/xml:const",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)

cc_library(
    name = "gimage",
    srcs = ["gimage.cc"],
    hdrs = ["//third_party/xmpmeta/includes:gimage_header"],
    includes = [
        "../",
        "../../google3",
        "../../includes",
        "../../internal",
    ],
    deps = [
        ":xmp_parser",
        "//base",
        "//third_party/xmpmeta/internal/xmpmeta/xml:const",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)

cc_library(
    name = "gpano",
    srcs = ["gpano.cc"],
    hdrs = ["//third_party/xmpmeta/includes:gpano_header"],
    includes = [
        "../",
        "../../google3",
        "../../includes",
        "../../internal",
    ],
    deps = [
        ":xmp_parser",
        "//base",
        "//third_party/xmpmeta/internal/xmpmeta/xml:const",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)

cc_library(
    name = "jpeg_io",
    srcs = ["jpeg_io.cc"],
    hdrs = ["jpeg_io.h"],
    includes = [
        "../",
        "../../google3",
        "../../internal",
    ],
    deps = [
        "//strings",  # For testing in Blaze.
        "//third_party/xmpmeta/internal/xmpmeta/external/strings",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)

cc_library(
    name = "md5",
    srcs = ["md5.cc"],
    hdrs = ["//third_party/xmpmeta/includes:md5_header"],
    includes = [
        "../",
        "../../includes",
        "external",
        "internal",
    ],
    linkopts = select({
        "//tools/cc_target_os:windows": ["-Wl,advapi32.lib"],
        "//conditions:default": [],
    }),
    deps = [
        "//strings",  # For testing in Blaze.
        "//third_party/xmpmeta/internal/xmpmeta/external/strings",
    ],
)

cc_library(
    name = "xmp_const",
    srcs = ["xmp_const.cc"],
    hdrs = ["//third_party/xmpmeta/includes:xmp_const_header"],
    includes = ["../../includes"],
    deps = [
        "//strings",
        "//third_party/libxml",
    ],
)

cc_library(
    name = "xmp_data",
    srcs = ["xmp_data.cc"],
    hdrs = ["//third_party/xmpmeta/includes:xmp_data_header"],
    includes = ["../../includes"],
    deps = [
        "//base",
        "//third_party/libxml",
    ],
)

cc_library(
    name = "xmp_parser",
    srcs = ["xmp_parser.cc"],
    hdrs = ["//third_party/xmpmeta/includes:xmp_parser_header"],
    copts = [
        "-fexceptions",
    ],
    includes = [
        "../",
        "../../google3",
        "../../includes",
        "../../internal",
    ],
    deps = [
        ":base64",
        ":jpeg_io",
        ":xmp_const",
        ":xmp_data",
        "//base",
        "//third_party/xmpmeta/internal/xmpmeta/external/strings",
        "//third_party/xmpmeta/internal/xmpmeta/xml:const",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)

cc_library(
    name = "xmp_writer",
    srcs = ["xmp_writer.cc"],
    hdrs = ["//third_party/xmpmeta/includes:xmp_writer_header"],
    copts = [
        "-fexceptions",
    ],
    includes = ["../../includes"],
    deps = [
        ":jpeg_io",
        ":md5",
        ":xmp_const",
        ":xmp_data",
        "//base",
        "//third_party/xmpmeta/internal/xmpmeta/external/strings",
        "//third_party/xmpmeta/internal/xmpmeta/xml:const",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)

cc_library(
    name = "test_util",
    testonly = 1,
    srcs = ["test_util.cc"],
    hdrs = ["test_util.h"],
    copts = ["-DXMPMETA_TEST_SRCDIR_SUFFIX=\\\"/google3/third_party/xmpmeta/testdata/\\\" "],
    includes = [
        "../",
        "../../google3",
    ],
    deps = [
        ":file",
        "//base",  # for logging
        "//testing/base/public:gunit_main",
    ],
)

cc_library(
    name = "test_xmp_creator",
    srcs = ["test_xmp_creator.cc"],
    hdrs = ["test_xmp_creator.h"],
    copts = [
        "-fexceptions",
    ],
    deps = [
        ":file",
        ":xmp_data",
        "//base",
        "//file/base",
        "//strings",
    ],
)

cc_test(
    name = "base64_test",
    srcs = ["base64_test.cc"],
    includes = [
        "../",
        "../../google3",
        "../../includes",
    ],
    deps = [
        ":base64",
        "//testing/base/public:gunit_main",
    ],
)

cc_test(
    name = "vr_photo_writer_test",
    srcs = ["vr_photo_writer_test.cc"],
    data = ["//third_party/xmpmeta:xmpmeta-testdata"],
    includes = [
        "../",
        "../../google3",
        "../../includes",
    ],
    deps = [
        ":file",
        ":test_util",
        ":vr_photo_writer",
        ":xmp_parser",
        ":xmp_writer",
        "//base",
        "//file/base",
        "//testing/base/public:gunit_main",
        "//third_party/xmpmeta:pano_meta_data",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
        "//util/task:status",
    ],
)

cc_test(
    name = "gaudio_test",
    srcs = ["gaudio_test.cc"],
    data = ["//third_party/xmpmeta:xmpmeta-testdata"],
    includes = [
        "../",
        "../../google3",
        "../../includes",
        "../../internal",
    ],
    deps = [
        ":file",
        ":gaudio",
        ":test_util",
        ":xmp_const",
        ":xmp_data",
        ":xmp_writer",
        "//file/base",
        "//testing/base/public:gunit_main",
        "//third_party/libxml",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)

cc_test(
    name = "gimage_test",
    srcs = ["gimage_test.cc"],
    data = ["//third_party/xmpmeta:xmpmeta-testdata"],
    includes = [
        "../",
        "../../google3",
        "../../includes",
        "../../internal",
    ],
    deps = [
        ":file",
        ":gimage",
        ":test_util",
        ":xmp_writer",
        "//file/base",
        "//testing/base/public:gunit_main",
        "//third_party/libxml",
        "//third_party/xmpmeta/internal/xmpmeta/xml:const",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)

cc_test(
    name = "gpano_test",
    srcs = ["gpano_test.cc"],
    data = ["//third_party/xmpmeta:xmpmeta-testdata"],
    includes = [
        "../",
        "../../google3",
        "../../includes",
        "../../internal",
    ],
    deps = [
        ":gpano",
        ":test_util",
        ":xmp_writer",
        "//file/base",
        "//testing/base/public:gunit_main",
        "//third_party/libxml",
        "//third_party/xmpmeta:pano_meta_data",
        "//third_party/xmpmeta/internal/xmpmeta/xml:const",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)

cc_test(
    name = "jpeg_io_test",
    srcs = ["jpeg_io_test.cc"],
    data = ["//third_party/xmpmeta:xmpmeta-testdata"],
    includes = [
        "../",
        "../../google3",
        "../../includes",
        "../../internal",
    ],
    deps = [
        ":file",
        ":jpeg_io",
        ":test_util",
        ":test_xmp_creator",
        "//base",
        "//file/base",
        "//testing/base/public:gunit_main",
        "//util/task:status",
    ],
)

cc_test(
    name = "md5_test",
    srcs = ["md5_test.cc"],
    includes = [
        "../",
        "../../google3",
        "../../includes",
        "../../internal",
    ],
    deps = [
        ":md5",
        "//testing/base/public:gunit_main",
    ],
)

cc_test(
    name = "xmp_parser_test",
    srcs = ["xmp_parser_test.cc"],
    includes = [
        "../",
        "../../google3",
        "../../includes",
        "../../internal",
    ],
    deps = [
        ":test_util",
        ":test_xmp_creator",
        ":xmp_parser",
        "//base",
        "//third_party/xmpmeta/internal/xmpmeta/xml:const",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:search",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)

cc_test(
    name = "xmp_writer_test",
    srcs = ["xmp_writer_test.cc"],
    data = ["//third_party/xmpmeta:xmpmeta-testdata"],
    includes = [
        "../",
        "../../google3",
        "../../includes",
    ],
    deps = [
        ":file",
        ":test_util",
        ":test_xmp_creator",
        ":xmp_const",
        ":xmp_parser",
        ":xmp_writer",
        "//base",
        "//third_party/xmpmeta/internal/xmpmeta/xml:const",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
        "//util/task:status",
    ],
)

cc_binary(
    name = "xmp_parser_cli",
    srcs = ["xmp_parser_cli.cc"],
    includes = ["../../includes"],
    deps = [
        ":file",
        ":gaudio",
        ":gimage",
        ":gpano",
        ":xmp_data",
        ":xmp_parser",
        "//third_party/xmpmeta:pano_meta_data",
        "//third_party/xmpmeta/internal/xmpmeta/xml:const",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)

cc_binary(
    name = "xmp_writer_cli",
    srcs = ["xmp_writer_cli.cc"],
    includes = [
        "../../includes",
    ],
    deps = [
        ":gaudio",
        ":gimage",
        ":gpano",
        ":vr_photo_writer",
        ":xmp_writer",
        "//base",
        "//file/base",
        "//file/localfile",
        "//image/wimage",
        "//image/wimage:wimage_file_io",
        "//strings",
        "//third_party/xmpmeta:pano_meta_data",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)
