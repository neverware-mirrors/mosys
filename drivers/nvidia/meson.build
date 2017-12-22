mosys_drivers_nvidia_src = []

subdir('tegra124')

libmosys_drivers_nvidia = static_library(
  'mosys_drivers_nvidia',
  [mosys_drivers_nvidia_src],
  include_directories : include_common,
  link_with: [
    libmosys_drivers_nvidia_tegra124,
  ],
  pic: true,
)
