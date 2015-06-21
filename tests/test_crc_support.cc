#include <cstdio>
#include <string>
#include "File.h"
#include "catch.hh"
#include "test_crc_support.h"

void test_computing(const CRC &crc, CRCType checksum)
{
    Progress progress;
    std::string content = "123456789";

    auto inFile = File::fromFileName(
        "test-in.txt",
        File::Mode::Write | File::Mode::Read | File::Mode::Binary);

    inFile->write(content.data(), content.size());
    inFile->seek(0, File::Origin::Start);
    REQUIRE(crc.computeChecksum(*inFile, progress) == checksum);

    std::remove("test-in.txt");
}

void test_appending(const CRC &crc, CRCType checksum)
{
    Progress progress;
    std::string content = "test content";

    {
        auto inFile = File::fromFileName("test-in.txt", File::Mode::Write);
        inFile->write(content.data(), content.size());
        REQUIRE(inFile->getSize() == content.size());
    }

    auto size1 = content.size();
    auto size2 = size1 + crc.getNumBytes();
    {
        auto inFile = File::fromFileName(
            "test-in.txt", File::Mode::Read | File::Mode::Binary);
        auto outFile = File::fromFileName(
            "test-out.txt", File::Mode::Write | File::Mode::Binary);
        crc.applyPatch(
            checksum, size1, *inFile, *outFile, false, progress, progress);
    }

    {
        auto inFile = File::fromFileName(
            "test-out.txt", File::Mode::Read | File::Mode::Binary);
        REQUIRE(inFile->getSize() == size2);
        REQUIRE(crc.computeChecksum(*inFile, progress) == checksum);

        std::unique_ptr<char[]> buf(new char[size1]);
        inFile->seek(0, File::Origin::Start);
        inFile->read(buf.get(), size1);
        REQUIRE(std::string(buf.get()) == content);
        std::remove("test-in.txt");
        std::remove("test-out.txt");
    }
}

void test_inserting(const CRC &crc, CRCType checksum)
{
    Progress progress;
    std::string content = "test content";

    {
        auto inFile = File::fromFileName("test-in.txt", File::Mode::Write);
        inFile->write(content.data(), content.size());
        REQUIRE(inFile->getSize() == content.size());
    }

    size_t offset1 = content.size() / 2;
    size_t offset2 = offset1 + crc.getNumBytes();
    size_t size = content.size() + crc.getNumBytes();

    {
        auto inFile = File::fromFileName(
            "test-in.txt", File::Mode::Read | File::Mode::Binary);
        auto outFile = File::fromFileName(
            "test-out.txt", File::Mode::Write | File::Mode::Binary);
        crc.applyPatch(
            checksum, offset1, *inFile, *outFile, false, progress, progress);
    }

    {
        auto inFile = File::fromFileName(
            "test-out.txt", File::Mode::Read | File::Mode::Binary);
        REQUIRE(inFile->getSize() == size);
        REQUIRE(crc.computeChecksum(*inFile, progress) == checksum);

        std::unique_ptr<char[]> buf(new char[size]);
        inFile->seek(0, File::Origin::Start);
        inFile->read(buf.get(), size);
        REQUIRE(std::string(buf.get(), offset1) == content.substr(0, offset1));
        REQUIRE(std::string(buf.get() + offset2, size - offset2)
            == content.substr(offset1, size - offset2));
        std::remove("test-in.txt");
        std::remove("test-out.txt");
    }
}

void test_overwriting(const CRC &crc, CRCType checksum)
{
    Progress progress;
    std::string content = "test content";

    {
        auto inFile = File::fromFileName("test-in.txt", File::Mode::Write);
        inFile->write(content.data(), content.size());
        REQUIRE(inFile->getSize() == content.size());
    }

    size_t offset = content.size() - crc.getNumBytes();

    {
        auto inFile = File::fromFileName(
            "test-in.txt", File::Mode::Read | File::Mode::Binary);
        auto outFile = File::fromFileName(
            "test-out.txt", File::Mode::Write | File::Mode::Binary);
        crc.applyPatch(
            checksum, offset, *inFile, *outFile, true, progress, progress);
    }

    {
        auto inFile = File::fromFileName(
            "test-out.txt", File::Mode::Read | File::Mode::Binary);
        REQUIRE(inFile->getSize() == content.size());
        REQUIRE(crc.computeChecksum(*inFile, progress) == checksum);

        std::unique_ptr<char[]> buf(new char[offset]);
        inFile->seek(0, File::Origin::Start);
        inFile->read(buf.get(), offset);
        REQUIRE(std::string(buf.get(), offset) == content.substr(0, offset));
        std::remove("test-in.txt");
        std::remove("test-out.txt");
    }
}
