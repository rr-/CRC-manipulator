#ifndef TEST_CRC_SUPPORT_H
#define TEST_CRC_SUPPORT_H
#include "lib/crc.h"

void testComputing(const CRC &crc, CRC::Value checksum);
void testAppending(const CRC &crc, CRC::Value checksum);
void testInserting(const CRC &crc, CRC::Value checksum);
void testOverwriting(const CRC &crc, CRC::Value checksum);

#endif
