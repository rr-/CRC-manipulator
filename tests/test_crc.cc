#include "catch.hh"
#include "lib/CRCFactories.h"
#include "test_crc_support.h"

namespace
{
    CRCType getTestChecksum(size_t numBytes)
    {
        return 0xDECEA5ED & (0xFFFFFFFFUL >> (32 - (numBytes << 3)));
    }
}

TEST_CASE("CRC computing works", "[crc]")
{
    for (auto &crc : createAllCRC())
        SECTION(crc->getSpecs().name)
            test_computing(*crc, crc->getSpecs().test);
}

TEST_CASE("CRC patch appending works", "[crc]")
{
    for (auto &crc : createAllCRC())
        SECTION(crc->getSpecs().name)
            test_appending(*crc, getTestChecksum(crc->getSpecs().numBytes));
}

TEST_CASE("CRC patch inserting works", "[crc]")
{
    for (auto &crc : createAllCRC())
        SECTION(crc->getSpecs().name)
            test_inserting(*crc, getTestChecksum(crc->getSpecs().numBytes));
}

TEST_CASE("CRC patch overwriting works", "[crc]")
{
    for (auto &crc : createAllCRC())
        SECTION(crc->getSpecs().name)
            test_overwriting(*crc, getTestChecksum(crc->getSpecs().numBytes));
}
