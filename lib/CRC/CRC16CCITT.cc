#include "CRC/CRC16CCITT.h"

CRCType CRC16CCITT::getPolynomial() const
{
    return (1 << 0 | 1 << 5 | 1 << 12);
}

CRCType CRC16CCITT::getPolynomialReverse() const
{
    return (1 << (15-0) | 1 << (15-5) | 1 << (15-12));
}

size_t CRC16CCITT::getNumBytes() const
{
    return 2;
}

CRCType CRC16CCITT::makeNextChecksum(CRCType prevChecksum, uint8_t c) const
{
    uint8_t tmp = (prevChecksum >> 8) ^ c;
    return static_cast<uint16_t>((prevChecksum << 8) ^ lookupTable[tmp]);
}

CRCType CRC16CCITT::makePrevChecksum(CRCType nextChecksum, uint8_t c) const
{
    return static_cast<uint16_t>(
        (c << 8)
        ^ invLookupTable[static_cast<uint8_t>(nextChecksum)]
        ^ (nextChecksum << 8)
        ^ (nextChecksum >> 8));
}

CRC16CCITT::CRC16CCITT() : CRC()
{
    setInitialXOR(0);
    setFinalXOR(0);
    for (uint16_t n = 0; n <= 0xff; n++)
    {
        uint16_t crc1 = 0;
        uint32_t crc2 = 0;
        for (uint8_t k = 0; k < 8; k++)
        {
            if ((crc1 ^ (n << (8 + k))) & 0x8000)
                crc1 = (crc1 << 1) ^ getPolynomial();
            else
                crc1 <<= 1;

            if ((crc2 ^ (n >> k)) & 1)
                crc2 = ((crc2 ^ getPolynomial()) >> 1) | 0x100000;
            else
                crc2 >>= 1;
        }
        crc2 &= 0xffff;
        lookupTable[n] = crc1;
        invLookupTable[n] = static_cast<uint16_t>(crc2);
    }
}

CRCType CRC16CCITT::computePatch(
    CRCType targetChecksum,
    File::OffsetType targetPos,
    File &inputFile,
    bool overwrite) const
{
    CRCType checksum1 = computePartialChecksum(
        inputFile,
        0,
        targetPos,
        getInitialXOR());

    CRCType checksum2 = computeReversePartialChecksum(
        inputFile,
        inputFile.getSize(),
        targetPos + (overwrite ? getNumBytes() : 0),
        static_cast<CRCType>(targetChecksum ^ getFinalXOR()));

    CRCType patch = checksum2;
    for (size_t i = 0, j = getNumBytes() - 1; i < getNumBytes(); i++, j--)
        patch = makePrevChecksum(patch, (checksum1 >> (i << 3)) & 0xff);

    return (patch << 8 | patch >> 8) & 0xffff;
}
