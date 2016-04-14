# Package-private targets for source code forked from google3/strings.
# Required for cross-platform portability.
licenses(["notice"])  # BSD

exports_files(["LICENSE"])

package(
    default_visibility = ["//third_party/xmpmeta:xmpmeta_pkg"],
    features = [
        "-layering_check",
        "-parse_headers",
    ],
)

cc_library(
    name = "strings",
    srcs = [
        "ascii_ctype.cc",
        "case.cc",
        "escaping.cc",
        "numbers.cc",
    ],
    hdrs = [
        "ascii_ctype.h",
        "case.h",
        "escaping.h",
        "fastmem.h",
        "numbers.h",
        "util.h",
    ],
    includes = [
        "../../../../google3",
    ],
    licenses = ["notice"],  # BSD
    deps = [
        "//base",
        "//strings",
        "//third_party/xmpmeta:logging",
    ],
)
