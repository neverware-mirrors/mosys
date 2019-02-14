cmocka_dep = dependency('cmocka')

foreach f: unittest_src
    name = '@0@'.format(f)
    test_dir = meson.current_source_dir()

    test_target = executable(
        name,
        f,
        include_directories: include_common,
        dependencies: cmocka_dep,
        link_with: libmosys
    )

    test(name, test_target, workdir: test_dir)
endforeach
