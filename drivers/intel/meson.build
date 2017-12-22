mosys_drivers_intel_src = [
  'ich_gpio.c',
  'lpss_gpio.c',
  'lpss_generic.c',
  'ich_generic.c',
]

subdir('baytrail')
subdir('braswell')
subdir('lynxpoint_lp')
subdir('nm10')
subdir('series6')

libmosys_drivers_intel = static_library(
  'mosys_drivers_intel',
  [mosys_drivers_intel_src],
  include_directories : include_common,
  link_with: [
    libmosys_drivers_intel_baytrail,
    libmosys_drivers_intel_braswell,
    libmosys_drivers_intel_lynxpoint_lp,
    libmosys_drivers_intel_nm10,
    libmosys_drivers_intel_series6,
  ],
  pic: true,
)
