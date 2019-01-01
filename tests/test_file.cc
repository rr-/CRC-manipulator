#include "catch.hh"
#include "lib/file.h"

TEST_CASE("Reading and writing from files works", "[file]")
{
    const std::string testContent = "test";
    {
        auto f = File::fromFileName("test.txt", File::Mode::Write);
        f->write(testContent.data(), testContent.size());
    }

    {
        auto f = File::fromFileName("test.txt", File::Mode::Read);
        REQUIRE(f->getSize() == testContent.size());
        REQUIRE(f->tell() == 0);
        std::unique_ptr<char[]> buffer(new char[testContent.size()]);
        f->read(buffer.get(), testContent.size());
        REQUIRE(std::string(buffer.get(), testContent.size()) == testContent);
    }

    std::remove("test.txt");
}

TEST_CASE("Support for big file sizes works", "[file]")
{
    REQUIRE(sizeof(File::OffsetType) > sizeof(uint32_t));
}
