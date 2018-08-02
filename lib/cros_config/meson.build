if test_cros_config
  cros_data_file = 'dummy_cros_config_data.c'
else
  cros_data_file = 'cros_config_data.c'
endif

if use_cros_config
  mosys_lib_cros_config_src = files(
    'cros_config.c',
    'cros_config_struct.c',
    cros_data_file,
  )
endif

cros_config_dtb_src = files(
  'config.dtb.S',
)
