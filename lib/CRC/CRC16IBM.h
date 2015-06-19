#ifndef CRC16IBM_H
#define CRC16IBM_H

#include "CRC/CRC.h"

class CRC16IBM : public CRC
{
    public:
        CRC16IBM();

    protected:
        virtual CRCType makeNextChecksum(CRCType prevChecksum, uint8_t c) const;
        virtual CRCType makePrevChecksum(CRCType nextChecksum, uint8_t c) const;
};

#endif
