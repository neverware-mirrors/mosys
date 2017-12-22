mosys_platform_beltino_src = [
  'eeprom.c',
  'nvram.c',
  'memory.c',
  'sensors.c',
  'sys.c',
  'gpio.c',
  'beltino.c',
]

libmosys_platform_beltino = static_library(
  'mosys_platform_beltino',
  [mosys_platform_beltino_src],
  include_directories : include_common,
  link_with: [],
  pic: true,
)
