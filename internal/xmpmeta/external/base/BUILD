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
    name = "base",
    hdrs = [
        "disable_warnings.h",
        "integral_types.h",
        "macros.h",
        "port.h",
    ],
    includes = [
        "../",
    ],
)
