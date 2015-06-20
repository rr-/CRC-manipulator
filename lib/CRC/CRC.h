#ifndef CRC_H
#define CRC_H
#include <functional>
#include "File/File.h"
#include "Progress.h"

/**
 * Just make it enough to hold any derived CRC.
 * Templates would be real PITA since no abstract pointers would be possible.
 */
typedef uint32_t CRCType;

class CRC
{
    private:
        size_t numBytes;
        CRCType polynomial;
        CRCType polynomialReverse;
        CRCType initialXOR;
        CRCType finalXOR;

        CRCType lookupTable[256];
        CRCType invLookupTable[256];

    public:
        CRC();
        virtual ~CRC();

        CRCType computeChecksum(File &inputFile, Progress &progress) const;

        void applyPatch(
            CRCType targetChecksum,
            File::OffsetType targetPosition,
            File &inputFile,
            File &outputFile,
            bool overwrite,
            Progress &writeProgress,
            Progress &checksumProgress) const;

        size_t getNumBytes() const;

    protected:
        CRC(size_t numBytes,
            CRCType polynomial,
            CRCType initialXOR,
            CRCType finalXOR);

    private:
        CRCType computePartialChecksum(
            File &inputFile,
            File::OffsetType startPosition,
            File::OffsetType endPosition,
            CRCType initialChecksum,
            Progress &progress) const;

        CRCType computeReversePartialChecksum(
            File &inputFile,
            File::OffsetType startPosition,
            File::OffsetType endPosition,
            CRCType initialChecksum,
            Progress &progress) const;

        virtual CRCType computePatch(
            CRCType targetChecksum,
            File::OffsetType targetPosition,
            File &inputFile,
            bool overwrite,
            Progress &progress) const;

        /**
         * The following methods calculate new checksums based on
         * previous checksum value and current input byte.
         */
        CRCType makeNextChecksum(CRCType checksum, uint8_t c) const;
        CRCType makePrevChecksum(CRCType checksum, uint8_t c) const;
};

#endif
