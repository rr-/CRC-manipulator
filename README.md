CRC-manipulator
===============

<pre>
crcmanip - CRC checksum manipulator
Freely manipulate CRC32 checksums through smart file patching.
Usage: crcmanip INFILE OUTFILE CHECKSUM [OPTIONS]

INFILE               input file. if -, standard input will be used.
OUTFILE              output file. if -, standard output will be used.
CHECKSUM             desired checksum.

OPTIONS can be:
-h, --help           print this information.
-p, --position=NUM   position where to append the patch. Unless specified,
                     patch will be placed at the end of the input file.
                     If position is negative, patch will be placed at the
                     n-th byte from the end of file.
    --insert         specifies that patch should be inserted (default)
    --overwrite      specifies that patch should overwrite existing bytes

CHECKSUM must be a hexadecimal value.
INFILE must be seekable stream. In other words, it cannot be a pipe
(particularly standard input), fifo etc.

Examples:
./crcmanip input.txt output.txt 1234abcd
./crcmanip input.txt output.txt 1234abcd -p -1
./crcmanip input.txt - 1234abcd >output.txt
./crcmanip - output.txt 1234abcd <input.txt
./crcmanip - - 1234abcd <input.txt >output.txt
</pre>

### Compiling

1. Get `g++` and `qt4` if you wish to compile GUI version.
2. Run following commands:

        ./bootstrap
        ./waf configure
        ./waf build

### Cross compiling for Windows

1. Get `mingw-w64` and `qt4` if you wish to compile GUI version.
   For Arch: `mingw-w64-gcc` from main repositories and `mingw-w64-qt4` +
   dependencies from AUR. I had no luck with linking to `mxe`'s version.
2. Run following:

        CROSS_COMPILE=i686-w64-mingw32-
        export CC=${CROSS_COMPILE}gcc
        export CXX=${CROSS_COMPILE}g++
        export AR=${CROSS_COMPILE}ar
        export PKGCONFIG=${CROSS_COMPILE}pkg-config

        ./waf configure && ./waf build
