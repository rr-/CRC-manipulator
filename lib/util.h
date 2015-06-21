#ifndef UTIL_H
#define UTIL_H
#include "file.h"

File::OffsetType computeAutoPosition(
    File::OffsetType fileSize, size_t crcSize, bool overwrite);

File::OffsetType shiftUserPosition(
    File::OffsetType userPosition,
    File::OffsetType fileSize,
    size_t crcSize,
    bool overwrite);

#endif
