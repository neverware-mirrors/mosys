mosys_lib_eeprom_src = [
  'eeprom.c',
  'eeprom_enet.c',
  'tg3.c',
]

libmosys_lib_eeprom = static_library(
  'mosys_lib_eeprom',
  [mosys_lib_eeprom_src],
  include_directories : include_common,
  link_with: [],
  pic: true,
)
