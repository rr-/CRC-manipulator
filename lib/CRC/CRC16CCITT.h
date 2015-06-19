#ifndef CRC16CCITT_H
#define CRC16CCITT_H

#include "CRC/CRC.h"

class CRC16CCITT : public CRC
{
    public:
        CRC16CCITT();

    protected:
        virtual CRCType makeNextChecksum(CRCType prevChecksum, uint8_t c) const;
        virtual CRCType makePrevChecksum(CRCType nextChecksum, uint8_t c) const;
};

#endif
