#include "CRC/CRC16CCITT.h"

CRCType CRC16CCITT::getPolynomial() const
{
    return (1 << 0 | 1 << 5 | 1 << 12);
}

size_t CRC16CCITT::getNumBytes() const
{
    return 2;
}

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

CRC16CCITT::CRC16CCITT() : CRC()
{
    setInitialXOR(0);
    setFinalXOR(0);
    for (uint16_t n = 0; n <= 0xff; n++)
    {
        uint16_t crc1 = 0;
        uint16_t crc2 = 0;
        for (uint8_t k = 0; k < 8; k++)
        {
            if ((crc1 ^ (n >> k)) & 1)
                crc1 = (crc1 >> 1) ^ getPolynomialReverse();
            else
                crc1 >>= 1;

            if ((crc2 ^ (n << (8 + k))) & 0x8000)
                crc2 = (crc2 ^ getPolynomialReverse()) << 1 | 1;
            else
                crc2 <<= 1;
        }
        lookupTable[n] = crc1;
        invLookupTable[n] = crc2;
    }
}
