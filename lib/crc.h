#ifndef CRC_H
#define CRC_H
#include <functional>
#include "file.h"
#include "progress.h"

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

    public:
        CRC(const Specs &specs);
        ~CRC();

        const Specs &getSpecs() const;

        Value computeChecksum(File &inputFile, Progress &progress) const;

        void applyPatch(
            Value targetChecksum,
            File::OffsetType targetPosition,
            File &inputFile,
            File &outputFile,
            bool overwrite,
            Progress &writeProgress,
            Progress &checksumProgress) const;

    private:
        struct Internals;
        std::unique_ptr<Internals> internals;
};

#endif
