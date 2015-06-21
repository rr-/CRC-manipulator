#ifndef FILE_H
#define FILE_H
#include <string>
#include <memory>
#include "config.h"

class File
{
    public:
        #if HAVE_OFF64T
            typedef off64_t OffsetType;
        #else
            typedef off_t OffsetType;
        #endif

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
        File &read(char *buffer, size_t size);
        File &read(unsigned char *buffer, size_t size);
        File &write(const char *buffer, size_t size);
        File &write(const unsigned char *buffer, size_t size);

        static std::unique_ptr<File> fromFileHandle(FILE *fileHandle);
        static std::unique_ptr<File> fromFileName(
            const std::string &fileName, int mode);

    private:
        File(FILE *fileHandle);
        FILE *fileHandle;
        OffsetType fileSize;
};

#endif
