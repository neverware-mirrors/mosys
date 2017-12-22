if use_cros_config
  test(
    'mosys test simple_tests',
    executable(
      'simple_tests',
      'simple_tests.c',
      include_directories : include_common,
      link_with: [
        libmosys,
      ]
    ),
  )
endif
