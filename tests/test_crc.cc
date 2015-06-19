#include <cstdio>
#include <string>
#include "catch.hh"
#include "File/File.h"
#include "test_crc.h"

void test_appending(std::unique_ptr<CRC> crc, CRCType checksum)
{
    std::string content = "test content";

    {
        auto inputFile = File::fromFileName("test-in.txt", File::Mode::Write);
        inputFile->write(content.data(), content.size());
        REQUIRE(inputFile->getSize() == content.size());
    }

    {
        auto inputFile = File::fromFileName(
            "test-in.txt", File::Mode::Read | File::Mode::Binary);
        auto outputFile = File::fromFileName(
            "test-out.txt", File::Mode::Write | File::Mode::Binary);
        crc->applyPatch(checksum, content.size(), *inputFile, *outputFile, false);
    }

    {
        auto inputFile = File::fromFileName(
            "test-out.txt", File::Mode::Read | File::Mode::Binary);
        REQUIRE(inputFile->getSize() == content.size() + crc->getNumBytes());
        REQUIRE(crc->computeChecksum(*inputFile) == checksum);

        std::unique_ptr<char[]> buf(new char[content.size()]);
        inputFile->seek(0, File::Origin::Start);
        inputFile->read(buf.get(), content.size());
        REQUIRE(std::string(buf.get()) == content);
        std::remove("test-in.txt");
        std::remove("test-out.txt");
    }
}

void test_computing(std::unique_ptr<CRC> crc, CRCType checksum)
{
    std::string content = "123456789";

    auto inputFile = File::fromFileName(
        "test-in.txt",
        File::Mode::Write | File::Mode::Read | File::Mode::Binary);

    inputFile->write(content.data(), content.size());
    inputFile->seek(0, File::Origin::Start);
    REQUIRE(crc->computeChecksum(*inputFile) == checksum);
}
