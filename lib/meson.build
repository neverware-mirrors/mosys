mosys_lib_src = []

subdir('acpi')
subdir('cbfs')
subdir('cros_config')
subdir('crypto')
subdir('dynamic_array')
subdir('eeprom')
subdir('eventlog')
subdir('file')
subdir('flashrom')
subdir('math')
subdir('misc')
subdir('sensors')
subdir('smbios')
subdir('spd')
subdir('string')
subdir('vbnv')
subdir('vpd')

libmosys_lib = static_library(
  'mosys_lib',
  [mosys_lib_src],
  include_directories : include_common,
  link_with: [
    libmosys_lib_acpi,
    libmosys_lib_cbfs,
    libmosys_lib_cros_config,
    libmosys_lib_crypto,
    libmosys_lib_dynamic_array,
    libmosys_lib_eeprom,
    libmosys_lib_eventlog,
    libmosys_lib_file,
    libmosys_lib_flashrom,
    libmosys_lib_math,
    libmosys_lib_misc,
    libmosys_lib_sensors,
    libmosys_lib_smbios,
    libmosys_lib_spd,
    libmosys_lib_string,
    libmosys_lib_vbnv,
    libmosys_lib_vpd,
  ],
  pic: true,
)
