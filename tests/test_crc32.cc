#include "CRC/CRC32.h"
#include "catch.hh"
#include "test_crc.h"

TEST_CASE("CRC32 patch appending works", "[crc32]")
{
    test_appending(std::unique_ptr<CRC>(new CRC32), 0xdeadbeef);
}

TEST_CASE("CRC32 patch inserting works", "[crc32]")
{
    test_inserting(std::unique_ptr<CRC>(new CRC32), 0xdeadbeef);
}

TEST_CASE("CRC32 patch overwriting works", "[crc32]")
{
    test_overwriting(std::unique_ptr<CRC>(new CRC32), 0xdeadbeef);
}

TEST_CASE("CRC32 computing", "[crc32]")
{
    test_computing(std::unique_ptr<CRC>(new CRC32), 0xcbf43926);
}
