mosys_platform_lumpy_src = [
  'eeprom.c',
  'nvram.c',
  'lumpy.c',
  'memory.c',
  'sensors.c',
  'sys.c',
  'vpd.c',
  'gpio.c',
  'ec.c',
]

libmosys_platform_lumpy = static_library(
  'mosys_platform_lumpy',
  [mosys_platform_lumpy_src],
  include_directories : include_common,
  link_with: [],
  pic: true,
)
