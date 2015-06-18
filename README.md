CRC-manipulator
===============

A tool that lets you reverse and freely change CRC32 checksums through smart
file patching.

### How it works

It appends a few bytes at the end of the file so that the file checksum
computes to given hexadecimal hash.

### Features

- CRC32, but might add support for other CRC checksums in the future.
- Available for GNU/Linux and Windows.
- Simple GUI (for more advanced options, use CLI version).

### Binaries for Windows

To download precompiled binaries for Windows, head over to
[releases](https://github.com/rr-/CRC-manipulator/releases).

### Compiling

1. Make sure you have `g++` and if you wish to compile GUI frontend - `qt4`.
2. Run following:

        ./bootstrap
        ./waf configure
        ./waf build

### Cross compiling for Windows

1. Make sure you have `mingw-w64` and if you with to compile GUI frontend -
   `qt4` (compiled with MinGW-w64).  
   **Arch Linux**: to get these, you can install `mingw-w64-gcc` from main
   repositories and `mingw-w64-qt4` + dependencies from AUR.
   **mxe**: I had no luck with linking to `mxe`'s qt version.
2. Run following:

        ./bootstrap

        CROSS_COMPILE=i686-w64-mingw32-
        export CC=${CROSS_COMPILE}gcc
        export CXX=${CROSS_COMPILE}g++
        export AR=${CROSS_COMPILE}ar
        export PKGCONFIG=${CROSS_COMPILE}pkg-config

        ./waf configure
        ./waf build
