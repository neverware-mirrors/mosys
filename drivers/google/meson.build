mosys_drivers_google_src = [
  'cros_ec_i2c.c',
  'cros_ec_lpc.c',
  'cros_ec.c',
  'cros_ec_dev.c',
  'cros_ec_cb.c',
  'cros_ec_lock.c',
]

libmosys_drivers_google = static_library(
  'mosys_drivers_google',
  [mosys_drivers_google_src],
  include_directories : include_common,
  link_with: [],
  pic: true,
)
