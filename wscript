# vi: ft=python
from waflib import Logs
import os

APPNAME = 'crcmanip'
try:
    VERSION = os.popen('git describe --tags').read().strip()
    VERSION_LONG = os.popen('git describe --always --dirty --long --tags').read().strip()
except:
    VERSION = '0.0'
    VERSION_LONG = '?'

def options(ctx):
    ctx.load(['compiler_cxx', 'qt4'])

    ctx.add_option(
        '-d',
        '--debug',
        dest = 'debug',
        default = False,
        action = 'store_true',
        help = 'enable emitting debug information')

    ctx.add_option(
        '--static',
        dest = 'static',
        default = False,
        action = 'store_true',
        help = 'enable static linking')

def configure_version(ctx):
    ctx.env.DEFINES = [ 'CRCMANIP_VERSION="' + VERSION_LONG + '"' ]

def configure_flags(ctx):
    ctx.env.CXXFLAGS = [
        '-Wall',
        '-Wextra',
        '-pedantic',
        '--std=c++11'
    ]

    if ctx.options.static:
        ctx.env.LINKFLAGS = [
            '-static-libgcc',
            '-static-libstdc++',
        ]
        Logs.info('Static linking enabled')
    else:
        Logs.info('Static linking disabled, pass --static to enable')

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
    configure_version(ctx)
    configure_flags(ctx)
    configure_features(ctx)
    try:
        ctx.load(['qt4'])
    except:
        pass
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
        target   = 'common',
        cxxflags = ['-iquote', lib_path],
        includes = ['.'])

    ctx.program(
        source   = cli_sources,
        target   = 'crcmanip-cli',
        includes = ['.'],
        cxxflags = ['-iquote', cli_path, '-iquote', lib_path],
        use      = [ 'common' ])

    if getattr(ctx.env, 'HAVE_QTCORE', False):
        ctx.program(
            source    = gui_sources,
            target    = 'crcmanip-gui',
            includes  = [ '.' ],
            cxxflags  = ['-iquote', gui_path, '-iquote', lib_path],
            linkflags = [ '-Wl,-subsystem,windows' if ctx.env.DEST_OS == 'win32' else '' ],
            defines   = [ 'WAF' ],
            features  = [ 'qt4' ],
            use       = [ 'QTCORE', 'QTGUI', 'common' ])

def dist(ctx):
    ctx.algo = 'zip'
    ctx.files = ctx.path.ant_glob('build')
