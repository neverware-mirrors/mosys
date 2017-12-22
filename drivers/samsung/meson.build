mosys_drivers_samsung_src = [
  'gpio.c',
]

subdir('exynos5250')
subdir('exynos5420')

libmosys_drivers_samsung = static_library(
  'mosys_drivers_samsung',
  [mosys_drivers_samsung_src],
  include_directories : include_common,
  link_with: [
    libmosys_drivers_samsung_exynos5250,
    libmosys_drivers_samsung_exynos5420,
  ],
  pic: true,
)
