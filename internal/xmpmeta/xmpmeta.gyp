{
  'variables' : {
    'google3_dir': '<(DEPTH)',
    'libxml_dir': '<(DEPTH)/third_party/libxml',
    'xmpmeta_dir': '<(DEPTH)/third_party/xmpmeta/internal/xmpmeta',
  },
  'targets': [
    {
      'target_name': 'base64',
      'type': 'static_library',
      'include_dirs' : [
        '<(xmpmeta_dir)/external',
        '<(google3_dir)',
      ],
      'dependencies': [
        '<(xmpmeta_dir)/external/strings/strings.gyp:strings',
      ],
      'sources': [
        'base64.cc',
      ],
    }, # base64 target.
    {
      'target_name': 'jpeg_io',
      'type': 'static_library',
      'include_dirs' : [
        '<(xmpmeta_dir)/external',
        '<(google3_dir)',
      ],
      'dependencies': [
        '<(xmpmeta_dir)/external/strings/strings.gyp:strings',
      ],
      'sources': [
        'jpeg_io.cc',
      ],
      'conditions': [
        ['OS=="win"', {
          'msvs_disabled_warnings': [
            '4267', # Conversion from size_t to int.
          ]}
        ]
      ],
    }, # jpeg_io target.
    {
      'target_name': 'md5',
      'type': 'static_library',
      'include_dirs' : [
        '<(xmpmeta_dir)/external',
        '<(google3_dir)',
      ],
      'dependencies': [
        '<(xmpmeta_dir)/external/strings/strings.gyp:strings',
      ],
      'sources': [
        'md5.cc',
      ],
      'conditions': [
        ['OS == "win"', {
          'link_settings': {
            'msvs_settings': {
              'VCLinkerTool': {
                'AdditionalDependencies': [
                  'Advapi32.lib',
                ],
              },
            },
          },
        }],
      ],
    }, # md5 target.
    {
      'target_name': 'xmp_const',
      'type': 'static_library',
      'include_dirs' : [
        '<(xmpmeta_dir)/external',
        '<(google3_dir)',
      ],
      'dependencies': [
        '<(libxml_dir)/libxml.gyp:libxml',
        '<(xmpmeta_dir)/external/strings/strings.gyp:strings',
      ],
      'sources': [
        'xmp_const.cc',
      ],
    }, # xmp_const target.
    {
      'target_name': 'xmp_data',
      'type': 'static_library',
      'include_dirs' : [
        '<(xmpmeta_dir)/external',
        '<(google3_dir)',
      ],
      'dependencies': [
        '<(libxml_dir)/libxml.gyp:libxml',
        '<(xmpmeta_dir)/external/strings/strings.gyp:strings',
      ],
      'sources': [
        'xmp_data.cc',
      ],
    }, # xmp_data target.
    {
      'target_name': 'xmp_parser',
      'type': 'static_library',
      'include_dirs' : [
        '<(xmpmeta_dir)/external',
        '<(google3_dir)',
      ],
      'dependencies': [
        ':base64',
        ':jpeg_io',
        ':xmp_data',
        '<(xmpmeta_dir)/xml/xml.gyp:utils',
      ],
      'sources': [
        'xmp_parser.cc',
      ],
      'conditions': [
        ['OS=="win"', {
          'msvs_disabled_warnings': [
            '4996', # Conversion from size_t to int.
          ]}
        ]
      ],
    }, # xmp_parser target.
    {
      'target_name': 'xmp_writer',
      'type': 'static_library',
      'include_dirs' : [
        '<(xmpmeta_dir)/external',
        '<(google3_dir)',
      ],
      'dependencies': [
        ':jpeg_io',
        ':md5',
        ':xmp_const',
        '<(xmpmeta_dir)/xml/xml.gyp:const',
        '<(xmpmeta_dir)/xml/xml.gyp:utils',
      ],
      'sources': [
        'xmp_writer.cc',
      ],
    }, # xmp_writer target.
    {
      'target_name': 'gaudio',
      'type': 'static_library',
      'include_dirs' : [
        '<(xmpmeta_dir)/external',
        '<(google3_dir)',
      ],
      'dependencies': [
        ':base64',
        ':xmp_parser',
        '<(xmpmeta_dir)/xml/xml.gyp:deserializer_impl',
        '<(xmpmeta_dir)/xml/xml.gyp:utils',
      ],
      'sources': [
        'gaudio.cc',
      ],
    }, # gaudio target.
    {
      'target_name': 'gimage',
      'type': 'static_library',
      'include_dirs' : [
        '<(xmpmeta_dir)/external',
        '<(google3_dir)',
      ],
      'dependencies': [
        ':xmp_parser',
        '<(xmpmeta_dir)/xml/xml.gyp:deserializer_impl',
        '<(xmpmeta_dir)/xml/xml.gyp:utils',
      ],
      'sources': [
        'gimage.cc',
      ],
    }, # gimage target.
    {
      'target_name': 'gpano',
      'type': 'static_library',
      'include_dirs' : [
        '<(xmpmeta_dir)/external',
        '<(google3_dir)',
      ],
      'dependencies': [
        ':xmp_parser',
        '<(xmpmeta_dir)/xml/xml.gyp:deserializer_impl',
        '<(xmpmeta_dir)/xml/xml.gyp:utils',
      ],
      'sources': [
        'gpano.cc',
      ],
    }, # gpano target.
  ],  # targets.
}
