{
  'variables' : {
    'google3_dir': '<(DEPTH)',
    'libxml_dir': '<(DEPTH)/third_party/libxml',
    'meta_dir': '<(DEPTH)/third_party/xmpmeta/internal/xmpmeta',
  },
  'targets': [
    {
      'target_name': 'const',
      'type': 'static_library',
      'sources': [
        'const.cc',
      ],
    }, # const target.
    {
      'target_name': 'search',
      'type': 'static_library',
      'include_dirs' : [
        '<(google3_dir)',
      ],
      'dependencies': [
        '<(libxml_dir)/libxml.gyp:libxml',
      ],
      'sources': [
        'search.cc',
      ],
    }, # search target.
    {
      'target_name': 'utils',
      'type': 'static_library',
      'include_dirs' : [
        '<(google3_dir)',
      ],
      'dependencies': [
        ':search',
        ':const',
      ],
      'sources': [
        'utils.cc',
      ],
    }, # utils target.
    {
      'target_name': 'serializer_impl',
      'type': 'static_library',
      'include_dirs' : [
        '<(meta_dir)/external',
        '<(google3_dir)',
      ],
      'dependencies': [
        ':const',
        '<(libxml_dir)/libxml.gyp:libxml',
        '<(meta_dir)/external/strings/strings.gyp:strings',
      ],
      'sources': [
        'serializer_impl.cc',
      ],
    }, # serializer_impl target.
    {
      'target_name': 'deserializer_impl',
      'type': 'static_library',
      'include_dirs' : [
        '<(meta_dir)/external',
        '<(google3_dir)',
      ],
      'dependencies': [
        ':search',
        ':utils',
        '<(libxml_dir)/libxml.gyp:libxml',
        '<(meta_dir)/external/strings/strings.gyp:strings',
      ],
      'sources': [
        'deserializer_impl.cc',
      ],
    }, # deserializer_impl target.
  ],  # targets.
}
