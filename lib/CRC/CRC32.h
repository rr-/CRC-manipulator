#ifndef CRC32_H
#define CRC32_H

#include "CRC/CRC.h"

class CRC32 : public CRC
{
    public:
        CRC32();

    protected:
        virtual CRCType getPolynomial() const;
        virtual CRCType getPolynomialReverse() const;
        virtual size_t getNumBytes() const;
        virtual CRCType makeNextChecksum(CRCType prevChecksum, uint8_t c) const;
        virtual CRCType makePrevChecksum(CRCType nextChecksum, uint8_t c) const;

    private:
        uint32_t lookupTable[256];
        uint32_t invLookupTable[256];
};

#endif
