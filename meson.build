project('mosys', 'c')

# Include common. This is passed to all subdir build files as well
include_common = include_directories(['include'])

# Config data used for creating a config header and including it
conf_data = configuration_data()

# Set for all platforms
conf_data.set('PROGRAM', '"mosys"')
conf_data.set('VERSION', '"1.2.03"')
conf_data.set('CONFIG_LITTLE_ENDIAN', 1)
conf_data.set('CONFIG_LOGLEVEL', 4)
conf_data.set('CONFIG_PLATFORM_EXPERIMENTAL', 1)
use_cros_config = get_option('use_cros_config') == 'true'
if use_cros_config
  conf_data.set('CONFIG_CROS_CONFIG', 1)
endif

# Setting on a per-arch basis
arch = get_option('arch')
if arch == 'x86' or arch == 'x86_64' or arch == 'amd64'
  conf_data.set('CONFIG_CHIPSET_INTEL', 1)
  conf_data.set('CONFIG_CHIPSET_INTEL_LYNXPOINT_LP', 1)
  conf_data.set('CONFIG_CHIPSET_INTEL_NM10', 1)
  conf_data.set('CONFIG_CHIPSET_INTEL_SERIES6', 1)
  conf_data.set('CONFIG_CROS_EC', 1)
  conf_data.set('CONFIG_CROS_EC_LPC', 1)
  conf_data.set('CONFIG_EC_ENE_KB932', 1)
  conf_data.set('CONFIG_EC_ITE_IT8500', 1)
  conf_data.set('CONFIG_EC_NUVOTON_NPCE781', 1)
  conf_data.set('CONFIG_EC_SMSC_MEC1308', 1)
  conf_data.set('CONFIG_EXPERIMENTAL_KIEV', 1)
  conf_data.set('CONFIG_INTF_PORT_IO', 1)
  conf_data.set('CONFIG_PLATFORM_ALEX', 1)
  conf_data.set('CONFIG_PLATFORM_ARCH_X86', 1)
  conf_data.set('CONFIG_PLATFORM_AURON', 1)
  conf_data.set('CONFIG_PLATFORM_BELTINO', 1)
  conf_data.set('CONFIG_PLATFORM_BUTTERFLY', 1)
  conf_data.set('CONFIG_PLATFORM_CYAN', 1)
  conf_data.set('CONFIG_PLATFORM_FIZZ', 1)
  conf_data.set('CONFIG_PLATFORM_GLADOS', 1)
  conf_data.set('CONFIG_PLATFORM_KAHLEE', 1)
  conf_data.set('CONFIG_PLATFORM_LINK', 1)
  conf_data.set('CONFIG_PLATFORM_LUMPY', 1)
  conf_data.set('CONFIG_PLATFORM_MARIO', 1)
  conf_data.set('CONFIG_PLATFORM_PARROT', 1)
  conf_data.set('CONFIG_PLATFORM_RAMBI', 1)
  conf_data.set('CONFIG_PLATFORM_REEF', 1)
  conf_data.set('CONFIG_PLATFORM_SAMUS', 1)
  conf_data.set('CONFIG_PLATFORM_SLIPPY', 1)
  conf_data.set('CONFIG_PLATFORM_STOUT', 1)
  conf_data.set('CONFIG_PLATFORM_STRAGO', 1)
  conf_data.set('CONFIG_PLATFORM_STUMPY', 1)
  conf_data.set('CONFIG_PLATFORM_ZGB', 1)
  conf_data.set('CONFIG_SOC_INTEL_BAYTRAIL', 1)
  conf_data.set('CONFIG_SOC_INTEL_BRASWELL', 1)
  conf_data.set('CONFIG_STORAGE_SANDISK_U100', 1)
  conf_data.set('CONFIG_SUPERIO_ITE_IT8772', 1)
elif arch == 'mip'
  conf_data.set('CONFIG_PLATFORM_ARCH_MIPSEL', 1)
elif arch == 'arm'
  conf_data.set('CONFIG_CROS_EC', 1)
  conf_data.set('CONFIG_CROS_EC_I2C', 1)
  conf_data.set('CONFIG_PLATFORM_ARCH_ARMEL', 1)
  conf_data.set('CONFIG_PLATFORM_CYCLONE', 1)
  conf_data.set('CONFIG_PLATFORM_DAISY', 1)
  conf_data.set('CONFIG_PLATFORM_GRU', 1)
  conf_data.set('CONFIG_PLATFORM_NYAN', 1)
  conf_data.set('CONFIG_PLATFORM_OAK', 1)
  conf_data.set('CONFIG_PLATFORM_PEACH', 1)
  conf_data.set('CONFIG_PLATFORM_PINKY', 1)
  conf_data.set('CONFIG_PLATFORM_SKATE', 1)
  conf_data.set('CONFIG_PLATFORM_SMAUG', 1)
  conf_data.set('CONFIG_PLATFORM_SPRING', 1)
  conf_data.set('CONFIG_PLATFORM_STORM', 1)
  conf_data.set('CONFIG_SOC_NVIDIA', 1)
  conf_data.set('CONFIG_SOC_NVIDIA_TEGRA124', 1)
  conf_data.set('CONFIG_SOC_SAMSUNG', 1)
  conf_data.set('CONFIG_SOC_SAMSUNG_EXYNOS5250', 1)
  conf_data.set('CONFIG_SOC_SAMSUNG_EXYNOS5420', 1)
  conf_data.set('CONFIG_TRACKPAD_CYPRESS_APA', 1)
endif

# Create the config header file and include it by default while compiling
configure_file(
  output : 'config.h',
  configuration : conf_data,
)
add_global_arguments('-include', 'config.h', language: 'c')

# External libs used by Mosys
fmap_dep = dependency('fmap')
uuid_dep = dependency('uuid')

# Subdirs with source to link against
subdir('core')
subdir('drivers')
subdir('intf')
subdir('lib')
subdir('platform')

# Lib momsys shared library target
libmosys = static_library(
  'mosys',
  [],
  dependencies: [
    fmap_dep,
    uuid_dep,
  ],
  include_directories: include_common,
  link_with: [
    libmosys_core,
    libmosys_drivers,
    libmosys_intf,
    libmosys_lib,
    libmosys_platform,
  ],
  pic: true
)

# Add static linking if static is set
link_args = []
if get_option('static') == 'true'
  link_args += '-static'
endif

# Mosys dynamic executable
executable(
  'mosys',
  ['mosys.c'],
  include_directories: include_common,
  link_args: link_args,
  link_with: [libmosys],
  install: true,
  install_dir: '/usr/sbin',
)

# Mosys static executable. This will be removed by the ebuild if static is not
# set, but is always built and installed first.
executable(
  'mosys_s',
  ['mosys.c'],
  include_directories: include_common,
  link_args: ['-static'],
  link_with: [libmosys],
  install: true,
  install_dir: '/usr/sbin',
)

# Import tests last, which can link against libmosys
subdir('tests')
