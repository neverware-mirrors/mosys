project('mosys', 'c')

# Include common. This is passed to all subdir build files as well
include_common = include_directories(['include'])

# Config data used for creating a config header and including it
conf_data = configuration_data()

# Set for all platforms
conf_data.set('PROGRAM', '"mosys"')
conf_data.set('VERSION', '"1.2.03"')
conf_data.set('CONFIG_LITTLE_ENDIAN', 1)
conf_data.set('CONFIG_LOGLEVEL', 4)
conf_data.set('CONFIG_PLATFORM_EXPERIMENTAL', 1)
use_cros_config = get_option('use_cros_config') == true
if use_cros_config
  conf_data.set('CONFIG_CROS_CONFIG', 1)
endif
test_cros_config = get_option('test_cros_config') == true
if test_cros_config
  conf_data.set('CONFIG_TEST_CROS_CONFIG', 1)
endif

# Setting on a per-arch basis
arch = get_option('arch')
if arch == 'x86' or arch == 'x86_64' or arch == 'amd64'
  conf_data.set('CONFIG_PLATFORM_ARCH_X86', 1)
elif arch == 'mip'
  conf_data.set('CONFIG_PLATFORM_ARCH_MIPSEL', 1)
elif arch == 'arm' or arch == 'arm64'
  conf_data.set('CONFIG_PLATFORM_ARCH_ARMEL', 1)
endif

# Create the config header file and include it by default while compiling
configure_file(
  output : 'config.h',
  configuration : conf_data,
)
add_global_arguments('-include', 'config.h', language: 'c')
add_global_arguments('-std=gnu99', language : 'c')

cc = meson.get_compiler('c')
if (cc.has_argument('-Wno-address-of-packed-member'))
  add_global_arguments(['-Wno-address-of-packed-member'], language: 'c')
endif
add_global_arguments('-Werror', language : 'c')

# External libs used by Mosys
fmap_dep = dependency('fmap')
uuid_dep = dependency('uuid')

libmosys_src = files()

# Subdirs with source to link against
subdir('core')
subdir('drivers')
subdir('intf')
subdir('lib')
subdir('platform')

deps = [
  fmap_dep,
  uuid_dep,
]
link_whole = []
objects = []

# Cros config is a special snowflake.
if use_cros_config
  libmosys_src += mosys_lib_cros_config_src
  fdt_dep = meson.get_compiler('c').find_library('fdt')
  deps += fdt_dep
  dtb_lib = static_library(
    'cros_config_dtb',
    cros_config_dtb_src,
    c_args: [
      '-D__ASSEMBLY__',
      '-c',
    ],
    link_args: [
      '-znoexecstack',
      '-r',
    ],
  )
  link_whole += dtb_lib
  objects = dtb_lib.extract_all_objects()
endif

# Lib momsys shared library target
libmosys = static_library(
  'mosys',
  libmosys_src,
  dependencies: deps,
  include_directories: include_common,
  link_whole: link_whole,
  objects: objects,
  pic: true,
)

# Mosys dynamic executable
executable(
  'mosys',
  ['mosys.c'],
  include_directories: include_common,
  link_with: [libmosys],
  install: true,
  install_dir: '/usr/sbin',
)
