#include <cstdio>
#include "File/File.h"

std::unique_ptr<File> File::fromFileHandle(FILE *fileHandle)
{
    return std::unique_ptr<File>(new File(fileHandle));
}

std::unique_ptr<File> File::fromFileName(
    const std::string &fileName, int mode)
{
    std::string modeString;

    if ((mode & Mode::Write) && (mode & Mode::Read))
        modeString += "r+";
    else if (mode & Mode::Write)
        modeString += "w";
    else if (mode & Mode::Read)
        modeString += "r";

    if (mode & Mode::Binary)
        modeString += "b";

    FILE *fileHandle = fopen(fileName.c_str(), modeString.c_str());
    if (fileHandle == nullptr)
        throw std::runtime_error("Couldn't open file");
    return fromFileHandle(fileHandle);
}

File::File(FILE *fileHandle) : fileHandle(fileHandle)
{
    try
    {
        seek(0, Origin::End);
        fileSize = tell();
        seek(0, Origin::Start);
    }
    catch (...)
    {
        fileSize = -1;
    }
}

File::~File()
{
    fclose(fileHandle);
}

File &File::seek(OffsetType offset, Origin origin)
{
    if (getSize() == -1)
        throw std::runtime_error("Stream is unseekable");

    int type;

    OffsetType destination = 0;
    switch (origin)
    {
        case Origin::Ahead:
            destination = offset;
            type = SEEK_CUR;
            break;

        case Origin::Behind:
            destination = - offset;
            type = SEEK_CUR;
            break;

        case Origin::Start:
            destination = offset;
            type = SEEK_SET;
            break;

        case Origin::End:
            destination = offset;
            type = SEEK_END;
            break;

        default:
            throw std::invalid_argument("Bad offset type");
    }

    int result = fseeko64(fileHandle, destination, type);
    if (result != 0)
        throw std::runtime_error("Failed to seek file");

    return *this;
}

File::OffsetType File::tell() const
{
    off_t ret = ftello64(fileHandle);
    if (ret == -1L)
        throw std::runtime_error("Stream is unseekable");
    return ret;
}

File &File::read(unsigned char *buffer, size_t size)
{
    OffsetType newPos = tell() + static_cast<OffsetType>(size);

    if (newPos > getSize())
        throw std::runtime_error("Trying to read content beyond EOF");

    if (fread(buffer, sizeof(unsigned char), size, fileHandle) != size)
        throw std::runtime_error("Can't read bytes at " + std::to_string(tell()));

    return *this;
}

File &File::write(unsigned char *buffer, size_t size)
{
    if (fwrite(buffer, sizeof(unsigned char), size, fileHandle) != size)
        throw std::runtime_error("Can't write bytes");

    if (fileSize >= 0 && fileSize < tell())
        fileSize = tell();

    return *this;
}

File::OffsetType File::getSize() const
{
    return fileSize;
}
