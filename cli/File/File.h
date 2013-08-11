#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#if defined(unix)
	#include <fcntl.h>
#else
	#include <Windows.h>
	#include <io.h>
#endif

class File
{
	public:
		typedef off_t OffsetType;
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
		const static int ERR_INSUFFICIENT_MEMORY = 1;
		const static int ERR_INSUFFICIENT_CONTENT = 2;
		const static int ERR_INVALID_POSITION = 3;
		const static int ERR_INVALID_PARAMETER = 4;
		const static int ERR_IO_ERROR = 5;
		const static int ERR_NOT_IMPLEMENTED = 6;

	private:
		FILE* fileHandle;
		OffsetType fileSize;

	protected:
		File(FILE* fileHandle);

	public:
		~File();

		virtual const size_t getBufferSize() const;
		virtual const OffsetType getFileSize() const;
		virtual const OffsetType tell() const;
		virtual File& seek(
			const OffsetType& offset,
			SeekOrigin origin);

		virtual File& read(unsigned char* buffer, const size_t& size);
		virtual File& write(unsigned char* buffer, const size_t& size);

		static File* fromFileHandle(FILE* fileHandle);
		static File* fromFileName(const char* fileName, int openMode);
};

#endif
