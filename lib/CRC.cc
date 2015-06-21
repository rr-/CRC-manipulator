#include <cassert>
#include "CRC.h"

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

    CRCType swapEndian(CRCType crc, size_t crcSize)
    {
        CRCType result = 0;
        for (size_t i = 0; i < crcSize; i++)
        {
            result <<= 8;
            result |= crc & 0xff;
            crc >>= 8;
        }
        return result;
    }
}

CRC::CRC(const CRCSpecs &specs) : specs(specs)
{
    auto poly = specs.polynomial;
    auto polyRev = getPolynomialReverse(poly, specs.numBytes);

    size_t numBits = specs.numBytes * 8;
    auto mask = 1 << (numBits - 1);
    for (uint16_t n = 0; n <= 0xff; n++)
    {
        CRCType t[2] = { n, n };
        t[!(specs.flags & CRCFlags::BigEndian)] = swapEndian(n, specs.numBytes);
        for (uint8_t k = 0; k < 8; k++)
        {
            if (specs.flags & CRCFlags::BigEndian)
            {
                t[0] = t[0] & mask
                    ? (t[0] << 1) ^ poly
                    : (t[0] << 1);

                t[1] = t[1] & 1
                    ? ((t[1] ^ poly) >> 1) | mask
                    : (t[1] >> 1);
            }
            else
            {
                t[0] = t[0] & 1
                    ? (t[0] >> 1) ^ polyRev
                    : (t[0] >> 1);

                t[1] = t[1] & mask
                    ? ((t[1] ^ polyRev) << 1) | 1
                    : (t[1] << 1);
            }
        }
        if (specs.flags & CRCFlags::BigEndian)
            t[1] ^= swapEndian(n, specs.numBytes);
        lookupTable[n] = t[0];
        invLookupTable[n] = t[1];
    }
}

CRC::~CRC()
{
}

const CRCSpecs &CRC::getSpecs() const
{
    return specs;
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
    assert(specs.numBytes < BufferSize);
    for (size_t i = 0; i < specs.numBytes; i++)
        buffer[i] = static_cast<uint8_t>(patch >> (i << 3));
    output.write(buffer.get(), specs.numBytes);
    if (overwrite)
    {
        pos += specs.numBytes;
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
    CRCType checksum = specs.initialXOR;
    checksum = computePartialChecksum(
        input, 0, input.getSize(), checksum, progress);

    if (specs.flags & CRCFlags::UseFileSize)
    {
        auto fileSize = input.getSize();
        while (fileSize)
        {
            checksum = makeNextChecksum(checksum, fileSize);
            fileSize >>= 8;
        }
    }
    return checksum ^ specs.finalXOR;
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
    targetChecksum ^= specs.finalXOR;

    if (specs.flags & CRCFlags::UseFileSize)
    {
        auto fileSize = inputFile.getSize() + (overwrite ? 0 : specs.numBytes);
        auto fileSizeCopy = fileSize;
        auto fileSizeByteCount = 0;
        while (fileSizeCopy)
        {
            fileSizeByteCount ++;
            fileSizeCopy >>= 8;
        }
        fileSize = swapEndian(fileSize, fileSizeByteCount);
        while (fileSize)
        {
            targetChecksum = makePrevChecksum(targetChecksum, fileSize);
            fileSize >>= 8;
        }
    }

    CRCType checksum1 = computePartialChecksum(
        inputFile,
        0,
        targetPos,
        specs.initialXOR,
        progress);

    CRCType checksum2 = computeReversePartialChecksum(
        inputFile,
        inputFile.getSize(),
        targetPos + (overwrite ? specs.numBytes : 0),
        targetChecksum,
        progress);

    CRCType patch = checksum2;

    if (specs.flags & CRCFlags::BigEndian)
        checksum1 = swapEndian(checksum1, specs.numBytes);
    for (size_t i = 0, j = specs.numBytes - 1; i < specs.numBytes; i++, j--)
        patch = makePrevChecksum(patch, checksum1 >> (j << 3));
    if (specs.flags & CRCFlags::BigEndian)
        patch = swapEndian(patch, specs.numBytes);

    return patch;
}

CRCType CRC::makeNextChecksum(CRCType prevChecksum, uint8_t c) const
{
    if (specs.flags & CRCFlags::BigEndian)
    {
        uint8_t index = (prevChecksum >> (specs.numBytes * 8 - 8)) ^ c;
        return (prevChecksum << 8) ^ lookupTable[index];
    }
    uint8_t index = prevChecksum ^ c;
    return (prevChecksum >> 8) ^ lookupTable[index];
}

CRCType CRC::makePrevChecksum(CRCType nextChecksum, uint8_t c) const
{
    if (specs.flags & CRCFlags::BigEndian)
    {
        uint8_t index = nextChecksum;
        return (c << (specs.numBytes * 8 - 8))
            ^ invLookupTable[index]
            ^ (nextChecksum << (specs.numBytes * 8 - 8)) ^ (nextChecksum >> 8);
    }
    uint8_t index = nextChecksum >> (specs.numBytes * 8 - 8);
    return (nextChecksum << 8) ^ invLookupTable[index] ^ c;
}
