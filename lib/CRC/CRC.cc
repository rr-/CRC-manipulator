#include <cassert>
#include "CRC/CRC.h"

namespace
{
    const size_t BufferSize = 8192;

    size_t getChunkSize(File::OffsetType currentPos, File::OffsetType maxPos)
    {
        if (currentPos + static_cast<File::OffsetType>(BufferSize) >= maxPos)
            return maxPos - currentPos;
        return BufferSize;
    }
}

CRC::CRC()
{
    progressFunction = nullptr;
}

CRC::~CRC()
{
}

CRCType CRC::getInitialXOR() const
{
    return initialXOR;
}

CRCType CRC::getFinalXOR() const
{
    return finalXOR;
}

void CRC::setInitialXOR(CRCType f)
{
    initialXOR = f;
}

void CRC::setFinalXOR(CRCType f)
{
    finalXOR = f;
}

void CRC::setProgressFunction(const CRC::ProgressFunction &f)
{
    progressFunction = f;
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
    bool overwrite) const
{
    std::unique_ptr<uint8_t[]> buffer(new uint8_t[BufferSize]);
    CRCType patch = computePatch(finalChecksum, targetPos, input, overwrite);

    input.seek(0, File::Origin::Start);
    File::OffsetType pos = input.tell();
    markProgress(ProgressType::WriteStart, 0, pos, input.getSize());

    //output first half
    while (pos < targetPos)
    {
        markProgress(ProgressType::WriteProgress, 0, pos, input.getSize());
        auto chunkSize = getChunkSize(pos, targetPos);
        input.read(buffer.get(), chunkSize);
        output.write(buffer.get(), chunkSize);
        pos += chunkSize;
    }

    //output patch
    assert(getNumBytes() < BufferSize);
    for (size_t i = 0; i < getNumBytes(); i++)
        buffer[i] = static_cast<uint8_t>(patch >> (i << 3));
    output.write(buffer.get(), getNumBytes());
    if (overwrite)
    {
        pos += getNumBytes();
        input.seek(pos, File::Origin::Start);
    }

    //output second half
    while (pos < input.getSize())
    {
        markProgress(ProgressType::WriteProgress, 0, pos, input.getSize());
        auto chunkSize = getChunkSize(pos, input.getSize());
        input.read(buffer.get(), chunkSize);
        output.write(buffer.get(), chunkSize);
        pos += chunkSize;
    }

    markProgress(ProgressType::WriteEnd, 0, pos, input.getSize());
}

/**
 * Computes the checksum of given file.
 * NOTICE: Leaves internal file pointer position intact.
 */
CRCType CRC::computeChecksum(File &input) const
{
    CRCType checksum = getInitialXOR();
    checksum = computePartialChecksum(input, 0, input.getSize(), checksum);
    return checksum ^ getFinalXOR();
}

/**
 * Computes partial checksum of given file.
 * NOTICE: Leaves internal file pointer position intact.
 */
CRCType CRC::computePartialChecksum(
    File &input,
    File::OffsetType startPos,
    File::OffsetType endPos,
    CRCType initialChecksum) const
{
    assert(startPos <= endPos);
    if (startPos == endPos)
        return initialChecksum;

    CRCType checksum = initialChecksum;
    std::unique_ptr<uint8_t[]> buffer(new uint8_t[BufferSize]);
    File::OffsetType oldPos = input.tell();
    File::OffsetType pos = startPos;
    input.seek(pos, File::Origin::Start);
    markProgress(ProgressType::ChecksumStart, startPos, startPos, endPos);

    while (pos < endPos)
    {
        markProgress(ProgressType::ChecksumProgress, startPos, pos, endPos);
        auto chunkSize = getChunkSize(pos, endPos);
        input.read(buffer.get(), chunkSize);
        for (size_t i = 0; i < chunkSize; i++)
            checksum = makeNextChecksum(checksum, buffer[i]);
        pos += chunkSize;
    }

    markProgress(ProgressType::ChecksumEnd, startPos, endPos, endPos);
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
    CRCType initialChecksum) const
{
    assert(startPos >= endPos);
    if (startPos == endPos)
        return initialChecksum;

    CRCType checksum = initialChecksum;
    std::unique_ptr<uint8_t[]> buffer(new uint8_t[BufferSize]);
    File::OffsetType oldPos = input.tell();
    File::OffsetType pos = startPos;
    markProgress(ProgressType::ChecksumStart, startPos, startPos, endPos);

    while (pos > endPos)
    {
        markProgress(ProgressType::ChecksumProgress, startPos, pos, endPos);
        auto chunkSize = getChunkSize(endPos, pos);
        pos -= chunkSize;
        input.seek(pos, File::Origin::Start);
        input.read(buffer.get(), chunkSize);
        for (size_t i = 0, j = chunkSize - 1; i < chunkSize; i++, j--)
            checksum = makePrevChecksum(checksum, buffer[j]);
    }

    markProgress(ProgressType::ChecksumEnd, startPos, endPos, endPos);
    input.seek(oldPos, File::Origin::Start);
    return checksum;
}

void CRC::markProgress(
    ProgressType progressType,
    File::OffsetType startPos,
    File::OffsetType curPos,
    File::OffsetType endPos) const
{
    if (progressFunction != nullptr)
        progressFunction(progressType, startPos, curPos, endPos);
}

CRCType CRC::computePatch(
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
        patch = makePrevChecksum(patch, (checksum1 >> (j << 3)) & 0xff);

    return patch;
}
