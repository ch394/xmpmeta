licenses(["notice"])  # BSD

exports_files(["LICENSE"])

package(
    default_hdrs_check = "loose",
    default_visibility = ["//third_party/xmpmeta:xmpmeta_pkg"],
    features = [
        "-layering_check",
        "-parse_headers",
    ],
)

# Library target.

cc_library(
    name = "xdmlib",
    hdrs = ["//third_party/xmpmeta/includes:xdmlib-headers"],
    deps = [
        ":audio",
        ":camera",
        ":camera_pose",
        ":cameras",
        ":device",
        ":device_pose",
        ":equirect_model",
        ":image",
        ":point_cloud",
        ":profile",
        ":profiles",
    ],
)

# XDM targets.

cc_library(
    name = "audio",
    srcs = ["audio.cc"],
    hdrs = ["//third_party/xmpmeta/includes:xdmlib_audio_header"],
    includes = [
        "../",
        "../../includes/",
    ],
    deps = [
        ":element",
        "//third_party/libxml",
        "//third_party/xmpmeta/internal/xmpmeta:base64",
        "//third_party/xmpmeta/internal/xmpmeta/external/strings",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer",
    ],
)

cc_library(
    name = "camera",
    srcs = ["camera.cc"],
    hdrs = ["//third_party/xmpmeta/includes:xdmlib_camera_header"],
    includes = [
        "../",
        "../../includes/",
    ],
    deps = [
        ":audio",
        ":camera_pose",
        ":element",
        ":image",
        "//third_party/xmpmeta/internal/xmpmeta/external/strings",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer",
    ],
)

cc_library(
    name = "camera_pose",
    srcs = ["camera_pose.cc"],
    hdrs = ["//third_party/xmpmeta/includes:xdmlib_camera_pose_header"],
    includes = [
        "../",
        "../../includes/",
    ],
    deps = [
        "//third_party/xmpmeta/internal/xmpmeta/external/strings",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer",
    ],
)

cc_library(
    name = "element",
    hdrs = ["element.h"],
    deps = [
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer",
    ],
)

cc_library(
    name = "device_pose",
    srcs = ["device_pose.cc"],
    hdrs = ["//third_party/xmpmeta/includes:xdmlib_device_pose_header"],
    includes = [
        "../",
        "../../includes/",
    ],
    deps = [
        "//base",
        "//strings",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer",
    ],
)

cc_library(
    name = "cameras",
    srcs = ["cameras.cc"],
    hdrs = ["//third_party/xmpmeta/includes:xdmlib_cameras_header"],
    includes = [
        "../",
        "../../includes/",
    ],
    deps = [
        ":camera",
        ":element",
        "//base",
        "//strings",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer",
    ],
)

cc_library(
    name = "device",
    srcs = ["device.cc"],
    hdrs = ["//third_party/xmpmeta/includes:xdmlib_device_header"],
    includes = [
        "../",
        "../../includes/",
    ],
    deps = [
        ":cameras",
        ":device_pose",
        ":profiles",
        "//base",
        "//strings",
        "//third_party/libxml",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_const",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_data",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_parser",
        "//third_party/xmpmeta/internal/xmpmeta/xml:const",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:search",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)

cc_library(
    name = "equirect_model",
    srcs = ["equirect_model.cc"],
    hdrs = ["//third_party/xmpmeta/includes:xdmlib_equirect_model_header"],
    includes = [
        "../",
        "../../google3/",
        "../../includes/",
    ],
    deps = [
        ":dimension",
        ":element",
        ":point",
        "//base",
        "//third_party/xmpmeta/internal/xmpmeta/external/strings",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer",
    ],
)

cc_library(
    name = "image",
    srcs = ["image.cc"],
    hdrs = ["//third_party/xmpmeta/includes:xdmlib_image_header"],
    includes = [
        "../",
        "../../includes/",
    ],
    deps = [
        ":element",
        "//third_party/libxml",
        "//third_party/xmpmeta/internal/xmpmeta:base64",
        "//third_party/xmpmeta/internal/xmpmeta/external/strings",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)

cc_library(
    name = "dimension",
    hdrs = ["dimension.h"],
)

cc_library(
    name = "point_cloud",
    srcs = ["point_cloud.cc"],
    hdrs = ["//third_party/xmpmeta/includes:xdmlib_point_cloud_header"],
    includes = [
        "../",
        "../../google3",
        "../../includes/",
    ],
    deps = [
        ":element",
        "//base",
        "//strings",
        "//third_party/xmpmeta/internal/xmpmeta/external/strings",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)

cc_library(
    name = "profiles",
    srcs = ["profiles.cc"],
    hdrs = ["//third_party/xmpmeta/includes:xdmlib_profiles_header"],
    includes = [
        "../",
        "../../google3",
        "../../includes/",
    ],
    deps = [
        ":element",
        ":profile",
        "//base",
        "//strings",
        "//third_party/xmpmeta/internal/xmpmeta/external/strings",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer",
    ],
)

cc_library(
    name = "point",
    hdrs = ["point.h"],
)

cc_library(
    name = "profile",
    srcs = ["profile.cc"],
    hdrs = ["//third_party/xmpmeta/includes:xdmlib_profile_header"],
    includes = [
        "../",
        "../../google3",
        "../../includes/",
    ],
    deps = [
        ":element",
        "//base",
        "//strings",
        "//third_party/xmpmeta/internal/xmpmeta/external/strings",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer",
    ],
)

cc_test(
    name = "audio_test",
    srcs = ["audio_test.cc"],
    data = [
        "//third_party/xmpmeta:xdm_testdata",
    ],
    includes = [
        "../",
        "../../includes/",
    ],
    deps = [
        ":audio",
        "//third_party/libxml",
        "//third_party/xmpmeta/internal/xmpmeta:file",
        "//third_party/xmpmeta/internal/xmpmeta:test_util",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_data",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_writer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:const",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)

cc_test(
    name = "camera_test",
    srcs = ["camera_test.cc"],
    data = [
        "//third_party/xmpmeta:xdm_testdata",
    ],
    deps = [
        ":audio",
        ":camera",
        ":camera_pose",
        ":image",
        "//third_party/libxml",
        "//third_party/xmpmeta/internal/xmpmeta:base64",
        "//third_party/xmpmeta/internal/xmpmeta:file",
        "//third_party/xmpmeta/internal/xmpmeta:test_util",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_data",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_writer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:const",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)

cc_test(
    name = "equirect_model_test",
    srcs = ["equirect_model_test.cc"],
    data = [
        "//third_party/xmpmeta:xdm_testdata",
    ],
    deps = [
        ":equirect_model",
        "//third_party/libxml",
        "//third_party/xmpmeta/internal/xmpmeta:file",
        "//third_party/xmpmeta/internal/xmpmeta:test_util",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_data",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_writer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:const",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)

cc_test(
    name = "cameras_test",
    srcs = ["cameras_test.cc"],
    data = [
        "//third_party/xmpmeta:xdm_testdata",
    ],
    deps = [
        ":audio",
        ":camera",
        ":cameras",
        "//third_party/libxml",
        "//third_party/xmpmeta/internal/xmpmeta:base64",
        "//third_party/xmpmeta/internal/xmpmeta:file",
        "//third_party/xmpmeta/internal/xmpmeta:test_util",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_data",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_writer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:const",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)

cc_test(
    name = "device_test",
    srcs = ["device_test.cc"],
    data = [
        "//third_party/xmpmeta:xdm_testdata",
    ],
    deps = [
        ":audio",
        ":camera",
        ":cameras",
        ":device",
        ":profile",
        ":profiles",
        "//third_party/libxml",
        "//third_party/xmpmeta/internal/xmpmeta:base64",
        "//third_party/xmpmeta/internal/xmpmeta:file",
        "//third_party/xmpmeta/internal/xmpmeta:test_util",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_data",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_writer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:const",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)

cc_test(
    name = "image_test",
    srcs = ["image_test.cc"],
    data = [
        "//third_party/xmpmeta:xdm_testdata",
    ],
    deps = [
        ":image",
        "//third_party/libxml",
        "//third_party/xmpmeta/internal/xmpmeta:file",
        "//third_party/xmpmeta/internal/xmpmeta:test_util",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_data",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_writer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:const",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)

cc_test(
    name = "camera_pose_test",
    srcs = ["camera_pose_test.cc"],
    data = [
        "//third_party/xmpmeta:xdm_testdata",
    ],
    deps = [
        ":camera_pose",
        "//third_party/libxml",
        "//third_party/xmpmeta/internal/xmpmeta:file",
        "//third_party/xmpmeta/internal/xmpmeta:test_util",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_data",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_writer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:const",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)

cc_test(
    name = "device_pose_test",
    srcs = ["device_pose_test.cc"],
    data = [
        "//third_party/xmpmeta:xdm_testdata",
    ],
    deps = [
        ":device_pose",
        "//third_party/libxml",
        "//third_party/xmpmeta/internal/xmpmeta:file",
        "//third_party/xmpmeta/internal/xmpmeta:test_util",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_data",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_writer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:const",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)

cc_test(
    name = "point_cloud_test",
    srcs = ["point_cloud_test.cc"],
    data = [
        "//third_party/xmpmeta:xdm_testdata",
    ],
    deps = [
        ":point_cloud",
        "//third_party/libxml",
        "//third_party/xmpmeta/internal/xmpmeta:file",
        "//third_party/xmpmeta/internal/xmpmeta:test_util",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_writer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:const",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)

cc_test(
    name = "profile_test",
    srcs = ["profile_test.cc"],
    data = [
        "//third_party/xmpmeta:xdm_testdata",
    ],
    deps = [
        ":profile",
        "//third_party/libxml",
        "//third_party/xmpmeta/internal/xmpmeta:file",
        "//third_party/xmpmeta/internal/xmpmeta:test_util",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_data",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_writer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:const",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)

cc_test(
    name = "profiles_test",
    srcs = ["profiles_test.cc"],
    data = [
        "//third_party/xmpmeta:xdm_testdata",
    ],
    deps = [
        ":profile",
        ":profiles",
        "//third_party/libxml",
        "//third_party/xmpmeta/internal/xmpmeta:file",
        "//third_party/xmpmeta/internal/xmpmeta:test_util",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_const",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_data",
        "//third_party/xmpmeta/internal/xmpmeta:xmp_writer",
        "//third_party/xmpmeta/internal/xmpmeta/xml:const",
        "//third_party/xmpmeta/internal/xmpmeta/xml:deserializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:serializer_impl",
        "//third_party/xmpmeta/internal/xmpmeta/xml:utils",
    ],
)
