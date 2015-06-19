#include "CRC/CRC32.h"

/**
 * NOTICE: following code is strongly based on SAR-PR-2006-05
 */

CRCType CRC32::makeNextChecksum(CRCType prevChecksum, uint8_t c) const
{
    uint8_t tmp = prevChecksum ^ c;
    return static_cast<uint32_t>((prevChecksum >> 8) ^ lookupTable[tmp]);
}

CRCType CRC32::makePrevChecksum(CRCType nextChecksum, uint8_t c) const
{
    uint8_t tmp = nextChecksum >> 24;
    return static_cast<uint32_t>((nextChecksum << 8) ^ invLookupTable[tmp] ^ c);
}

CRC32::CRC32() : CRC(4, 0x04C11DB7, 0xffffffff, 0xffffffff)
{
}
