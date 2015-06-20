#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

#include "CRC/CRC.h"
#include "CRC/CRC16CCITT.h"
#include "CRC/CRC16IBM.h"
#include "CRC/CRC32.h"
#include "File/File.h"
#include "util.h"

namespace
{
    std::vector<std::shared_ptr<CRC>> getAllCrcs()
    {
        std::vector<std::shared_ptr<CRC>> crcs;
        crcs.push_back(std::shared_ptr<CRC>(new CRC32));
        crcs.push_back(std::shared_ptr<CRC>(new CRC16CCITT));
        crcs.push_back(std::shared_ptr<CRC>(new CRC16IBM));
        return crcs;
    }

    void printUsage(std::ostream &s, std::vector<std::shared_ptr<CRC>> crcs)
    {
        s << "CRC manipulator v" << CRCMANIP_VERSION << "\n";
        s << R"(
Freely reverse and change CRC checksums through smart file patching.
Usage: crcmanip INFILE OUTFILE CHECKSUM [OPTIONS]

INFILE               path to input file.
OUTFILE              path to output file.
CHECKSUM             target checksum.

OPTIONS can be:
-h, --help           print this information.
-p, --position=NUM   position where to append the patch. Unless specified,
                     patch will be placed at the end of the input file.
                     If position is negative, patch will be placed at the
                     n-th byte from the end of file.
    --insert         specifies that patch should be inserted (default)
    --overwrite      specifies that patch should overwrite existing bytes
-a, --algorithm=ALG  specifies which algorithm to use.

Available algorithms:
)";

        bool isDefault = true;
        for (auto &crc : crcs)
        {
            std::cout
                << "* "
                << crc->getName()
                << (isDefault ? " (default)" : "")
                << "\n";
            isDefault = false;
        }

        s << R"(
CHECKSUM must be a hexadecimal value.

Examples:
./crcmanip input.txt output.txt 1234abcd
./crcmanip input.txt output.txt 1234abcd -p -1
)";
    }

    void validateChecksum(CRC &crc, const std::string &str)
    {
        if (str.length() > crc.getNumBytes() * 2)
        {
            throw std::runtime_error(
                "Error: Specified checksum has more than "
                + std::to_string(crc.getNumBytes() * 2)
                + " digits.");
        }

        if (str.length() < crc.getNumBytes() * 2)
        {
            std::cerr
                << "Warning: specified checksum has less than "
                << crc.getNumBytes() * 2
                << " digits. Resulting checksum will be padded with 0.\n";
        }

        const std::string allowedCharacters = "0123456789abcdefABCDEF";
        for (auto &c : str)
        {
            if (allowedCharacters.find(c) == std::string::npos)
            {
                throw std::runtime_error(
                    "Error: Specified checksum contains invalid characters.\n"
                    "Only hexadecimal values are accepted.\n");
            }
        }
    }

    class FacadeArgs
    {
        public:
            std::shared_ptr<CRC> crc;
            CRCType checksum;
            std::string inputPath;
            std::string outputPath;
            File::OffsetType position;
            bool automaticPosition;
            bool overwrite;
    };

    FacadeArgs parseArguments(
        std::vector<std::string> args, std::vector<std::shared_ptr<CRC>> crcs)
    {
        FacadeArgs fa;
        fa.crc = crcs[0];
        fa.automaticPosition = true;
        fa.position = 0;
        fa.overwrite = false;
        fa.inputPath = "";
        fa.outputPath = "";

        for (auto &arg : args)
        {
            if (arg == "-h" || arg == "--help")
            {
                printUsage(std::cout, crcs);
                exit(EXIT_SUCCESS);
            }
        }

        try
        {
            if (args.size() < 1)
                throw std::runtime_error("No input file specified.");
            fa.inputPath = args[0];

            if (args.size() < 2)
                throw std::runtime_error("No output file specified.");
            fa.outputPath = args[1];

            if (args.size() < 3)
                throw std::runtime_error("No checksum specified.");

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
                        throw std::runtime_error(arg + " needs a parameter.");
                    auto pos = args[++i];
                    fa.automaticPosition = false;
                    fa.position = std::stoll(pos);
                }
                else if (arg == "-a" || arg == "--alg" || arg == "--algorithm")
                {
                    if (i == args.size() - 1)
                        throw std::runtime_error(arg + " needs a parameter.");
                    auto algo  = args[++i];
                    bool found = false;
                    for (auto &crc : crcs)
                    {
                        if (crc->getName() == algo)
                        {
                            fa.crc = crc;
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                        throw std::runtime_error("Unknown algorithm: " + algo);
                }
            }

            validateChecksum(*fa.crc, args[2]);
            fa.checksum = std::stoull(args[2], nullptr, 16);
        }
        catch (std::exception &e)
        {
            std::cerr << e.what() << "\n";
            printUsage(std::cerr, crcs);
            exit(EXIT_FAILURE);
        }
        return fa;
    }

    void run(FacadeArgs &fa)
    {
        Progress writeProgress;
        writeProgress.started
            = []() { std::cout << "Output started" << std::endl; };
        writeProgress.finished
            = []() { std::cout << "Output finished" << std::endl; };

        Progress checksumProgress;
        checksumProgress.started
            = []() { std::cout << "Partial checksum started" << std::endl; };
        checksumProgress.finished
            = []() { std::cout << "Partial checksum finished" << std::endl; };

        checksumProgress.changed = writeProgress.changed
            = [](double percentage)
                {
                    std::cout
                        << std::setw(5)
                        << std::fixed
                        << std::setprecision(2)
                        << percentage
                        << "% done\r";
                        std::cout.flush();
                };

        std::unique_ptr<File> inputFile;
        try
        {
            inputFile = File::fromFileName(
                fa.inputPath, File::Mode::Read | File::Mode::Binary);
        }
        catch (...)
        {
            std::cerr << "Failed to open " << fa.inputPath << " for reading.\n";
            exit(EXIT_FAILURE);
        }

        if (fa.automaticPosition)
        {
            fa.position = computeAutoPosition(
                inputFile->getSize(), fa.crc->getNumBytes(), fa.overwrite);
        }
        else
        {
            fa.position = shiftUserPosition(
                fa.position,
                inputFile->getSize(),
                fa.crc->getNumBytes(),
                fa.overwrite);
        }

        std::unique_ptr<File> outputFile;
        try
        {
            outputFile = File::fromFileName(
                fa.outputPath, File::Mode::Write | File::Mode::Binary);
        }
        catch (...)
        {
            std::cerr
                << "Failed to open " << fa.outputPath << " for writing.\n";
            exit(EXIT_FAILURE);
        }

        fa.crc->applyPatch(
            fa.checksum,
            fa.position,
            *inputFile,
            *outputFile,
            fa.overwrite,
            writeProgress,
            checksumProgress);

        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char **argv)
{
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++)
        args.push_back(std::string(argv[i]));

    auto crcs = getAllCrcs();
    auto fa = parseArguments(args, crcs);
    run(fa);
}
