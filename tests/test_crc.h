#ifndef TEST_CRC_H
#define TEST_CRC_H
#include <memory>
#include "CRC/CRC.h"

void test_appending(std::unique_ptr<CRC> crc, CRCType checksum);
void test_inserting(std::unique_ptr<CRC> crc, CRCType checksum);
void test_overwriting(std::unique_ptr<CRC> crc, CRCType checksum);
void test_computing(std::unique_ptr<CRC> crc, CRCType checksum);

#endif
