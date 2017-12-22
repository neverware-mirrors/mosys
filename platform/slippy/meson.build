mosys_platform_slippy_src = [
  'eeprom.c',
  'nvram.c',
  'memory.c',
  'slippy.c',
  'sys.c',
  'gpio.c',
  'ec.c',
]

libmosys_platform_slippy = static_library(
  'mosys_platform_slippy',
  [mosys_platform_slippy_src],
  include_directories : include_common,
  link_with: [],
  pic: true,
)
