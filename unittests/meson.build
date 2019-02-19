cmocka_dep = dependency('cmocka')

# The Mosys binary is cross-compiled and thus cannot be
# executed on the build machine. Build a copy of the
# library with the native compiler, and use it to build
# a test executable with the native compiler.
# TODO(chromium:910641): Explore alternative ways to do
# this.
libmosys_native_TESTONLY = static_library(
  'mosys',
  libmosys_src,
  dependencies: deps,
  include_directories: include_common,
  pic: true,
  native: true,
  # We would expect this isn't necessary b/c of the line
  # "add_global_arguments('-include', 'config.h', language: 'c')"
  # in the main meson.build. However, it seems this isn't passed
  # to the native lib build; hard code it in so compilation
  # will work.
  c_args: ['-includeconfig.h']
)

foreach f: unittest_src
    name = '@0@'.format(f)
    test_dir = meson.current_source_dir()

    test_target = executable(
        name,
        f,
        include_directories: include_common,
        dependencies: cmocka_dep,
        link_with: libmosys_native_TESTONLY,
        native: true,
    )

    test(name, test_target, workdir: test_dir)
endforeach
