mosys_platform_rambi_src = [
  'eeprom.c',
  'nvram.c',
  'memory.c',
  'sys.c',
  'gpio.c',
  'rambi.c',
  'psu.c',
  'ec.c',
]

libmosys_platform_rambi = static_library(
  'mosys_platform_rambi',
  [mosys_platform_rambi_src],
  include_directories : include_common,
  link_with: [],
  pic: true,
)
