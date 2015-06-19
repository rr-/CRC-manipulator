#include "CRC/CRC16IBM.h"

CRCType CRC16IBM::makeNextChecksum(CRCType prevChecksum, uint8_t c) const
{
    uint8_t tmp = prevChecksum ^ c;
    return static_cast<uint16_t>((prevChecksum >> 8) ^ lookupTable[tmp]);
}

CRCType CRC16IBM::makePrevChecksum(CRCType nextChecksum, uint8_t c) const
{
    uint8_t tmp = nextChecksum >> 8;
    return static_cast<uint16_t>((nextChecksum << 8) ^ invLookupTable[tmp] ^ c);
}

CRC16IBM::CRC16IBM() : CRC(2, 0x8005, 0, 0)
{
}
