# By Claude Opus

project(
  'boost',
  'cpp',
  version : '1.92.0',
  license : 'BSL-1.0',
  meson_version : '>= 1.3.0',
  default_options : [
    'cpp_std=c++17',
  ],
)

cpp = meson.get_compiler('cpp')

boost_inc = include_directories('.', is_system : true)

# Libraries compiled here are linked explicitly, disable MSVC auto-linking
# (#pragma comment(lib, "libboost_...")) which would look for b2-named libs.
boost_args = ['-DBOOST_ALL_NO_LIB']

boost_headers_deps = [dependency('threads')]
if host_machine.system() == 'windows'
  # Asio only auto-links Winsock with MSVC
  boost_headers_deps += [
    cpp.find_library('ws2_32'),
    cpp.find_library('mswsock'),
  ]
endif

boost_headers_dep = declare_dependency(
  include_directories : boost_inc,
  compile_args : boost_args,
  dependencies : boost_headers_deps,
  version : meson.project_version(),
)

boost_program_options_lib = static_library(
  'boost_program_options',
  files(
    'libs/program_options/src/cmdline.cpp',
    'libs/program_options/src/config_file.cpp',
    'libs/program_options/src/convert.cpp',
    'libs/program_options/src/options_description.cpp',
    'libs/program_options/src/parsers.cpp',
    'libs/program_options/src/positional_options.cpp',
    'libs/program_options/src/split.cpp',
    'libs/program_options/src/utf8_codecvt_facet.cpp',
    'libs/program_options/src/value_semantic.cpp',
    'libs/program_options/src/variables_map.cpp',
    'libs/program_options/src/winmain.cpp',
  ),
  dependencies : boost_headers_dep,
)

boost_program_options_dep = declare_dependency(
  link_with : boost_program_options_lib,
  dependencies : boost_headers_dep,
  version : meson.project_version(),
)

# Meson hands this same variable to every dependency('boost', modules : [...])
# call, whatever the requested modules, so it carries all compiled libraries.
boost_dep = declare_dependency(
  dependencies : [boost_headers_dep, boost_program_options_dep],
  version : meson.project_version(),
)
