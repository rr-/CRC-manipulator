#ifndef FILE_H
#define FILE_H

#include <string>
#include <memory>

#if defined __CYGWIN__
    #define _FILE_OFFSET_BITS 64
    #define fseeko64(a,b,c) fseeko(a,b,c)
    #define ftello64(a) ftello(a)
    #define off64_t off_t
#endif

class File
{
    public:
        typedef off64_t OffsetType;

        enum class Origin : uint8_t
        {
            Start,
            Ahead,
            Behind,
            End
        };

        enum Mode
        {
            Read = 1,
            Write = 2,
            Binary = 4
        };

        ~File();

        OffsetType getSize() const;
        OffsetType tell() const;
        File &seek(OffsetType offset, Origin origin);
        File &read(unsigned char *buffer, size_t size);
        File &write(unsigned char *buffer, size_t size);

        static std::unique_ptr<File> fromFileHandle(FILE *fileHandle);
        static std::unique_ptr<File> fromFileName(
            const std::string &fileName, int mode);

    private:
        File(FILE *fileHandle);
        FILE *fileHandle;
        OffsetType fileSize;
};

#endif
