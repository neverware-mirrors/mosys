mosys_core_command_src = [
  'i2c.c',
  'eeprom.c',
  'eventlog.c',
  'nvram.c',
  'memory.c',
  'fp.c',
  'hid.c',
  'pd.c',
  'battery.c',
  'sh.c',
  'vpd.c',
  'memory_spd.c',
  'gpio.c',
  'sensor.c',
  'storage.c',
  'psu.c',
  'platform.c',
  'ec.c',
  'smbios.c',
]

libmosys_core_command = static_library(
  'mosys_core_command',
  [mosys_core_command_src],
  include_directories : include_common,
  link_with: [],
  pic: true,
)
