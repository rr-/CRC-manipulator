#include <iostream>
#include <iomanip>
#include <vector>

#ifndef __unix
    #include <sys/fcntl.h> // for setmode
#endif

#include "CRC/CRC.h"
#include "CRC/CRC16CCITT.h"
#include "CRC/CRC16IBM.h"
#include "CRC/CRC32.h"
#include "File/File.h"

namespace
{
    void updateProgress(
        CRC::ProgressType progressType,
        File::OffsetType startPos,
        File::OffsetType curPos,
        File::OffsetType endPos)
    {
        static int i = 0;
        static CRC::ProgressType lastProgressType;

        if (lastProgressType != progressType || i == 0)
        {
            lastProgressType = progressType;
            switch (progressType)
            {
                case CRC::ProgressType::WriteStart:
                    std::cout << "Output started" << std::endl;
                    break;

                case CRC::ProgressType::WriteEnd:
                    std::cout << "Output ended" << std::endl;
                    break;

                case CRC::ProgressType::ChecksumStart:
                    std::cout << "Partial checksum started" << std::endl;
                    break;

                case CRC::ProgressType::ChecksumEnd:
                    std::cout << "Partial checksum ended" << std::endl;
                    break;

                default:
                    break;
            }
        }

        switch (progressType)
        {
            case CRC::ProgressType::WriteProgress:
            case CRC::ProgressType::ChecksumProgress:
                if (i % 2500 == 0)
                {
                    std::cout
                        << std::setw(5)
                        << std::fixed
                        << std::setprecision(2)
                        << (curPos - startPos) * 100.0f / (endPos - startPos)
                        << "% done\r";
                    std::cout.flush();
                }
                ++i;
                break;

            default:
                break;
        }
    }

    void printUsage(std::ostream &s)
    {
        s << "CRC manipulator v" << CRCMANIP_VERSION << "\n";
        s << R"(
Freely reverse and change CRC32 checksums through smart file patching.
Usage: crcmanip INFILE OUTFILE CHECKSUM [OPTIONS]

INFILE               input file. if -, standard input will be used.
OUTFILE              output file. if -, standard output will be used.
CHECKSUM             target checksum.

OPTIONS can be:
-h, --help           print this information.
-p, --position=NUM   position where to append the patch. Unless specified,
                     patch will be placed at the end of the input file.
                     If position is negative, patch will be placed at the
                     n-th byte from the end of file.
    --insert         specifies that patch should be inserted (default)
    --overwrite      specifies that patch should overwrite existing bytes
)";

        /*
        s << R"(
Available checksum algorithms:
    --crc32          use crc32 (default)
    --crc16-ibm      use crc16-ibm, also known as crc16-ansi and crc16
    --crc16-ccitt    use crc16-ccitt
    --init-xor=VAL   use custom VAL as initial XOR value
    --final-xor=VAL  use custom VAL as final XOR value
)";
        */

        s << R"(
CHECKSUM must be a hexadecimal value.
INFILE must be seekable stream. In other words, it cannot be a pipe
(particularly standard input), fifo etc.

Examples:
./crcmanip input.txt output.txt 1234abcd
./crcmanip input.txt output.txt 1234abcd -p -1
./crcmanip input.txt - 1234abcd >output.txt
./crcmanip - output.txt 1234abcd <input.txt
./crcmanip - - 1234abcd <input.txt >output.txt
)";
        exit(EXIT_FAILURE);
    }

    void validateChecksum(CRC &crc, const std::string &str)
    {
        if (str.length() > crc.getNumBytes() * 2)
        {
            std::cerr
                << "Error: Specified checksum has more than "
                << crc.getNumBytes() * 2
                << " digits.\n";
            exit(EXIT_FAILURE);
        }

        if (str.length() < crc.getNumBytes() * 2)
        {
            std::cerr
                << "Warning: specified checksum has less than "
                << crc.getNumBytes() * 2
                << " digits. Resulting checksum will be padded with 0.\n";
        }

        const std::string allowedCharacters = "0123456789abcdefABCDEF";
        for (size_t i = 0; i < str.length(); i++)
        {
            if (allowedCharacters.find(str[i]) == std::string::npos)
            {
                std::cerr
                    << "Error: Specified checksum contains invalid characters. "
                    << "Only hexadecimal values are accepted.\n";
                exit(EXIT_FAILURE);
            }
        }
    }

    class FacadeArgs
    {
        public:
            std::unique_ptr<CRC> crc;
            CRCType checksum;
            std::string inputPath;
            std::string outputPath;
            File::OffsetType position;
            bool automaticPosition;
            bool overwrite;
    };

    FacadeArgs parseArguments(std::vector<std::string> args)
    {
        FacadeArgs fa;
        fa.crc.reset(new CRC32);

        for (auto &arg : args)
        {
            if (arg == "-h" || arg == "--help")
            {
                printUsage(std::cout);
                exit(EXIT_SUCCESS);
            }
        }

        if (args.size() < 1)
        {
            std::cerr << "No input file specified.\n";
            printUsage(std::cerr);
            exit(EXIT_FAILURE);
        }
        if (args[0] != "")
            fa.inputPath = args[0];

        if (args.size() < 2)
        {
            std::cerr << "No output file specified.\n";
            printUsage(std::cerr);
            exit(EXIT_FAILURE);
        }
        if (args[1] != "-")
            fa.outputPath = args[1];

        if (args.size() < 3)
        {
            std::cerr << "No checksum specified.\n";
            printUsage(std::cerr);
            exit(EXIT_FAILURE);
        }

        validateChecksum(*fa.crc, args[2]);
        fa.checksum = std::stoull(args[2], nullptr, 16);

        fa.automaticPosition = true;
        fa.position = 0;

        for (size_t i = 3; i < args.size(); i++)
        {
            auto &arg = args[i];
            if (arg == "--insert")
                fa.overwrite = false;
            else if (arg == "--overwrite")
                fa.overwrite = true;
            else if (arg == "-p" || arg == "--pos" || arg == "--position")
            {
                if (i == args.size() - 1)
                {
                    std::cerr << "--position needs an additional parameter.\n";
                    printUsage(std::cerr);
                    exit(EXIT_FAILURE);
                }
                arg = args[++i];
                fa.automaticPosition = false;
                fa.position = std::stoll(arg);
            }
        }

        return fa;
    }

    void run(FacadeArgs &fa)
    {
        fa.crc->setProgressFunction(updateProgress);

        std::unique_ptr<File> inputFile;
        try
        {
            if (fa.inputPath == "")
            {
                #ifndef __unix
                    setmode(fileno(stdin), O_BINARY);
                #endif
                inputFile = File::fromFileHandle(stdin);
            }
            else
            {
                inputFile = File::fromFileName(
                    fa.inputPath, File::Mode::Read | File::Mode::Binary);
            }
        }
        catch (...)
        {
            std::cerr << "Failed to open " << fa.inputPath << " for reading.\n";
            exit(EXIT_FAILURE);
        }

        auto totalSize = inputFile->getSize();
        if (!fa.overwrite)
            totalSize += fa.crc->getNumBytes();

        if (fa.automaticPosition)
        {
            fa.position = fa.overwrite
                ? totalSize - fa.crc->getNumBytes()
                : totalSize;
        }

        if (fa.position < 0)
            fa.position += totalSize;

        if (fa.position < 0 ||
            fa.position + static_cast<File::OffsetType>(fa.crc->getNumBytes())
                > totalSize)
        {
            std::cerr << "Patch position is located outside available input.\n";
            exit(EXIT_FAILURE);
        }

        std::unique_ptr<File> outputFile;
        try
        {
            if (fa.outputPath == "")
            {
                #ifndef __unix
                    setmode(fileno(stdout), O_BINARY);
                #endif
                outputFile = File::fromFileHandle(stdout);
            }
            else
            {
                outputFile = File::fromFileName(
                    fa.outputPath, File::Mode::Write | File::Mode::Binary);
            }
        }
        catch (...)
        {
            std::cerr
                << "Failed to open " << fa.outputPath << " for writing.\n";
            exit(EXIT_FAILURE);
        }

        fa.crc->applyPatch(
            fa.checksum, fa.position, *inputFile, *outputFile, fa.overwrite);

        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char **argv)
{
    std::vector<std::string> args;
    for (int i = 1; i < argc; i ++)
        args.push_back(std::string(argv[i]));

    auto fa = parseArguments(args);
    run(fa);
}
