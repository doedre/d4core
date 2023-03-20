project('d4core', 'cpp',
  version : '0.1',
  default_options : ['cpp_std=c++20', 'warning_level=3'])

tests_dir = join_paths(meson.source_root(), 'tests')

d4core_dep = declare_dependency(
  include_directories : 'include'
)

static_factory_test = executable('static_factory_test',
  'tests/static_factory.cpp',
  dependencies : d4core_dep
)

test('Static factory test', static_factory_test)

