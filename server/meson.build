project(
  'cosmo-craft-server',
  'cpp',
  version : '0.1.0',
  license : 'MIT',
  meson_version : '>= 1.3.0',
)

if host_machine.system() == 'windows'
  default_options = [
    'cpp_std=c++latest'
  ]
  default_cpp_args = ['/bigobj', '-D_WIN32_WINNT=_WIN32_WINNT_WIN10']
else
  default_options = [
    'cpp_std=c++2c'
  ]
  default_cpp_args = []
endif

gtest = dependency('gtest')
gtest_main = dependency('gtest_main')
boost_headers = dependency('boost')
boost_po = dependency('boost', modules: ['program_options'])
spdlog = dependency('spdlog')

core = static_library(
  'core',
  files(
    'src/lib.cpp',
    'src/net.cpp'
  ),
  include_directories: 'src',
  dependencies: [boost_headers, spdlog],
  cpp_args: default_cpp_args,
  override_options: default_options,
)
core_dep = declare_dependency(
  link_with: core,
  include_directories: 'src',
)

executable('cosmocraft-server', 'src/main.cpp',
  dependencies: [core_dep, boost_po],
  cpp_args: default_cpp_args,
  override_options: default_options,
  install: true
)

test('tests',
  executable('tests', files('src/tests.cpp'),
    dependencies: [core_dep, gtest, gtest_main, boost_headers],
    cpp_args: default_cpp_args,
    override_options: default_options
  )
)
