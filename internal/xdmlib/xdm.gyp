{
  'variables' : {
    'libxml_dir': '<(DEPTH)/third_party/libxml'
    'xmpmeta_includes_dir': '<(DEPTH)/third_party/xmpmeta/includes',
    'xmpmeta_internal_dir': '<(DEPTH)/third_party/xmpmeta/internal/xmpmeta',
  },
  'targets': [
    {
      'target_name': 'audio',
      'type': 'static_library',
      'include_dirs' : [
        '<(google3_dir)',
        '<(xmpmeta_includes_dir)',
      ],
      'dependencies': [
        '<(libxml_dir)/libxml.gyp:libxml',
        '<(xmpmeta_internal_dir)/xmpmeta.gyp:base64',
      ],
      'sources': [
        'audio.cc',
      ],
    }, # audio target.
    {
      'target_name': 'image',
      'type': 'static_library',
      'include_dirs' : [
        '<(google3_dir)',
        '<(xmpmeta_includes_dir)',
      ],
      'dependencies': [
        '<(libxml_dir)/libxml.gyp:libxml',
        '<(xmpmeta_internal_dir)/xmpmeta.gyp:base64',
      ],
      'sources': [
        'image.cc',
      ],
    }, # image target.
    {
      'target_name': 'camera',
      'type': 'static_library',
      'include_dirs' : [
        '<(google3_dir)',
        '<(xmpmeta_includes_dir)',
      ],
      'dependencies': [
        ':audio',
        ':camera_pose',
        ':image',
      ],
      'sources': [
        'camera.cc',
      ],
    }, # camera target.
    {
      'target_name': 'camera_pose',
      'type': 'static_library',
      'include_dirs' : [
        '<(google3_dir)',
        '<(xmpmeta_includes_dir)',
      ],
      'sources': [
        'camera_pose.cc',
      ],
    }, # camera_pose target.
    {
      'target_name': 'device_pose',
      'type': 'static_library',
      'include_dirs' : [
        '<(google3_dir)',
        '<(xmpmeta_includes_dir)',
      ],
      'sources': [
        'device_pose.cc',
      ],
    }, # device_pose target.
    {
      'target_name': 'equirect_model',
      'type': 'static_library',
      'include_dirs' : [
        '<(google3_dir)',
        '<(xmpmeta_includes_dir)',
      ],
      'sources': [
        'equirect_model.cc',
      ],
    }, # equirect_model target.
    {
      'target_name': 'profile',
      'type': 'static_library',
      'include_dirs' : [
        '<(google3_dir)',
        '<(xmpmeta_includes_dir)',
      ],
      'sources': [
        'profile.cc',
      ],
    }, # profile target.
  ],  # targets.
}
