platform_support_src += files(
  'platform_list.c',
)

if use_cros_config
  subdir('cheza')
  subdir('kukui')
  subdir('trogdor')
else
  subdir('cyclone')
  subdir('mistral')
  subdir('nyan')
  subdir('oak')
  subdir('pinky')
  subdir('storm')
endif

subdir('gru')
