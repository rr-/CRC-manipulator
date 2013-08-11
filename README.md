CRC-manipulator
===============

crcmanip - CRC checksum manipulator 0.25
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
