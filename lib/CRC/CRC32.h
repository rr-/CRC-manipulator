#ifndef CRC32_H
#define CRC32_H

#include "CRC/CRC.h"

class CRC32 : public CRC
{
    public:
        CRC32();

    protected:
        virtual CRCType makeNextChecksum(CRCType prevChecksum, uint8_t c) const;
        virtual CRCType makePrevChecksum(CRCType nextChecksum, uint8_t c) const;
};

#endif
