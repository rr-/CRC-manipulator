#ifndef FILEADAPTER_H
#define FILEADAPTER_H

#include <stdio.h>
#include <assert.h>
#include "../debug.h"

class IFile {
	public:
		typedef long long int OffsetType;
		enum SeekOrigin {
			FSEEK_BEGINNING,
			FSEEK_AHEAD,
			FSEEK_BEHIND,
			FSEEK_END
		};
		enum OpenMode {
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


	public:
		virtual const size_t getBufferSize() const = 0;
		virtual const OffsetType getFileSize() const = 0;
		virtual const OffsetType tell() const = 0;
		virtual IFile& seek(const OffsetType& offset, SeekOrigin origin) = 0;
		virtual IFile& read(unsigned char* buffer, const size_t& size) = 0;
		virtual IFile& write(unsigned char* buffer, const size_t& size) = 0;
};

#endif
