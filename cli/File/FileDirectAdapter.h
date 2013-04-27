#ifndef FILEDIRECTADAPTER_H
#define FILEDIRECTADAPTER_H

#include <stdio.h>
#if defined(unix)
	#include <fcntl.h>
#else
	#include <Windows.h>
	#include <io.h>
#endif
#include "IFile.h"

class FileDirectAdapter : public IFile {
	friend class FileFactory;

	private:
		FILE* fileHandle;
		IFile::OffsetType fileSize;

	protected:
		FileDirectAdapter(FILE* fileHandle);
		~FileDirectAdapter();

	public:
		virtual const size_t getBufferSize() const;
		virtual const IFile::OffsetType getFileSize() const;
		virtual const IFile::OffsetType tell() const;
		virtual IFile& seek(const IFile::OffsetType& offset, IFile::SeekOrigin origin);
		virtual IFile& read(unsigned char* buffer, const size_t& size);
		virtual IFile& write(unsigned char* buffer, const size_t& size);
};

#endif
