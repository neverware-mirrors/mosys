mosys_platform_stumpy_src = [
  'eeprom.c',
  'nvram.c',
  'memory.c',
  'stumpy.c',
  'sensors.c',
  'sys.c',
  'vpd.c',
]

libmosys_platform_stumpy = static_library(
  'mosys_platform_stumpy',
  [mosys_platform_stumpy_src],
  include_directories : include_common,
  link_with: [],
  pic: true,
)
