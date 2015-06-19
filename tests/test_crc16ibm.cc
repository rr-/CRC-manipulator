#include "CRC/CRC16IBM.h"
#include "catch.hh"
#include "test_crc.h"

TEST_CASE("CRC16IBM patch appending works", "[crc16]")
{
    test_appending(std::unique_ptr<CRC>(new CRC16IBM), 0xdead);
}

TEST_CASE("CRC16IBM patch inserting works", "[crc16]")
{
    test_inserting(std::unique_ptr<CRC>(new CRC16IBM), 0xdead);
}

TEST_CASE("CRC16IBM patch overwriting works", "[crc16]")
{
    test_overwriting(std::unique_ptr<CRC>(new CRC16IBM), 0xdead);
}

TEST_CASE("CRC16IBM computing", "[crc16]")
{
    test_computing(std::unique_ptr<CRC>(new CRC16IBM), 0xbb3d);
}
