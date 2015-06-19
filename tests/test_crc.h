#ifndef TEST_CRC_H
#define TEST_CRC_H
#include <memory>
#include "CRC/CRC.h"

void test_appending(std::unique_ptr<CRC> crc, CRCType checksum);

#endif
