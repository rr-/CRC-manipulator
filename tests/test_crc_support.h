#ifndef TEST_CRC_SUPPORT_H
#define TEST_CRC_SUPPORT_H
#include "CRC.h"

void test_computing(const CRC &crc, CRCType checksum);
void test_appending(const CRC &crc, CRCType checksum);
void test_inserting(const CRC &crc, CRCType checksum);
void test_overwriting(const CRC &crc, CRCType checksum);

#endif
