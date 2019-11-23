test_headers = files(
    'catch.hh',
    'test_crc_support.h'
)

test_src = files(
    'main.cc',
    'test_crc.cc',
    'test_crc_support.cc',
    'test_file.cc',
    'test_position.cc'
)

crcmanip_test = executable(
    'crcmanip-tests',
    sources: test_src,
    install: false,
    include_directories: incs,
    link_with: crcmanip_lib
)
