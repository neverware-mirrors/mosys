mosys_platform_stout_src = [
  'eeprom.c',
  'nvram.c',
  'memory.c',
  'battery.c',
  'sys.c',
  'stout.c',
  'storage.c',
  'ec.c',
]

libmosys_platform_stout = static_library(
  'mosys_platform_stout',
  [mosys_platform_stout_src],
  include_directories : include_common,
  link_with: [],
  pic: true,
)
