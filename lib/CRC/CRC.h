#ifndef CRC_H
#define CRC_H

#include <functional>
#include "../File/File.h"

typedef uint32_t CRCType; //just make it enough to hold any derived CRC.
//Templates would be real PITA since no abstract pointers would be possible.

class CRC
{
    //Types, constants
    public:
        enum CRCProgressType
        {
            CRCPROG_WRITE_START,
            CRCPROG_WRITE_PROGRESS,
            CRCPROG_WRITE_END,
            CRCPROG_CHECKSUM_START,
            CRCPROG_CHECKSUM_PROGRESS,
            CRCPROG_CHECKSUM_END
        };
        const static int ERR_NOT_IMPLEMENTED = 0;
        const static int ERR_PATCH_FAILED = 1;

        typedef std::function<void(
            const CRCProgressType &progressType,
            const File::OffsetType &startPosition,
            const File::OffsetType &currentPosition,
            const File::OffsetType &endPosition)>
        ProgressFunction;


        virtual size_t getNumBytes() const = 0;
        virtual CRCType getPolynomial() const = 0;
        virtual CRCType getPolynomialReverse() const = 0;

        CRC();
        virtual ~CRC();
        const CRCType &getInitialXOR() const;
        const CRCType &getFinalXOR() const;
        void setInitialXOR(const CRCType &t);
        void setFinalXOR(const CRCType &t);

        void setProgressFunction(const ProgressFunction &progressFunction);

        CRCType computeChecksum(File &inputFile) const;

        void applyPatch(
            const CRCType &desiredCRC,
            const File::OffsetType &desiredPosition,
            File &inputFile,
            File &outputFile,
            const bool &overwrite = false) const;

    protected:
        void markProgress(
            const CRCProgressType &progressType,
            const File::OffsetType &startPosition,
            const File::OffsetType &currentPosition,
            const File::OffsetType &endPosition) const;

        CRCType computePartialChecksum(
            File &inputFile,
            const File::OffsetType &startPosition,
            const File::OffsetType &endPosition,
            const CRCType &initialChecksum = 0) const;

        CRCType computeReversePartialChecksum(
            File &inputFile,
            const File::OffsetType &startPosition,
            const File::OffsetType &endPosition,
            const CRCType &initialChecksum = 0) const;

        virtual CRCType computePatch(
            const CRCType &desiredCRC,
            const File::OffsetType &desiredPosition,
            File &inputFile,
            const bool &overwrite = false) const = 0;

        /**
         * The following methods calculate new checksums based on
         * previous checksum value and current input byte.
         */
        virtual CRCType makeNextChecksum(
            const CRCType &checksum,
            unsigned char c) const = 0;

        virtual CRCType makePrevChecksum(
            const CRCType &checksum,
            unsigned char c) const = 0;

    private:
        CRCType finalXOR;
        CRCType initialXOR;
        ProgressFunction progressFunction;
};

#endif
