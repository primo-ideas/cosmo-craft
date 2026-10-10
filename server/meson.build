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
elif host_machine.system() == 'linux'
  default_options = [
    'cpp_std=c++2c'
  ]
  default_cpp_args = []
else
  default_options = [
    'cpp_std=c++26'
  ]
  default_cpp_args = []
endif

gtest = dependency('gtest')
boost_headers = dependency('boost')
boost_program_options = dependency('boost', modules: ['program_options'])
spdlog = dependency('spdlog')
json = dependency('nlohmann_json')
jolt = declare_dependency(
  dependencies : dependency('Jolt', method : 'cmake', modules : ['Jolt::Jolt']),
  compile_args : ['-DJPH_NO_DEBUG'],
)

core = static_library(
  'core',
  files(
    'lib/instance.cpp',
    'lib/listener.cpp',
    'lib/session.cpp',
    'lib/log.cpp',
    'lib/game.cpp',
    'lib/protocol.cpp',
    'lib/physics.cpp',
    'lib/player.cpp',
    'lib/voxel.cpp',
  ),
  include_directories: ['lib', 'lib/include'],
  dependencies: [boost_headers, spdlog, json, jolt],
  cpp_args: default_cpp_args,
  override_options: default_options,
)
core_dep = declare_dependency(
  link_with: core,
  include_directories: 'lib/include',
)

executable('cosmocraft-server', 'bin/main.cpp',
  dependencies: [core_dep, boost_program_options, spdlog],
  cpp_args: default_cpp_args,
  override_options: default_options,
  install: true
)

test('tests',
  executable('tests', files('bin/tests.cpp'),
    dependencies: [core_dep, gtest, boost_headers, spdlog, json],
    cpp_args: default_cpp_args,
    override_options: default_options
  )
)
