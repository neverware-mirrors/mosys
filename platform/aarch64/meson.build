libmosys_src += files(
  'platform_list.c',
)

# aarch64 userspace in ChromeOS is still experimental.
#
# - Devices which have a 64-bit processor but 32-bit userspace should
#   be placed under platform/arm/.
#
# - Devices which we are currently experimenting with 64-bit userspace
#   but still have a 32-bit build should be placed under platform/arm/
#   and symlinked here.
#
# - Devices which are 64-bit userspace only (none yet) should be
#   placed under here, not under platform/arm/.

if use_cros_config
  subdir('cheza')
  subdir('kukui')
  subdir('trogdor')
endif

subdir('gru')
