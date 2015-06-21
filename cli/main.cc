#include <algorithm>
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>
#include "lib/CRCFactories.h"
#include "lib/File.h"
#include "lib/util.h"

namespace
{
    void printUsage(std::ostream &s, std::vector<std::shared_ptr<CRC>> crcs)
    {
        s << "CRC manipulator v" << CRCMANIP_VERSION << "\n";
        s << R"(
Freely reverse and change CRC checksums through smart file patching.
Usage: crcmanip p[atch] INFILE OUTFILE CHECKSUM [PATCH_OPTIONS]
   or: crcmanip c[alc]  INFILE [CALC_OPTIONS]
   or: crcmanip h[elp]

INFILE               path to input file
OUTFILE              path to output file
CHECKSUM             target checksum; must be a hexadecimal value

PATCH_OPTIONS can be:
-a, --algorithm ALG  specifies which algorithm to use
-p, --position NUM   position where to append the patch. Unless specified,
                     patch will be placed at the end of the input file.
                     If position is negative, patch will be placed at the
                     n-th byte from the end of file.
    --insert         specifies that patch should be inserted (default)
    --overwrite      specifies that patch should overwrite existing bytes

CALC_OPTIONS can be:
-a, --algorithm ALG  specifies which algorithm to use

Available algorithms:
)";

        bool isDefault = true;
        for (auto &crc : crcs)
        {
            std::cout
                << "* "
                << crc->getSpecs().name
                << (isDefault ? " (default)" : "")
                << "\n";
            isDefault = false;
        }

        s << R"(
Examples:
./crcmanip p input.txt output.txt 1234abcd
./crcmanip patch input.txt output.txt 1234abcd -p -1
./crcmanip calc input.txt -a CRC16IBM
)";
    }

    void validateChecksum(CRC &crc, const std::string &str)
    {
        size_t expectedDigits = crc.getSpecs().numBytes * 2;
        if (str.length() > expectedDigits)
        {
            throw std::runtime_error(
                "Error: Specified checksum has more than "
                + std::to_string(expectedDigits)
                + " digits.");
        }

        if (str.length() < expectedDigits)
        {
            std::cerr
                << "Warning: specified checksum has less than "
                << expectedDigits
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

    class Command
    {
        public:
            virtual void parse(std::vector<std::string> args) = 0;
            virtual void run() const = 0;
    };

    class CalculateCommand : public Command
    {
        public:
            CalculateCommand(std::vector<std::shared_ptr<CRC>> crcs);
            virtual void parse(std::vector<std::string> args);
            virtual void run() const;

        private:
            std::shared_ptr<CRC> crc;
            std::unique_ptr<File> inputFile;

            std::vector<std::shared_ptr<CRC>> crcs;
    };

    CalculateCommand::CalculateCommand(std::vector<std::shared_ptr<CRC>> crcs)
        : crcs(crcs)
    {
    }

    void CalculateCommand::parse(std::vector<std::string> args)
    {
        crc = crcs[0];

        if (args.size() < 1)
            throw std::runtime_error("No input file specified.");
        inputFile = File::fromFileName(
            args[0], File::Mode::Read | File::Mode::Binary);

        for (size_t i = 1; i < args.size(); i++)
        {
            auto &arg = args[i];
            if (arg == "-a" || arg == "--alg" || arg == "--algorithm")
            {
                if (i == args.size() - 1)
                    throw std::runtime_error(arg + " needs a parameter.");
                auto algo  = args[++i];
                auto it = std::find_if(
                    crcs.begin(), crcs.end(), [&](std::shared_ptr<CRC> crc)
                    { return crc->getSpecs().name == algo; });
                if (it == crcs.end())
                    throw std::runtime_error("Unknown algorithm: " + algo);
                crc = *it;
            }
        }
    }

    void CalculateCommand::run() const
    {
        Progress dummyProgress;
        auto checksum = crc->computeChecksum(*inputFile, dummyProgress);
        std::cout
            << std::hex
            << std::setw(crc->getSpecs().numBytes * 2)
            << std::setfill('0')
            << checksum
            << std::endl;
    }

    class PatchCommand : public Command
    {
        public:
            PatchCommand(std::vector<std::shared_ptr<CRC>> crcs);
            virtual void parse(std::vector<std::string> args);
            virtual void run() const;

        private:
            std::shared_ptr<CRC> crc;
            std::unique_ptr<File> inputFile;
            std::unique_ptr<File> outputFile;
            CRC::Value checksum;
            File::OffsetType position;
            bool positionSupplied;
            bool overwrite;

            std::vector<std::shared_ptr<CRC>> crcs;
    };

    PatchCommand::PatchCommand(std::vector<std::shared_ptr<CRC>> crcs)
        : crcs(crcs)
    {
    }

    void PatchCommand::parse(std::vector<std::string> args)
    {
        crc = crcs[0];
        positionSupplied = false;
        position = 0;
        overwrite = false;

        if (args.size() < 1)
            throw std::runtime_error("No input file specified.");
        inputFile = File::fromFileName(
            args[0], File::Mode::Read | File::Mode::Binary);

        if (args.size() < 2)
            throw std::runtime_error("No output file specified.");
        outputFile = File::fromFileName(
            args[1], File::Mode::Write | File::Mode::Binary);

        if (args.size() < 3)
            throw std::runtime_error("No checksum specified.");

        for (size_t i = 3; i < args.size(); i++)
        {
            auto &arg = args[i];
            if (arg == "--insert")
                overwrite = false;
            else if (arg == "--overwrite")
                overwrite = true;
            else if (arg == "-p" || arg == "--pos" || arg == "--position")
            {
                if (i == args.size() - 1)
                    throw std::runtime_error(arg + " needs a parameter.");
                auto pos = args[++i];
                positionSupplied = true;
                position = std::stoll(pos);
            }
            else if (arg == "-a" || arg == "--alg" || arg == "--algorithm")
            {
                if (i == args.size() - 1)
                    throw std::runtime_error(arg + " needs a parameter.");
                auto algo  = args[++i];
                auto it = std::find_if(
                    crcs.begin(), crcs.end(), [&](std::shared_ptr<CRC> crc)
                    { return crc->getSpecs().name == algo; });
                if (it == crcs.end())
                    throw std::runtime_error("Unknown algorithm: " + algo);
                crc = *it;
            }
        }

        validateChecksum(*crc, args[2]);
        checksum = std::stoull(args[2], nullptr, 16);
    }

    void PatchCommand::run() const
    {
        Progress writeProgress;
        writeProgress.started = []() { std::cout << "Output started\n"; };
        writeProgress.finished = []() { std::cout << "Output finished\n"; };

        Progress crcProgress;
        crcProgress.started = []() { std::cout << "Checksum started\n"; };
        crcProgress.finished = []() { std::cout << "Checksum finished\n"; };

        crcProgress.changed = writeProgress.changed
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

        auto correctedPosition = positionSupplied
            ? shiftUserPosition(
                position,
                inputFile->getSize(),
                crc->getSpecs().numBytes,
                overwrite)
            : computeAutoPosition(
                inputFile->getSize(),
                crc->getSpecs().numBytes,
                overwrite);

        crc->applyPatch(
            checksum,
            correctedPosition,
            *inputFile,
            *outputFile,
            overwrite,
            writeProgress,
            crcProgress);
    }
}

int main(int argc, char **argv)
{
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++)
        args.push_back(std::string(argv[i]));

    auto crcs = createAllCRC();

    for (auto &arg : args)
    {
        if (arg == "-h" || arg == "--help")
        {
            printUsage(std::cout, crcs);
            return 0;
        }
    }

    std::unique_ptr<Command> command;
    try
    {
        if (args.size() < 1)
            throw std::runtime_error("No command chosen.");

        auto cmdName = args[0];
        args.erase(args.begin());
        cmdName.erase(0, cmdName.find_first_not_of('-'));

        if (cmdName == "p" || cmdName == "patch")
            command.reset(new PatchCommand(crcs));
        else if (cmdName == "c" || cmdName == "calc" || cmdName == "calculate")
            command.reset(new CalculateCommand(crcs));
        else if (cmdName == "h" || cmdName == "help")
        {
            printUsage(std::cout, crcs);
            return 0;
        }
        else
            throw std::invalid_argument("Unknown command: " + cmdName);

        command->parse(args);
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << "\n";
        printUsage(std::cerr, crcs);
        return 1;
    }

    try
    {
        command->run();
        return 0;
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << "\n";
        return 1;
    }
}
