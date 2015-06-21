#ifndef TEST_CRC_SUPPORT_H
#define TEST_CRC_SUPPORT_H
#include "lib/CRC.h"

void test_computing(const CRC &crc, CRC::Value checksum);
void test_appending(const CRC &crc, CRC::Value checksum);
void test_inserting(const CRC &crc, CRC::Value checksum);
void test_overwriting(const CRC &crc, CRC::Value checksum);

#endif
