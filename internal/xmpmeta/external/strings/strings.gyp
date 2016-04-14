{
  'variables' : {
    'xmpmeta_dir': '<(DEPTH)/third_party/xmpmeta/internal/xmpmeta',
  },
  'target_defaults': {
    'include_dirs' : [
      '<(xmpmeta_dir)/external',
    ],
  },
  'targets': [
    {
      'target_name': 'strings',
      'type': 'static_library',
      'sources': [
        'ascii_ctype.cc',
        'escaping.cc',
        'numbers.cc',
        'case.cc',
      ],
      'conditions': [
        ['OS=="win"', {
          'msvs_disabled_warnings': [
            '4996', # Posix name for this item is deprecated.
          ]}
        ]
      ],
    }, # strings target.
  ],  # targets.
}
