#ifndef CRC_H
#define CRC_H

#include <functional>
#include "File/File.h"

/**
 * Just make it enough to hold any derived CRC.
 * Templates would be real PITA since no abstract pointers would be possible.
 */
typedef uint32_t CRCType;

class CRC
{
    //Types, constants
    public:
        enum class ProgressType : uint8_t
        {
            WriteStart,
            WriteProgress,
            WriteEnd,
            ChecksumStart,
            ChecksumProgress,
            ChecksumEnd
        };

        typedef std::function<void(
            ProgressType progressType,
            File::OffsetType startPosition,
            File::OffsetType currentPosition,
            File::OffsetType endPosition)>
        ProgressFunction;

        CRC();
        virtual ~CRC();

        void setProgressFunction(const ProgressFunction &progressFunction);

        CRCType computeChecksum(File &inputFile) const;

        void applyPatch(
            CRCType targetChecksum,
            File::OffsetType targetPosition,
            File &inputFile,
            File &outputFile,
            bool overwrite = false) const;

        size_t getNumBytes() const;

    protected:
        CRC(size_t numBytes,
            CRCType polynomial,
            CRCType initialXOR,
            CRCType finalXOR);
        size_t numBytes;
        CRCType polynomial;
        CRCType polynomialReverse;
        CRCType initialXOR;
        CRCType finalXOR;

        void markProgress(
            ProgressType progressType,
            File::OffsetType startPosition,
            File::OffsetType currentPosition,
            File::OffsetType endPosition) const;

        CRCType computePartialChecksum(
            File &inputFile,
            File::OffsetType startPosition,
            File::OffsetType endPosition,
            CRCType initialChecksum = 0) const;

        CRCType computeReversePartialChecksum(
            File &inputFile,
            File::OffsetType startPosition,
            File::OffsetType endPosition,
            CRCType initialChecksum = 0) const;

        virtual CRCType computePatch(
            CRCType targetChecksum,
            File::OffsetType targetPosition,
            File &inputFile,
            bool overwrite = false) const;

        /**
         * The following methods calculate new checksums based on
         * previous checksum value and current input byte.
         */
        virtual CRCType makeNextChecksum(CRCType checksum, uint8_t c) const = 0;
        virtual CRCType makePrevChecksum(CRCType checksum, uint8_t c) const = 0;

    protected:
        CRCType lookupTable[256];
        CRCType invLookupTable[256];

    private:
        ProgressFunction progressFunction;
};

#endif
