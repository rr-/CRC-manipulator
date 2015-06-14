#if defined __linux__
#elif defined _WIN32 || defined _WIN64 || defined __CYGWIN__
    #include <Windows.h>
#else
    #error unknown platform
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#ifndef __unix
    #include <sys/fcntl.h> // for setmode
#endif

#include "version.h"
#include "CRC/CRC.h"
#include "CRC/CRC32.h"
#include "CRC/CRC16IBM.h"
#include "CRC/CRC16CCITT.h"

const char *getPathToSelf()
{
    const size_t bufferSize = 256;
    static char buffer[bufferSize];
    if (false);
    #if defined __linux__
    else if (readlink("/proc/self/exe", buffer, bufferSize));
    else if (readlink("/proc/curproc/file", buffer, bufferSize));
    else if (readlink("/proc/self/path/a.out", buffer, bufferSize));
    #elif defined _WIN32 || defined _WIN64
    else if (GetModuleFileName(NULL, buffer, bufferSize));
    #endif
    else strcpy(buffer, "crcmanip");

    char *slash = NULL;
    slash = strrchr(buffer, '/');
    if (slash != NULL)
    {
        strcpy(buffer, slash + 1);
    }
    slash = strrchr(buffer, '\\');
    if (slash != NULL)
    {
        strcpy(buffer, slash + 1);
    }
    return buffer;
}

const char *getVersion()
{
    const size_t bufferSize = 256;
    static char buffer[bufferSize];
    sprintf(buffer, "%d.%d", MAJOR_VERSION, MINOR_VERSION);
    return buffer;
}

void updateProgress(
    const CRC::CRCProgressType &progressType,
    const File::OffsetType &startPos,
    const File::OffsetType &curPos,
    const File::OffsetType &endPos)
{
    static int i = 0;
    static CRC::CRCProgressType lastProgressType;

    if (lastProgressType != progressType || i == 0)
    {
        lastProgressType = progressType;
        switch (progressType)
        {
            case CRC::CRCPROG_WRITE_START:
                fprintf(stderr, "Output started\n");
                break;

            case CRC::CRCPROG_WRITE_END:
                fprintf(stderr, "Output ended\n");
                break;

            case CRC::CRCPROG_CHECKSUM_START:
                fprintf(stderr, "Partial checksum started\n");
                break;

            case CRC::CRCPROG_CHECKSUM_END:
                fprintf(stderr, "Partial checksum ended\n");
                break;

            default:
                break;
        }
    }

    switch (progressType)
    {
        case CRC::CRCPROG_WRITE_PROGRESS:
        case CRC::CRCPROG_CHECKSUM_PROGRESS:
            if (i % 1000 == 0)
            {
                fprintf(
                    stderr,
                    "%6.02f%%\r",
                    (curPos - startPos) * 100.0f / (endPos - startPos));
            }
            ++ i;
            break;

        default:
            break;
    }
}

void usage(FILE *where)
{
    fprintf(where, "%s - CRC checksum manipulator %s\n", getPathToSelf(), getVersion());
    fprintf(where, "Freely manipulate CRC32 checksums through smart file patching.\n");
    fprintf(where, "Usage: %s INFILE OUTFILE CHECKSUM [OPTIONS]\n", getPathToSelf());
    fputs("\n", where);
    fputs("INFILE               input file. if -, standard input will be used.\n", where);
    fputs("OUTFILE              output file. if -, standard output will be used.\n", where);
    fputs("CHECKSUM             desired checksum.\n\n", where);
    fputs("OPTIONS can be:\n", where);
    fputs("-h, --help           print this information.\n", where);
    fputs("-p, --position=NUM   position where to append the patch. Unless specified,\n", where);
    fputs("                     patch will be placed at the end of the input file.\n", where);
    fputs("                     If position is negative, patch will be placed at the\n", where);
    fputs("                     n-th byte from the end of file.\n", where);
    fputs("    --insert         specifies that patch should be inserted (default)\n", where);
    fputs("    --overwrite      specifies that patch should overwrite existing bytes\n", where);
    fputs("\n", where);
    /*
    fputs("Available checksum algorithms:\n", where);
    fputs("    --crc32          use crc32 (default)\n", where);
    fputs("    --crc16-ibm      use crc16-ibm, also known as crc16-ansi and crc16\n", where);
    fputs("    --crc16-ccitt    use crc16-ccitt\n", where);
    fputs("    --init-xor=VAL   use custom VAL as initial XOR value\n", where);
    fputs("    --final-xor=VAL  use custom VAL as final XOR value\n", where);
    fputs("\n", where);
    */
    fputs("CHECKSUM must be a hexadecimal value.\n", where);
    fputs("INFILE must be seekable stream. In other words, it cannot be a pipe\n", where);
    fputs("(particularly standard input), fifo etc.\n", where);
    fputs("\n", where);
    fputs("Examples:\n", where);
    fputs("./crcmanip input.txt output.txt 1234abcd\n", where);
    fputs("./crcmanip input.txt output.txt 1234abcd -p -1\n", where);
    fputs("./crcmanip input.txt - 1234abcd >output.txt\n", where);
    fputs("./crcmanip - output.txt 1234abcd <input.txt\n", where);
    fputs("./crcmanip - - 1234abcd <input.txt >output.txt\n", where);
    exit(EXIT_FAILURE);
}

void validateChecksum(CRC &activeCRC, const char *str, CRCType &checksum)
{
    if (strlen(str) > activeCRC.getNumBytes() * 2)
    {
        fprintf(stderr,
            "Error: Specified checksum has more than %zd digits.\n",
            activeCRC.getNumBytes() * 2);
        exit(EXIT_FAILURE);
    }
    if (strlen(str) < activeCRC.getNumBytes() * 2)
    {
        fprintf(stderr,
            "Warning: specified checksum has less than %zd digits. "
            "Resulting checksum will be padded with 0.\n",
            activeCRC.getNumBytes() * 2);
    }
    const char *allowedCharacters = "0123456789abcdefABCDEF";
    for (size_t i = 0; i < strlen(str); i ++)
    {
        if (strchr(allowedCharacters, str[i]) == NULL)
        {
            fprintf(stderr,
                "Error: Specified checksum contains invalid characters. "
                "Only hexadecimal values are accepted.\n");
            exit(EXIT_FAILURE);
        }
    }
    checksum = strtoul(str, NULL, 16);
}

int main(int argc, char **argv)
{
    CRC *activeCRC = new CRC32();
    activeCRC->setProgressFunction(&updateProgress);

    for (int i = 1; i < argc; i ++)
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            usage(stdout);
            exit(EXIT_SUCCESS);
        }
    }

    if (argc < 2)
    {
        fprintf(stderr, "No input file specified.\n");
        usage(stderr);
        exit(EXIT_FAILURE);
    }
    char *pathIn = argv[1];
    if (strcmp(pathIn, "-") == 0)
    {
        pathIn = NULL;
    }

    if (argc < 3)
    {
        fprintf(stderr, "No output file specified.\n");
        usage(stderr);
        exit(EXIT_FAILURE);
    }
    char *pathOut = argv[2];
    if (strcmp(pathOut, "-") == 0)
    {
        pathOut = NULL;
    }

    if (argc < 4)
    {
        fprintf(stderr, "No checksum specified.\n");
        usage(stderr);
        exit(EXIT_FAILURE);
    }

    uint32_t desiredChecksum = 0x12345678;
    validateChecksum(*activeCRC, argv[3], desiredChecksum);

    bool overwrite = false;
    bool desiredPositionSpecified = false;
    File::OffsetType desiredPosition = -1;

    for (int i = 4; i < argc; i ++)
    {
        if (strcmp(argv[i], "--insert") == 0)
        {
            overwrite = false;
            continue;
        }
        if (strcmp(argv[i], "--overwrite") == 0)
        {
            overwrite = true;
            continue;
        }
        if (strcmp(argv[i], "-p") == 0 ||
            strcmp(argv[i], "--pos") == 0 ||
            strcmp(argv[i], "--position") == 0)
        {
            if (i == argc - 1)
            {
                fprintf(stderr, "--position needs an additional parameter.");
                usage(stderr);
                exit(EXIT_FAILURE);
            }
            ++ i;
            if (strcmp(argv[i], "-0") != 0)
            {
                desiredPosition = sizeof(desiredPosition) == sizeof(long long)
                    ? atoll(argv[i])
                    : atol(argv[i]);
                desiredPositionSpecified = true;
            }
            continue;
        }
    }

    // Open the input file
    File *inputFile = NULL;
    try
    {
        if (pathIn == NULL)
        {
            #ifndef __unix
                setmode(fileno(stdin), O_BINARY);
            #endif
            inputFile = File::fromFileHandle(stdin);
        }
        else
        {
            inputFile = File::fromFileName(
                pathIn, File::FOPEN_READ | File::FOPEN_BINARY);
        }
    }
    catch (...)
    {
        fprintf(stderr, "Failed to open %s for reading.\n", pathIn);
        exit(EXIT_FAILURE);
    }
    File::OffsetType totalSize = inputFile->getFileSize();

    if (!desiredPositionSpecified)
    {
        if (overwrite)
        {
            desiredPosition = totalSize - activeCRC->getNumBytes();
        }
        else
        {
            desiredPosition = totalSize;
        }
    }
    else
    {
        if (desiredPosition < 0)
        {
            desiredPosition += totalSize;
        }
    }
    if (!overwrite)
    {
        totalSize += activeCRC->getNumBytes();
    }
    if (desiredPosition < 0 ||
        desiredPosition + ((File::OffsetType) activeCRC->getNumBytes()) > totalSize)
    {
        fputs("Patch position is located outside available input.\n", stderr);
        exit(EXIT_FAILURE);
    }

    // Open the output file
    File *outputFile = NULL;
    try
    {
        if (pathOut == NULL)
        {
            #ifndef __unix
                setmode(fileno(stdout), O_BINARY);
            #endif
            outputFile = File::fromFileHandle(stdout);
        }
        else
        {
            outputFile = File::fromFileName(
                pathOut, File::FOPEN_WRITE | File::FOPEN_BINARY);
        }
    }
    catch (...)
    {
        fprintf(stderr, "Failed to open %s for writing.\n", pathOut);
        exit(EXIT_FAILURE);
    }

    // Perform desired operation
    activeCRC->applyPatch(
        desiredChecksum,
        desiredPosition,
        *inputFile,
        *outputFile,
        overwrite);

    // Cleanup
    delete activeCRC;
    delete inputFile;
    delete outputFile;
    exit(EXIT_SUCCESS);
}
