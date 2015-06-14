#include "File.h"
#include "../debug.h"
#include <string.h>
#include <assert.h>

File *File::fromFileHandle(FILE *fileHandle)
{
    return new File(fileHandle);
}

File *File::fromFileName(const char *fileName, int openMode)
{
    char modeString[5];
    strcpy(modeString, "");

    if ((openMode & FOPEN_WRITE) && (openMode & FOPEN_READ))
        strcat(modeString, "r+");
    else if (openMode & FOPEN_WRITE)
        strcat(modeString, "w");
    else if (openMode & FOPEN_READ)
        strcat(modeString, "r");

    if (openMode & FOPEN_BINARY)
        strcat(modeString, "b");

    assert(fileName != NULL);
    FILE *fileHandle = fopen(fileName, modeString);
    if (fileHandle == NULL)
    {
        pmesg(ERRLEV_ERROR, "I/O error\n");
        throw ERR_IO_ERROR;
    }
    return fromFileHandle(fileHandle);
}

File::File(FILE *fileHandle) : fileHandle(NULL)
{
    this->fileHandle = fileHandle;

    bool ok = true;
    ok &= (fseeko64(this->fileHandle, 0, SEEK_END) != -1);
    ok &= ((this->fileSize = ftello64(this->fileHandle)) != -1);
    ok &= (fseeko64(this->fileHandle, 0, SEEK_SET)) != -1;

    if (!ok)
    {
        this->fileSize = -1;
    }
}

File::~File()
{
    fclose(this->fileHandle);
}

File &File::seek(
    const OffsetType &offset,
    SeekOrigin origin)
{
    OffsetType destination = 0;
    switch (origin)
    {
        case FSEEK_AHEAD:
            destination = this->tell() + offset;
            break;
        case FSEEK_BEHIND:
            destination = this->tell() - offset;
            break;
        case FSEEK_BEGINNING:
            destination = offset;
            break;
        case FSEEK_END:
            destination = this->getFileSize() - offset;
            break;
        default:
            pmesg(ERRLEV_ERROR, "invalid parameter (%d)\n", origin);
            throw ERR_INVALID_PARAMETER;
    }

    if (this->getFileSize() == -1)
    {
        pmesg(ERRLEV_WARNING, "seeking on unseekable stream\n");
        return *this;
    }

    if (destination < 0 || destination > this->getFileSize())
    {
        pmesg(ERRLEV_WARNING,
            "invalid position (not 0<=%lld<=%lld)\n",
            destination,
            this->getFileSize());
        throw ERR_INVALID_POSITION;
    }

    int result = fseeko64(this->fileHandle, offset, SEEK_SET);

    if (result != 0)
    {
        pmesg(ERRLEV_ERROR, "I/O error\n");
        throw ERR_IO_ERROR;
    }
    return *this;
}

File::OffsetType File::tell() const
{
    off_t ret = ftello64(this->fileHandle);
    if (ret == -1L)
    {
        pmesg(ERRLEV_ERROR, "I/O error\n");
        throw ERR_IO_ERROR;
    }
    return ((OffsetType) ret);
}

File &File::read(unsigned char *buffer, const size_t &size)
{
    if (((OffsetType) this->tell()) + ((OffsetType) size) > this->getFileSize())
    {
        pmesg(ERRLEV_WARNING,
            "insufficient content (%lld + %lld > %lld)\n",
            this->tell(),
            (OffsetType) size,
            this->getFileSize());
        throw ERR_INSUFFICIENT_CONTENT;
    }
    if (fread(buffer, sizeof(unsigned char), size, this->fileHandle) != size)
    {
        pmesg(ERRLEV_ERROR,
            "I/O error (size: %ld, position: %ld)\n",
            size,
            this->tell());
        throw ERR_IO_ERROR;
    }
    return *this;
}

File &File::write(unsigned char *buffer, const size_t &size)
{
    if (fwrite(buffer, sizeof(unsigned char), size, this->fileHandle) != size)
    {
        pmesg(ERRLEV_ERROR, "I/O error\n");
        throw ERR_IO_ERROR;
    }
    if (this->fileSize != -1)
    {
        if (this->fileSize < this->tell())
        {
            this->fileSize = this->tell();
        }
    }
    return *this;
}

size_t File::getBufferSize() const
{
    return 8192;
}

File::OffsetType File::getFileSize() const
{
    return this->fileSize;
}
