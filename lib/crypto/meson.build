mosys_lib_crypto_src = [
  'crypto.c',
]

subdir('mincrypt')

libmosys_lib_crypto = static_library(
  'mosys_lib_crypto',
  [mosys_lib_crypto_src],
  include_directories : include_common,
  link_with: [
    libmosys_lib_crypto_mincrypt,
  ],
  pic: true,
)
