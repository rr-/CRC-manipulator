#ifndef FILEBUFFEREDADAPTER_H
#define FILEBUFFEREDADAPTER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "IFile.h"

/**
 * Emulates seekable stream. Makes sense only with input streams.
 * Every write operation after seeking just once will throw an
 * exception.
 */

class FileBufferedAdapter : public IFile {
	friend class FileFactory;

	private:
		FILE* fileHandle;
		unsigned char* theBuffer;
		unsigned char* thePointer;
		IFile::OffsetType fileSize;
		IFile::OffsetType realSize;
		bool seeked;

	protected:
		FileBufferedAdapter(FILE* fileHandle);
		~FileBufferedAdapter();

	public:
		virtual const size_t getBufferSize() const;
		virtual const IFile::OffsetType getFileSize() const;
		virtual const IFile::OffsetType tell() const;
		virtual IFile& seek(const IFile::OffsetType& offset, IFile::SeekOrigin origin);
		virtual IFile& read(unsigned char* buffer, const size_t& size);
		virtual IFile& write(unsigned char* buffer, const size_t& size);
		virtual IFile& lock(const IFile::LockType& type, bool block);
		virtual IFile& unlock();
};

#endif
