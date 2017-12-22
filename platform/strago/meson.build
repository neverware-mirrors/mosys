mosys_platform_strago_src = [
  'eeprom.c',
  'strago.c',
  'nvram.c',
  'memory.c',
  'sys.c',
  'gpio.c',
]

libmosys_platform_strago = static_library(
  'mosys_platform_strago',
  [mosys_platform_strago_src],
  include_directories : include_common,
  link_with: [],
  pic: true,
)
