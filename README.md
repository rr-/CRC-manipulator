CRC-manipulator
===============

A tool that lets you reverse and freely change CRC checksums through smart
file patching.

### How it works

It appends a few bytes at the end of the file so that the file checksum
computes to given hexadecimal hash.

### Features

- Patching and calculating CRC checksums for:
  - CRC32
  - CRC32POSIX (`cksum` from GNU coreutils)
  - CRC16CCITT
  - CRC16IBM
- Available for GNU/Linux and Windows.
- Minimal GUI (supports CRC32 only; for more advanced options, use CLI version).

### Seeing it in action

**GUI version**:

![GUI version](http://i.imgur.com/A5LzhBs.png)

**CLI version**:

![CLI version](http://i.imgur.com/a7YTbxk.png)

### Binaries for Windows

To download precompiled binaries for Windows, head over to
[releases](https://github.com/rr-/CRC-manipulator/releases).

### Compiling

1. Make sure you have `g++` `python3-setuptools` and `python3-wheel`
packages installed

2. Install Meson and Ninja

        pip3 install meson ninja

3. Run following

        meson build --buildtype release
        ninja -C build

The `crcmanip-gui` is not compiled by default. If you want to compile it, make
sure you have `qt5` packages installed. Then replace the third step above
with this one:

        meson build -Dgui=true --buildtype release
        ninja -C build

### Cross-compiling for Windows without GUI

1. Install `mingw-w64`

2. Replace the content of the `cross_mingw_i686.txt` and
`cross_mingw_x86_64.txt` files with the one of the `mingw-w64` files
that you can find [here](https://github.com/mesonbuild/meson/tree/master/cross)

3. Build the binaries

    Windows 32-bit

        meson build_windows32 --buildtype release \
                              --cross-file cross_mingw_i686.txt
        ninja -C build_windows32

    Windows 64-bit

        meson build_windows64 --buildtype release \
                              --cross-file cross_mingw_x86_64.txt
        ninja -C build_windows64

### Cross-compiling for Windows with GUI

1. Install `mxe`

2. Add the `mxe` path to the environment variable `PATH`

3. Install the static version of the qt5 library

    Windows 32-bit

        cd your/path/to/mxe/
        make MXE_TARGETS=i686-w64-mingw32.static qt5

    Windows 64-bit

        cd your/path/to/mxe/
        make MXE_TARGETS=x86_64-w64-mingw32.static qt5

4. Build the binaries

    Windows 32-bit

        meson build_windows32 --buildtype release \
                              -Dgui=true \
                              -Dmxe='/your/path/to/mxe/32-bit' \
                              --cross-file cross_mingw_i686.txt
        ninja -C build_windows32

    Windows 64-bit

        meson build_windows64 --buildtype release \
                              -Dgui=true \
                              -Dmxe='/your/path/to/mxe/64-bit' \
                              --cross-file cross_mingw_x86_64.txt
        ninja -C build_windows64

### Building tests

To build tests, simply add the `-Dtests=true` option to the `meson` command
