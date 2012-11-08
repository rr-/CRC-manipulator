CRC-manipulator
===============

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
Program will try to perform operations in place, but to make it possible,
the input file must be seekable. If it isn't seekable (for example, input
was piped - as opposed to file redirection), program is going to allocate
all the memory needed to load the input without any buffering.

Examples:
./crcmanip input.txt output.txt 1234abcd
./crcmanip input.txt output.txt 1234abcd -p -1
./crcmanip input.txt - 1234abcd >output.txt
./crcmanip - output.txt 1234abcd <input.txt
./crcmanip - - 1234abcd <input.txt >output.txt
cat input.txt|./crcmanip - output.txt 1234abcd
