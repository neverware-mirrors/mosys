mosys_core_src = [
  'log.c',
  'intf_list.c',
  'big_lock.c',
  'globals.c',
  'file_backed_range.c',
  'callbacks.c',
  'csem.c',
  'output.c',
  'list.c',
  'kv_pair.c',
  'alloc.c',
  'ipc_lock.c',
  'platform.c',
  'file_lock.c',
]

subdir('command')

libmosys_core = static_library(
  'mosys_core',
  [mosys_core_src],
  include_directories : include_common,
  link_with: [
    libmosys_core_command,
  ],
  pic: true,
)
