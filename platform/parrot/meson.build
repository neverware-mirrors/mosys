mosys_platform_parrot_src = [
  'eeprom.c',
  'nvram.c',
  'memory.c',
  'parrot.c',
  'sys.c',
  'vpd.c',
  'ec.c',
]

libmosys_platform_parrot = static_library(
  'mosys_platform_parrot',
  [mosys_platform_parrot_src],
  include_directories : include_common,
  link_with: [],
  pic: true,
)
