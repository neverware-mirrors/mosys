mosys_platform_samus_src = [
  'eeprom.c',
  'nvram.c',
  'memory.c',
  'samus.c',
  'sys.c',
  'gpio.c',
]

libmosys_platform_samus = static_library(
  'mosys_platform_samus',
  [mosys_platform_samus_src],
  include_directories : include_common,
  link_with: [],
  pic: true,
)
