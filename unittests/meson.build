cmocka_dep = dependency('cmocka')

foreach f: unittest_src
    path = '@0@'.format(f)
    # Strip the leading path and suffix to get the
    # test name. E.g. 'a/b/test.c' -> 'test'.
    file_name = path.split('/').get(-1)
    name = file_name.split('.').get(0)

    test_target = executable(
        name,
        f,
        include_directories: include_common,
        dependencies: cmocka_dep,
        link_with: libmosys,
    )
endforeach
