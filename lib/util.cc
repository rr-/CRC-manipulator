#include "util.h"

File::OffsetType computeAutoPosition(
    File::OffsetType fileSize,
    size_t crcSize,
    bool overwrite)
{
    auto totalSize = fileSize;
    if (!overwrite)
        totalSize += crcSize;

    File::OffsetType targetPosition = crcSize
        ? totalSize - crcSize
        : totalSize;

    if (targetPosition < 0)
    {
        targetPosition += overwrite
            ? fileSize - crcSize
            : fileSize;
    }

    if (targetPosition < 0 ||
        targetPosition + static_cast<File::OffsetType>(crcSize) > totalSize)
    {
        throw std::invalid_argument(
            "Patch position is located outside available input");
    }

    return targetPosition;
}

File::OffsetType shiftUserPosition(
    File::OffsetType userPosition,
    File::OffsetType fileSize,
    size_t crcSize,
    bool overwrite)
{
    auto targetPosition = userPosition;
    auto totalSize = fileSize;
    if (!overwrite)
        totalSize += crcSize;

    while (targetPosition < 0)
        targetPosition += fileSize;

    if (targetPosition < 0 ||
        targetPosition + static_cast<File::OffsetType>(crcSize) > totalSize)
    {
        throw std::invalid_argument(
            "Patch position is located outside available input");
    }

    return targetPosition;
}
