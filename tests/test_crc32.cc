#include <cstdio>
#include <string>
#include "catch.hh"
#include "File/File.h"
#include "CRC/CRC32.h"

TEST_CASE("CRC32 works", "[crc32]")
{
    std::unique_ptr<CRC> crc(new CRC32);
    std::string content = "test content";

    {
        auto inputFile = File::fromFileName("test-in.txt", File::Mode::Write);
        inputFile->write(content.data(), content.size());
        REQUIRE(inputFile->getSize() == content.size());
    }

    {
        auto inputFile = File::fromFileName("test-in.txt", File::Mode::Read | File::Mode::Binary);
        auto outputFile = File::fromFileName("test-out.txt", File::Mode::Write | File::Mode::Binary);
        crc->applyPatch(0xdeadbeef, content.size(), *inputFile, *outputFile, false);
    }

    {
        auto inputFile = File::fromFileName("test-out.txt", File::Mode::Read | File::Mode::Binary);
        REQUIRE(inputFile->getSize() == content.size() + crc->getNumBytes());
        REQUIRE(crc->computeChecksum(*inputFile) == 0xdeadbeef);

        std::unique_ptr<char[]> buf(new char[content.size()]);
        inputFile->seek(0, File::Origin::Start);
        inputFile->read(buf.get(), content.size());
        REQUIRE(std::string(buf.get()) == content);
        std::remove("test-in.txt");
        std::remove("test-out.txt");
    }
}
