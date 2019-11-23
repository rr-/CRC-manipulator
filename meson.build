project(
    'crcmanip',
    'cpp',
    license: 'MIT',
    meson_version: '>= 0.49.0',
    default_options: ['cpp_std=c++11'],
    version: '0.5.2'
)

conf = configuration_data()
ldflags = []

# Set package version
output = '?'
r = run_command('git', 'describe', '--always', '--dirty', '--long', '--tags')
if r.returncode() == 0
    output = r.stdout().split('-')[0]
endif
conf.set_quoted('CRCMANIP_VERSION', output)

# Set compiler flags
cxx = meson.get_compiler('cpp')
cxxflags = [
    '-Wall',
    '-Wextra',
    '-pedantic'
]

add_project_arguments(cxx.get_supported_arguments(cxxflags), language: 'cpp')

# Qt5 dependency
qt5_mod = import('qt5')
qt5widgets = dependency('qt5', modules : 'Widgets')

host_system = host_machine.system()
if host_system == 'windows'
    if get_option('mxe') == ''
        error('You have to set an mxe path to compile the crcmanip-gui')
    else
        mxe_path = get_option('mxe')
    endif
endif

# Check functions
check_functions = [
    'fseeko64',
    'fseeko',
    '_fseeki64'
]

foreach name: check_functions
    if cxx.has_function(name)
        conf.set('HAVE_@0@'.format(name.to_upper()), 1)
    endif
endforeach

# Create config.h
config_h = configure_file(output: 'config.h', configuration: conf)

incs = include_directories('.', 'lib', 'gui')

subdir('lib')
subdir('cli')
subdir('gui')

if get_option('tests')
    main_url = 'https://raw.githubusercontent.com/'
    url = main_url + 'catchorg/Catch2/master/single_include/catch2/catch.hpp'
    r = run_command('wget', '-O', 'tests/catch.hh', url)
    if r.returncode() != 0
        error('Cannot download catch.hh')
    endif
    subdir('tests')
endif
