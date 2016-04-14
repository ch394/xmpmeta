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

filegroup(
    name = "srcs",
    srcs = glob([
        "*.mk",
        "*.cc",
        "*.h",
    ]),
)

cc_library(
    name = "const",
    srcs = ["const.cc"],
    hdrs = ["const.h"],
    includes = ["../../"],
)

cc_library(
    name = "deserializer",
    hdrs = ["deserializer.h"],
    includes = [
        "../../",
        "../../../google3",
    ],
)

cc_library(
    name = "deserializer_impl",
    srcs = ["deserializer_impl.cc"],
    hdrs = ["deserializer_impl.h"],
    includes = [
        "../../",
        "../../../google3",
        "../../../includes",
        "../external",
    ],
    deps = [
        ":deserializer",
        ":search",
        ":utils",
        "//third_party/libxml",
        "//third_party/xmpmeta:integral_types",
        "//third_party/xmpmeta:logging",
        "//third_party/xmpmeta/internal/xmpmeta:base64",
        "//third_party/xmpmeta/internal/xmpmeta/external/strings",
    ],
)

cc_library(
    name = "search",
    srcs = ["search.cc"],
    hdrs = [
        "search.h",
    ],
    includes = [
        "../../",
        "../../../google3",
    ],
    deps = [
        "//third_party/libxml",
        "//third_party/xmpmeta:logging",
    ],
)

cc_library(
    name = "serializer",
    hdrs = ["serializer.h"],
)

cc_library(
    name = "serializer_impl",
    srcs = ["serializer_impl.cc"],
    hdrs = ["serializer_impl.h"],
    includes = [
        "../../",
        "../../../google3",
        "../../../includes",
        "../external",
    ],
    deps = [
        ":const",
        ":serializer",
        ":utils",
        "//strings",  # For testing in Blaze.
        "//third_party/libxml",
        "//third_party/xmpmeta:logging",
        "//third_party/xmpmeta/internal/xmpmeta/external/strings",
    ],
)

cc_library(
    name = "utils",
    srcs = ["utils.cc"],
    hdrs = ["utils.h"],
    includes = [
        "../../",
        "../../../google3",
    ],
    deps = [
        ":const",
        ":search",
        "//third_party/libxml",
        "//third_party/xmpmeta:logging",
    ],
)

cc_test(
    name = "deserializer_impl_test",
    srcs = ["deserializer_impl_test.cc"],
    includes = [
        "../../",
        "../../../google3",
        "../../../includes",
    ],
    deps = [
        ":deserializer_impl",
        ":utils",
        "//testing/base/public:gunit_main",
        "//third_party/libxml",
        "//third_party/xmpmeta/internal/xmpmeta:base64",
        "//third_party/xmpmeta/internal/xmpmeta/external/strings",
    ],
)

cc_test(
    name = "search_test",
    srcs = ["search_test.cc"],
    includes = [
        "../../",
        "../../../google3",
    ],
    deps = [
        ":search",
        ":utils",
        "//testing/base/public:gunit_main",
        "//third_party/libxml",
    ],
)

cc_test(
    name = "serializer_impl_test",
    srcs = ["serializer_impl_test.cc"],
    includes = [
        "../",
        "../../../google3",
        "../../../includes",
    ],
    deps = [
        ":serializer_impl",
        ":utils",
        "//testing/base/public:gunit_main",
        "//third_party/libxml",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_parser",
        "//third_party/xmpmeta/internal/xmpmeta/external/strings",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer_impl",
    ],
)

cc_test(
    name = "utils_test",
    srcs = ["utils_test.cc"],
    includes = [
        "../../",
        "../../../google3",
    ],
    deps = [
        ":utils",
        "//testing/base/public:gunit_main",
        "//third_party/libxml",
        "//third_party/xmpmeta/internal/xmpmeta:test_util",
        "//third_party/xmpmeta/internal/xmpmeta:test_xmp_creator",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_parser",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_writer",
    ],
)
