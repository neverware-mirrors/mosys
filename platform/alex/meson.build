mosys_platform_alex_src = [
  'eeprom.c',
  'memory.c',
  'sys.c',
  'vpd.c',
  'gpio.c',
  'series5.c',
  'ec.c',
]

libmosys_platform_alex = static_library(
  'mosys_platform_alex',
  [mosys_platform_alex_src],
  include_directories : include_common,
  link_with: [],
  pic: true,
)
