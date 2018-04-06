mosys_lib_cros_config_src = [
  'cros_config.c',
  'cros_config_struct.c',
  'cros_config_data.c',
]

deps = []
link_with = []

if use_cros_config
  fdt_dep = meson.get_compiler('c').find_library('fdt')
  deps += fdt_dep
  dtb_lib = static_library(
    'cros_config_dtb',
    ['config.dtb.S'],
    c_args: [
      '-Itools/vpd_encode',
      '-D__ASSEMBLY__',
      '-c',
    ],
    link_args: [
      '-znoexecstack',
      '-r',
    ],
  )
  link_with += dtb_lib
endif

libmosys_lib_cros_config = static_library(
  'mosys_lib_cros_config',
  [mosys_lib_cros_config_src],
  dependencies: deps,
  include_directories : include_common,
  link_with: link_with,
  pic: true,
)
