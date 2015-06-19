#include "CRC/CRC16CCITT.h"
#include "CRC/CRC16IBM.h"
#include "CRC/CRC32.h"
#include "catch.hh"
#include "test_crc_support.h"

TEST_CASE("CRC computing works", "[crc]")
{
    SECTION("CRC32")      { test_computing(CRC32(),      0xcbf43926); }
    SECTION("CRC16CCITT") { test_computing(CRC16CCITT(), 0x2189); }
    SECTION("CRC16IBM")   { test_computing(CRC16IBM(),   0xbb3d); }
}

TEST_CASE("CRC patch appending works", "[crc]")
{
    SECTION("CRC32")      { test_appending(CRC32(),      0xdeadbeef); }
    SECTION("CRC16CCITT") { test_appending(CRC16CCITT(), 0xdead); }
    SECTION("CRC16IBM")   { test_appending(CRC16IBM(),   0xdead); }
}

TEST_CASE("CRC patch inserting works", "[crc]")
{
    SECTION("CRC32")      { test_inserting(CRC32(),      0xdeadbeef); }
    SECTION("CRC16CCITT") { test_inserting(CRC16CCITT(), 0xdead); }
    SECTION("CRC16IBM")   { test_inserting(CRC16IBM(),   0xdead); }
}

TEST_CASE("CRC patch overwriting works", "[crc]")
{
    SECTION("CRC32")      { test_overwriting(CRC32(),      0xdeadbeef); }
    SECTION("CRC16CCITT") { test_overwriting(CRC16CCITT(), 0xdead); }
    SECTION("CRC16IBM")   { test_overwriting(CRC16IBM(),   0xdead); }
}
