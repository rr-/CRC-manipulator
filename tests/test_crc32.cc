#include "CRC/CRC32.h"
#include "catch.hh"
#include "test_crc.h"

TEST_CASE("CRC32 patch appending works", "[crc32]")
{
    test_appending(std::unique_ptr<CRC>(new CRC32), 0xdeadbeef);
}
