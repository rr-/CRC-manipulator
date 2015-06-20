#include <cassert>
#include <iostream>
#include "CRC/CRC.h"

/**
 * NOTICE: following code is strongly based on SAR-PR-2006-05
 */

namespace
{
    const size_t BufferSize = 8192;

    size_t getChunkSize(File::OffsetType currentPos, File::OffsetType maxPos)
    {
        if (currentPos + static_cast<File::OffsetType>(BufferSize) >= maxPos)
            return maxPos - currentPos;
        return BufferSize;
    }

    CRCType getPolynomialReverse(CRCType polynomial, size_t numBytes)
    {
        CRCType rev = 0;
        size_t numBits = numBytes * 8;
        for (size_t i = 0; i < numBits; i++)
            rev |= !!(polynomial & (1 << i)) << (numBits - 1 - i);
        return rev;
    }
}

CRC::CRC()
{
}

CRC::CRC(
    size_t numBytes, CRCType polynomial, CRCType initialXOR, CRCType finalXOR) :
    numBytes(numBytes),
    polynomial(polynomial),
    polynomialReverse(getPolynomialReverse(polynomial, numBytes)),
    initialXOR(initialXOR),
    finalXOR(finalXOR)
{
    size_t numBits = numBytes * 8;
    auto var1 = numBits - 8;
    auto var2 = 1 << (numBits - 1);
    for (uint16_t n = 0; n <= 0xff; n++)
    {
        CRCType crc1 = n;
        CRCType crc2 = n << var1;
        for (uint8_t k = 0; k < 8; k++)
        {
            if (crc1 & 1)
                crc1 = (crc1 >> 1) ^ polynomialReverse;
            else
                crc1 >>= 1;

            if (crc2 & var2)
                crc2 = ((crc2 ^ polynomialReverse) << 1) | 1;
            else
                crc2 <<= 1;
        }
        lookupTable[n] = crc1;
        invLookupTable[n] = crc2;
    }
}

CRC::~CRC()
{
}

size_t CRC::getNumBytes() const
{
    return numBytes;
}

/**
 * Method that copies the input to the output, outputting
 * computed patch at given position along the way.
 */
void CRC::applyPatch(
    CRCType finalChecksum,
    File::OffsetType targetPos,
    File &input,
    File &output,
    bool overwrite,
    Progress &writeProgress,
    Progress &checksumProgress) const
{
    std::unique_ptr<uint8_t[]> buffer(new uint8_t[BufferSize]);
    CRCType patch = computePatch(
        finalChecksum, targetPos, input, overwrite, checksumProgress);

    input.seek(0, File::Origin::Start);
    File::OffsetType pos = input.tell();
    writeProgress.start(input.getSize());

    //output first half
    while (pos < targetPos)
    {
        writeProgress.set(pos);
        auto chunkSize = getChunkSize(pos, targetPos);
        input.read(buffer.get(), chunkSize);
        output.write(buffer.get(), chunkSize);
        pos += chunkSize;
    }

    //output patch
    assert(numBytes < BufferSize);
    for (size_t i = 0; i < numBytes; i++)
        buffer[i] = static_cast<uint8_t>(patch >> (i << 3));
    output.write(buffer.get(), numBytes);
    if (overwrite)
    {
        pos += numBytes;
        input.seek(pos, File::Origin::Start);
    }

    //output second half
    while (pos < input.getSize())
    {
        writeProgress.set(pos);
        auto chunkSize = getChunkSize(pos, input.getSize());
        input.read(buffer.get(), chunkSize);
        output.write(buffer.get(), chunkSize);
        pos += chunkSize;
    }

    writeProgress.finish();
}

/**
 * Computes the checksum of given file.
 * NOTICE: Leaves internal file pointer position intact.
 */
CRCType CRC::computeChecksum(File &input, Progress &progress) const
{
    CRCType checksum = initialXOR;
    checksum = computePartialChecksum(
        input, 0, input.getSize(), checksum, progress);
    return checksum ^ finalXOR;
}

/**
 * Computes partial checksum of given file.
 * NOTICE: Leaves internal file pointer position intact.
 */
CRCType CRC::computePartialChecksum(
    File &input,
    File::OffsetType startPos,
    File::OffsetType endPos,
    CRCType initialChecksum,
    Progress &progress) const
{
    assert(startPos <= endPos);
    if (startPos == endPos)
        return initialChecksum;

    CRCType checksum = initialChecksum;
    std::unique_ptr<uint8_t[]> buffer(new uint8_t[BufferSize]);
    File::OffsetType oldPos = input.tell();
    File::OffsetType pos = startPos;
    input.seek(pos, File::Origin::Start);
    progress.start(endPos - startPos);

    while (pos < endPos)
    {
        progress.set(pos - startPos);
        auto chunkSize = getChunkSize(pos, endPos);
        input.read(buffer.get(), chunkSize);
        for (size_t i = 0; i < chunkSize; i++)
            checksum = makeNextChecksum(checksum, buffer[i]);
        pos += chunkSize;
    }

    progress.finish();
    input.seek(oldPos, File::Origin::Start);
    return checksum;
}

/**
 * Computes reverse partial checksum of given file.
 * NOTICE: Leaves internal file pointer position intact.
 */
CRCType CRC::computeReversePartialChecksum(
    File &input,
    File::OffsetType startPos,
    File::OffsetType endPos,
    CRCType initialChecksum,
    Progress &progress) const
{
    assert(startPos >= endPos);
    if (startPos == endPos)
        return initialChecksum;

    CRCType checksum = initialChecksum;
    std::unique_ptr<uint8_t[]> buffer(new uint8_t[BufferSize]);
    File::OffsetType oldPos = input.tell();
    File::OffsetType pos = startPos;
    progress.start(startPos);

    while (pos > endPos)
    {
        progress.set(startPos - pos);
        auto chunkSize = getChunkSize(endPos, pos);
        pos -= chunkSize;
        input.seek(pos, File::Origin::Start);
        input.read(buffer.get(), chunkSize);
        for (size_t i = 0, j = chunkSize - 1; i < chunkSize; i++, j--)
            checksum = makePrevChecksum(checksum, buffer[j]);
    }

    progress.finish();
    input.seek(oldPos, File::Origin::Start);
    return checksum;
}

CRCType CRC::computePatch(
    CRCType targetChecksum,
    File::OffsetType targetPos,
    File &inputFile,
    bool overwrite,
    Progress &progress) const
{
    CRCType checksum1 = computePartialChecksum(
        inputFile,
        0,
        targetPos,
        initialXOR,
        progress);

    CRCType checksum2 = computeReversePartialChecksum(
        inputFile,
        inputFile.getSize(),
        targetPos + (overwrite ? numBytes : 0),
        static_cast<CRCType>(targetChecksum ^ finalXOR),
        progress);

    CRCType patch = checksum2;
    for (size_t i = 0, j = numBytes - 1; i < numBytes; i++, j--)
        patch = makePrevChecksum(patch, (checksum1 >> (j << 3)) & 0xff);

    return patch;
}

CRCType CRC::makeNextChecksum(CRCType prevChecksum, uint8_t c) const
{
    uint8_t index = prevChecksum ^ c;
    return (prevChecksum >> 8) ^ lookupTable[index];
}

CRCType CRC::makePrevChecksum(CRCType nextChecksum, uint8_t c) const
{
    uint8_t index = nextChecksum >> (numBytes * 8 - 8);
    return (nextChecksum << 8) ^ invLookupTable[index] ^ c;
}
