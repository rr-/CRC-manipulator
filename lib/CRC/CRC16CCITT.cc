#include "CRC/CRC16CCITT.h"

CRCType CRC16CCITT::makeNextChecksum(CRCType prevChecksum, uint8_t c) const
{
    uint8_t tmp = prevChecksum ^ c;
    return static_cast<uint16_t>((prevChecksum >> 8) ^ lookupTable[tmp]);
}

CRCType CRC16CCITT::makePrevChecksum(CRCType nextChecksum, uint8_t c) const
{
    uint8_t tmp = nextChecksum >> 8;
    return static_cast<uint16_t>((nextChecksum << 8) ^ invLookupTable[tmp] ^ c);
}

CRC16CCITT::CRC16CCITT() : CRC(2, 0x1021, 0, 0)
{
}
