licenses(["notice"])  # BSD

package(
    default_visibility = ["//third_party/xmpmeta:xmpmeta_pkg"],
    features = [
        "-parse_headers",
        "no_layering_check",
    ],
)

filegroup(
    name = "xmpmeta-headers",
    srcs = glob(["xmpmeta/*.h"]),
)

filegroup(
    name = "xdmlib-headers",
    srcs = glob(["xmpmeta/*.h"]),
)

cc_library(
    name = "pano_meta_data",
    hdrs = ["xmpmeta/pano_meta_data.h"],
)

# xmpmeta headers.

filegroup(
    name = "vr_photo_writer_header",
    srcs = ["xmpmeta/vr_photo_writer.h"],
)

filegroup(
    name = "gaudio_header",
    srcs = ["xmpmeta/gaudio.h"],
)

filegroup(
    name = "gimage_header",
    srcs = ["xmpmeta/gimage.h"],
)

filegroup(
    name = "gpano_header",
    srcs = ["xmpmeta/gpano.h"],
)

filegroup(
    name = "md5_header",
    srcs = ["xmpmeta/md5.h"],
)

filegroup(
    name = "pano_meta_data_header",
    srcs = ["xmpmeta/pano_meta_data.h"],
)

filegroup(
    name = "xmp_const_header",
    srcs = ["xmpmeta/xmp_const.h"],
)

filegroup(
    name = "xmp_data_header",
    srcs = ["xmpmeta/xmp_data.h"],
)

filegroup(
    name = "xmp_parser_header",
    srcs = ["xmpmeta/xmp_parser.h"],
)

filegroup(
    name = "xmp_writer_header",
    srcs = ["xmpmeta/xmp_writer.h"],
)

# xdmlib headers.

filegroup(
    name = "xdmlib_audio_header",
    srcs = ["xdmlib/audio.h"],
)

filegroup(
    name = "xdmlib_camera_header",
    srcs = ["xdmlib/camera.h"],
)

filegroup(
    name = "xdmlib_camera_pose_header",
    srcs = ["xdmlib/camera_pose.h"],
)

filegroup(
    name = "xdmlib_cameras_header",
    srcs = ["xdmlib/cameras.h"],
)

filegroup(
    name = "xdmlib_device_header",
    srcs = ["xdmlib/device.h"],
)

filegroup(
    name = "xdmlib_device_pose_header",
    srcs = ["xdmlib/device_pose.h"],
)

filegroup(
    name = "xdmlib_equirect_model_header",
    srcs = ["xdmlib/equirect_model.h"],
)

filegroup(
    name = "xdmlib_image_header",
    srcs = ["xdmlib/image.h"],
)

filegroup(
    name = "xdmlib_point_cloud_header",
    srcs = ["xdmlib/point_cloud.h"],
)

filegroup(
    name = "xdmlib_profile_header",
    srcs = ["xdmlib/profile.h"],
)

filegroup(
    name = "xdmlib_profiles_header",
    srcs = ["xdmlib/profiles.h"],
)
