#include "CRCFactories.h"
#include "catch.hh"
#include "test_crc_support.h"

TEST_CASE("CRC computing works", "[crc]")
{
    SECTION("CRC32")      { test_computing(*createCRC32(),      0xcbf43926); }
    SECTION("CRC16CCITT") { test_computing(*createCRC16CCITT(), 0x2189); }
    SECTION("CRC16IBM")   { test_computing(*createCRC16IBM(),   0xbb3d); }
}

TEST_CASE("CRC patch appending works", "[crc]")
{
    SECTION("CRC32")      { test_appending(*createCRC32(),      0xdeadbeef); }
    SECTION("CRC16CCITT") { test_appending(*createCRC16CCITT(), 0xdead); }
    SECTION("CRC16IBM")   { test_appending(*createCRC16IBM(),   0xdead); }
}

TEST_CASE("CRC patch inserting works", "[crc]")
{
    SECTION("CRC32")      { test_inserting(*createCRC32(),      0xdeadbeef); }
    SECTION("CRC16CCITT") { test_inserting(*createCRC16CCITT(), 0xdead); }
    SECTION("CRC16IBM")   { test_inserting(*createCRC16IBM(),   0xdead); }
}

TEST_CASE("CRC patch overwriting works", "[crc]")
{
    SECTION("CRC32")      { test_overwriting(*createCRC32(),      0xdeadbeef); }
    SECTION("CRC16CCITT") { test_overwriting(*createCRC16CCITT(), 0xdead); }
    SECTION("CRC16IBM")   { test_overwriting(*createCRC16IBM(),   0xdead); }
}
