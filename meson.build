project(
    'libdeds',
    'c',
    license : 'GPL-2.0-or-later',
    version : '1.2.1',
    default_options: ['b_ndebug=if-release'],
)

inc = include_directories('include')

subdir('src')
subdir('include')
subdir('tests')

libdeds_dep = declare_dependency(
    include_directories: inc,
    link_with: lib,
)
