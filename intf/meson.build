mosys_intf_src = [
  'i2c.c',
  'mmio.c',
  'io.c',
  'pci.c',
]

libmosys_intf = static_library(
  'mosys_intf',
  [mosys_intf_src],
  include_directories : include_common,
  link_with: [],
  pic: true,
)
