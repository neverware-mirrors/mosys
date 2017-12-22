mosys_drivers_src = [
  'gpio.c',
  'superio.c',
]

subdir('ene')
subdir('google')
subdir('intel')
subdir('ite')
subdir('nuvoton')
subdir('nvidia')
subdir('samsung')
subdir('sandisk')
subdir('smsc')

libmosys_drivers = static_library(
  'mosys_drivers',
  [mosys_drivers_src],
  include_directories : include_common,
  link_with: [
    libmosys_drivers_ene,
    libmosys_drivers_google,
    libmosys_drivers_intel,
    libmosys_drivers_ite,
    libmosys_drivers_nuvoton,
    libmosys_drivers_nvidia,
    libmosys_drivers_samsung,
    libmosys_drivers_sandisk,
    libmosys_drivers_smsc,
  ],
  pic: true,
)
