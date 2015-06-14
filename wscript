# vi: ft=python
from waflib import Logs
import os

APPNAME = 'crcmanip'
VERSION = '0.26'

def options(ctx):
    ctx.load('compiler_cxx')

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
        Logs.info('Debug information enabled')
    else:
        Logs.info('Debug information disabled, pass -d to enable')

    ctx.load('compiler_cxx')

def configure(ctx):
    configure_flags(ctx)

def build(ctx):
    lib_dir = 'lib'
    cli_dir = 'cli'
    lib_path = ctx.path.find_node(lib_dir).abspath()
    cli_path = ctx.path.find_node(cli_dir).abspath()

    lib_sources = ctx.path.ant_glob(lib_dir + '/**/*.cc')
    cli_sources = ctx.path.ant_glob(cli_dir + '/**/*.cc')

    ctx.objects(
        source = lib_sources,
        target = 'lib',
        cxxflags = ['-iquote', lib_path])

    ctx.program(
        source = cli_sources,
        target = 'crcmanip-cli',
        cxxflags = ['-iquote', cli_path, '-iquote', lib_path],
        use = [
            'lib',
        ])

def dist(ctx):
    ctx.algo = 'zip'
    ctx.files = ctx.path.ant_glob('build')
