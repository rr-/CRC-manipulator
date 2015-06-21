#ifndef CRC_H
#define CRC_H
#include <functional>
#include "File.h"
#include "Progress.h"

class CRC final
{
    public:
        /**
         * Rather than using templates, we're making sure CRC type is big
         * enough to hold any possible value. Because of this, one can
         * conveniently reference any kind of CRC without knowing its size.
         */
        typedef uint32_t Value;

        enum Flags
        {
            BigEndian   = 1,
            UseFileSize = 2,
        };

        typedef struct
        {
            std::string name;
            size_t numBytes;
            Value polynomial;
            Value initialXOR;
            Value finalXOR;
            Value test;
            int flags;
        } Specs;

    private:
        Specs specs;
        Value lookupTable[256];
        Value invLookupTable[256];

    public:
        CRC(const Specs &specs);
        ~CRC();

        Value computeChecksum(File &inputFile, Progress &progress) const;

        void applyPatch(
            Value targetChecksum,
            File::OffsetType targetPosition,
            File &inputFile,
            File &outputFile,
            bool overwrite,
            Progress &writeProgress,
            Progress &checksumProgress) const;

        const Specs &getSpecs() const;

    private:
        Value computePartialChecksum(
            File &inputFile,
            File::OffsetType startPosition,
            File::OffsetType endPosition,
            Value initialChecksum,
            Progress &progress) const;

        Value computeReversePartialChecksum(
            File &inputFile,
            File::OffsetType startPosition,
            File::OffsetType endPosition,
            Value initialChecksum,
            Progress &progress) const;

        Value computePatch(
            Value targetChecksum,
            File::OffsetType targetPosition,
            File &inputFile,
            bool overwrite,
            Progress &progress) const;

        /**
         * The following methods calculate new checksums based on
         * previous checksum value and current input byte.
         */
        Value makeNextChecksum(Value checksum, uint8_t c) const;
        Value makePrevChecksum(Value checksum, uint8_t c) const;
};

#endif
