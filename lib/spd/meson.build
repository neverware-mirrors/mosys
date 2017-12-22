mosys_lib_spd_src = [
  'spd_fields.c',
  'nonspd_modules.c',
  'nonspd.c',
  'jedec_id.c',
  'ddr4.c',
  'ddr2.c',
  'ddr3.c',
  'spd.c',
]

libmosys_lib_spd = static_library(
  'mosys_lib_spd',
  [mosys_lib_spd_src],
  include_directories : include_common,
  link_with: [],
  pic: true,
)
