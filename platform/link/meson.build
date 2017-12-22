mosys_platform_link_src = [
  'eeprom.c',
  'nvram.c',
  'memory.c',
  'sys.c',
  'gpio.c',
  'link.c',
  'ec.c',
]

libmosys_platform_link = static_library(
  'mosys_platform_link',
  [mosys_platform_link_src],
  include_directories : include_common,
  link_with: [],
  pic: true,
)
