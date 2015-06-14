#include "CRC16IBM.h"
#include "../debug.h"

CRCType CRC16IBM::getPolynomial() const
{
    return (1 << 0 | 1 << 2 | 1 << 15);
}

CRCType CRC16IBM::getPolynomialReverse() const
{
    return (1 << (15-0) | 1 << (15-2) | 1 << (15-15));
}

size_t CRC16IBM::getNumBytes() const
{
    return 2;
}

size_t CRC16IBM::getNumBits() const
{
    return 16;
}

CRCType CRC16IBM::makeNextChecksum(
    const CRCType &prevChecksum,
    unsigned char c) const
{
    uint8_t tmp = (prevChecksum ^ c) & 0xff;
    uint16_t ret = (prevChecksum >> 8) ^ this->lookupTable[tmp];
    ret &= 0xffff;
    return ret;
}

CRCType CRC16IBM::makePrevChecksum(
    const CRCType &nextChecksum,
    unsigned char c) const
{
    uint8_t tmp = (nextChecksum >> 8) & 0xff;
    uint16_t ret = (nextChecksum << 8) ^ this->invLookupTable[tmp] ^ c;
    ret &= 0xffff;
    return ret;
}

CRC16IBM::CRC16IBM() : CRC()
{
    this->setInitialXOR(0);
    this->setFinalXOR(0);
    for (uint16_t n = 0; n <= 0xff; n ++)
    {
        uint16_t crc1 = 0;
        uint16_t crc2 = 0;
        for (uint8_t k = 0; k < 8; k ++)
        {
            if ((crc1 ^ (n >> k)) & 1)
                crc1 = (crc1 >> 1) ^ this->getPolynomialReverse();
            else
                crc1 >>= 1;

            if ((crc2 ^ (n << (8 + k))) & 0x8000)
                crc2 = (crc2 ^ this->getPolynomialReverse()) << 1 | 1;
            else
                crc2 <<= 1;
        }
        this->lookupTable[n] = crc1;
        this->invLookupTable[n] = crc2;
    }
}

CRCType CRC16IBM::computePatch(
    const CRCType &desiredChecksum,
    const File::OffsetType &desiredPosition,
    File &inputFile,
    const bool &overwrite) const
{
    uint16_t checksum1 = this->computePartialChecksum(
        inputFile,
        0,
        desiredPosition,
        this->getInitialXOR());

    uint16_t checksum2 = this->computeReversePartialChecksum(
        inputFile,
        inputFile.getFileSize(),
        desiredPosition + ((File::OffsetType) (overwrite ? 2 : 0)),
        (uint16_t) (desiredChecksum ^ this->getFinalXOR()));

    uint16_t patch = checksum2;
    for (size_t i = 0, j = 1; i < 2; i ++, j --)
        patch = this->makePrevChecksum(patch,(checksum1 >> (j << 3)) & 0xff);

    return patch;
}
