project('mosys', 'c')

# Include common. This is passed to all subdir build files as well
include_common = include_directories(['include'])

# Config data used for creating a config header and including it
conf_data = configuration_data()

use_vpd_file_cache = get_option('use_vpd_file_cache') == true
use_cros_config = get_option('use_cros_config') == true
if use_cros_config
  conf_data.set('CONFIG_CROS_CONFIG', 1)
endif

# Setting on a per-arch basis
arch = host_machine.cpu_family()
if arch == 'x86' or arch == 'x86_64'
  conf_data.set('CONFIG_PLATFORM_ARCH_X86', 1)
elif arch == 'arm' or arch == 'aarch64'
  conf_data.set('CONFIG_PLATFORM_ARCH_ARMEL', 1)
endif

platform_intf = get_option('platform_intf')
if platform_intf != ''
  conf_data.set_quoted('CONFIG_SINGLE_PLATFORM', platform_intf)
endif

# Create the config header file and include it by default while compiling
configure_file(
  output : 'config.h',
  configuration : conf_data,
)
add_global_arguments('-include', 'config.h', language: 'c')
add_global_arguments('-std=gnu99', language : 'c')

cc = meson.get_compiler('c')
add_global_arguments('-Wall', language : 'c')
add_global_arguments('-Werror', language : 'c')
add_global_arguments('-Wstrict-prototypes', language : 'c')
add_global_arguments('-Wundef', language : 'c')

# Require that externally-visible symbols have a prototype.  This
# helps find missing usages of 'static'.
add_global_arguments('-Wmissing-variable-declarations', language : 'c')
add_global_arguments('-Wmissing-prototypes', language : 'c')

# External libs used by Mosys
minijail_dep = declare_dependency(link_args: '-lminijail')

libmosys_src = files()
platform_support_src = files()
unittest_src = files()

# Subdirs with source to link against
subdir('core')
subdir('drivers')
subdir('intf')
subdir('lib')
subdir('platform')

deps = [minijail_dep]

# Cros config is a special snowflake.
if use_cros_config
  libmosys_src += mosys_lib_cros_config_src
endif

subdir('mains')
subdir('unittests')
