#ifndef CLI_UTIL_H
#define CLI_UTIL_H
#include "File/File.h"

File::OffsetType computeAutoPosition(
    File::OffsetType fileSize,
    size_t crcSize,
    bool overwrite);

File::OffsetType shiftUserPosition(
    File::OffsetType userPosition,
    File::OffsetType fileSize,
    size_t crcSize,
    bool overwrite);

#endif
