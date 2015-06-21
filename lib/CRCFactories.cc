#include "CRCFactories.h"

std::unique_ptr<CRC> createCRC32()
{
    CRC::Specs specs = {};
    specs.name       = "CRC32";
    specs.numBytes   = 4;
    specs.polynomial = 0x04C11DB7;
    specs.initialXOR = 0xFFFFFFFF;
    specs.finalXOR   = 0xFFFFFFFF;
    specs.test       = 0xCBF43926;
    return std::unique_ptr<CRC>(new CRC(specs));
}

std::unique_ptr<CRC> createCRC32POSIX()
{
    CRC::Specs specs = {};
    specs.name       = "CRC32POSIX";
    specs.numBytes   = 4;
    specs.polynomial = 0x04C11DB7;
    specs.initialXOR = 0x00000000;
    specs.finalXOR   = 0xFFFFFFFF;
    specs.flags      = CRC::Flags::BigEndian | CRC::Flags::UseFileSize;
    specs.test       = 0x377A6011;
    return std::unique_ptr<CRC>(new CRC(specs));
}

std::unique_ptr<CRC> createCRC16CCITT()
{
    CRC::Specs specs = {};
    specs.name       = "CRC16CCITT";
    specs.numBytes   = 2;
    specs.polynomial = 0x1021;
    specs.initialXOR = 0x0000;
    specs.finalXOR   = 0x0000;
    specs.test       = 0x2189;
    return std::unique_ptr<CRC>(new CRC(specs));
}

std::unique_ptr<CRC> createCRC16IBM()
{
    CRC::Specs specs = {};
    specs.name       = "CRC16IBM";
    specs.numBytes   = 2;
    specs.polynomial = 0x8005;
    specs.initialXOR = 0x0000;
    specs.finalXOR   = 0x0000;
    specs.test       = 0xBB3D;
    return std::unique_ptr<CRC>(new CRC(specs));
}

std::vector<std::shared_ptr<CRC>> createAllCRC()
{
    std::vector<std::shared_ptr<CRC>> crcs;
    crcs.push_back(createCRC32());
    crcs.push_back(createCRC32POSIX());
    crcs.push_back(createCRC16CCITT());
    crcs.push_back(createCRC16IBM());
    return crcs;
}
