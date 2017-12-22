mosys_platform_mario_src = [
  'eeprom.c',
  'memory.c',
  'sys.c',
  'vpd.c',
  'gpio.c',
  'ec.c',
  'cr48.c',
]

libmosys_platform_mario = static_library(
  'mosys_platform_mario',
  [mosys_platform_mario_src],
  include_directories : include_common,
  link_with: [],
  pic: true,
)
