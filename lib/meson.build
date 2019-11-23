lib_src = files(
    'crc.cc',
    'crc_factories.cc',
    'file.cc',
    'progress.cc',
    'util.cc'
)

crcmanip = library('crcmanip',
                   sources: [lib_src, config_h],
                   install: true,
                   include_directories: incs)

crcmanip_dep = declare_dependency(link_with: crcmanip,
                                  include_directories: incs,
                                  sources: config_h)
