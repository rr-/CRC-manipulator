# vi: ft=python
from waflib import Logs
import os

APPNAME = 'crcmanip'
VERSION = '0.26'

def options(ctx):
    ctx.load(['compiler_cxx', 'qt4'])

    ctx.add_option(
        '-d',
        '--debug',
        dest = 'debug',
        default = False,
        action = 'store_true',
        help = 'enable emitting debug information')

def configure_flags(ctx):
    ctx.env.CXXFLAGS = [
        '-Wall',
        '-Wextra',
        '-pedantic',
        '--std=c++11',
    ]

    if ctx.options.debug:
        ctx.env.CXXFLAGS += ['-g']
        if 'gcc' in ctx.env.CXX_NAME or 'g++' in ctx.env.CXX_NAME:
            ctx.env.CXXFLAGS += ['-ggdb']
        Logs.info('Debug information enabled')
    else:
        Logs.info('Debug information disabled, pass -d to enable')

def configure_features(ctx):
    seek_funcs = ['fseeko64', 'fseeko', '_fseeki64']
    for f in seek_funcs:
        ctx.check_cxx(
            msg = 'Checking for ' + f + ' support',
            fragment = "#include <cstdio>\nint main(){" + f + '(0, 0, 0); }',
            define_name = f.strip('_').upper(),
            mandatory = False)

    ctx.check_cxx(
        msg = 'Checking for off64_t support',
        fragment = "#include <cstdio>\nint main(){off64_t x=0;}",
        define_name = 'OFF64T',
        mandatory = False)

def configure(ctx):
    ctx.load(['compiler_cxx'])
    configure_flags(ctx)
    configure_features(ctx)
    ctx.load(['qt4'])
    ctx.write_config_header('config.h')

def build(ctx):
    lib_dir = 'lib'
    cli_dir = 'cli'
    gui_dir = 'gui'
    lib_path = ctx.path.find_node(lib_dir).abspath()
    cli_path = ctx.path.find_node(cli_dir).abspath()
    gui_path = ctx.path.find_node(gui_dir).abspath()

    lib_sources = ctx.path.ant_glob(lib_dir + '/**/*.cc')
    cli_sources = ctx.path.ant_glob(cli_dir + '/**/*.cc')
    gui_sources = ctx.path.ant_glob(gui_dir + '/**/*.cc') \
                + ctx.path.ant_glob(gui_dir + '/**/*.ui')

    ctx.objects(
        source   = lib_sources,
        target   = 'lib',
        cxxflags = ['-iquote', lib_path])

    ctx.program(
        source   = cli_sources,
        target   = 'crcmanip-cli',
        cxxflags = ['-iquote', cli_path, '-iquote', lib_path],
        includes = ['.'],
        use      = [ 'lib', ])

    ctx.program(
        source   = gui_sources,
        target   = 'crcmanip-gui',
        features = [ 'qt4' ],
        cxxflags = ['-iquote', gui_path, '-iquote', lib_path],
        includes = [ '.' ],
        defines  = [ 'WAF' ],
        use      = [ 'QTCORE', 'QTGUI', 'lib', ])

def dist(ctx):
    ctx.algo = 'zip'
    ctx.files = ctx.path.ant_glob('build')
