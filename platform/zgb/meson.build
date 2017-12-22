mosys_platform_zgb_src = [
  'eeprom.c',
  'chromia700.c',
  'nvram.c',
  'memory.c',
  'sys.c',
  'vpd.c',
  'gpio.c',
  'ec.c',
]

libmosys_platform_zgb = static_library(
  'mosys_platform_zgb',
  [mosys_platform_zgb_src],
  include_directories : include_common,
  link_with: [],
  pic: true,
)
