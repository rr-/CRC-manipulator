#ifndef CRC_FACTORIES_H
#define CRC_FACTORIES_H
#include <memory>
#include <vector>
#include "crc.h"

std::unique_ptr<CRC> createCRC32();
std::unique_ptr<CRC> createCRC32POSIX();
std::unique_ptr<CRC> createCRC16CCITT();
std::unique_ptr<CRC> createCRC16MODEM();
std::unique_ptr<CRC> createCRC16IBM();
std::vector<std::shared_ptr<CRC>> createAllCRC();

#endif
