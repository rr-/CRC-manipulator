#ifndef FILE_H
#define FILE_H

#if defined __CYGWIN__
    #define _FILE_OFFSET_BITS 64
    #define fseeko64(a,b,c) fseeko(a,b,c)
    #define ftello64(a) ftello(a)
    #define off64_t off_t
#endif
#include <stdio.h>

class File
{
    public:
        typedef off64_t OffsetType;
        enum SeekOrigin
        {
            FSEEK_BEGINNING,
            FSEEK_AHEAD,
            FSEEK_BEHIND,
            FSEEK_END
        };
        enum OpenMode
        {
            FOPEN_READ = 1,
            FOPEN_WRITE = 2,
            FOPEN_BINARY = 4
        };

    private:
        FILE* fileHandle;
        OffsetType fileSize;

    protected:
        File(FILE* fileHandle);

    public:
        ~File();

        size_t getBufferSize() const;
        OffsetType getFileSize() const;
        OffsetType tell() const;
        File &seek(const OffsetType &offset, SeekOrigin origin);
        File &read(unsigned char* buffer, const size_t &size);
        File &write(unsigned char* buffer, const size_t &size);

        static File* fromFileHandle(FILE* fileHandle);
        static File* fromFileName(const char* fileName, int openMode);
};

#endif
