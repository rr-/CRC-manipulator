gui_headers = [
    'drop_line_edit.h',
    'main_window.h'
]

processed = qt5_mod.preprocess(
    moc_headers : gui_headers,
    ui_files: 'main_window.ui'
)

gui_src = [
    'main.cc',
    'main_window.cc',
    'drop_line_edit.cc',
    processed
]

win_dep = []
if host_system == 'windows'
    ldflags += '-Wl,--allow-multiple-definition'
    mxe_qt5 = mxe_path / 'qt5/'
    win_dep += cxx.find_library('qwindows', dirs: mxe_qt5 / 'plugins/platforms')
    deps = [
        'Qt5ThemeSupport',
        'Qt5EventDispatcherSupport',
        'Qt5FontDatabaseSupport',
        'Qt5WindowsUIAutomationSupport',
        'wtsapi32'
    ]
    foreach d : deps
        win_dep += cxx.find_library(d, dirs: mxe_qt5 / 'lib')
    endforeach
endif

crcmanip_gui  = executable(
    'crcmanip-gui',
    sources: gui_src,
    include_directories: incs,
    gui_app: true,
    install: true,
    dependencies: [qt5widgets, crcmanip_dep, win_dep],
    link_args: ldflags,
)
