#include "FileBufferedAdapter.h"

FileBufferedAdapter::FileBufferedAdapter(FILE* fileHandle) : fileHandle(NULL), theBuffer(NULL), thePointer(NULL), realSize(0), seeked(false) {
	this->fileHandle = fileHandle;
	size_t bytesRead = 0;
	size_t oldSize = 0;
	unsigned char* readBuffer =(unsigned char*) malloc(this->getBufferSize());
	if (!readBuffer) {
		pmesg(ERRLEV_CRITICAL, "Insufficient memory!\n");
		throw ERR_INSUFFICIENT_MEMORY;
	}
	while ((bytesRead = fread(readBuffer, 1, this->getBufferSize(), fileHandle)) != 0) {
		oldSize = this->realSize;
		this->realSize += bytesRead;
		this->theBuffer =(unsigned char*) realloc(this->theBuffer, this->realSize);
		if (!this->theBuffer) {
			free(readBuffer);
			pmesg(ERRLEV_CRITICAL, "Insufficient memory!\n");
			throw ERR_INSUFFICIENT_MEMORY;
		}
		memcpy(this->theBuffer + oldSize, readBuffer, bytesRead);
	}
	free(readBuffer);
	this->fileSize = this->realSize;
	pmesg(ERRLEV_DEBUG, "Real size: %d\n", this->realSize);
	thePointer = theBuffer;
}

FileBufferedAdapter::~FileBufferedAdapter() {
	fclose(this->fileHandle);
	free(this->theBuffer);
	this->theBuffer = NULL;
	this->thePointer = NULL;
}

IFile& FileBufferedAdapter::lock(const IFile::LockType& type, bool block) {
	throw ERR_NOT_IMPLEMENTED;
}

IFile& FileBufferedAdapter::unlock() {
	throw ERR_NOT_IMPLEMENTED;
}


IFile& FileBufferedAdapter::seek(const IFile::OffsetType& offset, SeekOrigin origin) {
	unsigned char* oldPointer = this->thePointer;
	switch(origin) {
		case FSEEK_BEHIND:
			if (this->tell() - offset < 0 || this->tell() - offset > this->getFileSize()) {
				pmesg(ERRLEV_WARNING, "invalid position(not 0<=%ld<=%ld)\n", this->tell() - offset, this->getFileSize());
				throw ERR_INVALID_POSITION;
			}
			this->thePointer -= offset;
			break;
		case FSEEK_AHEAD:
			if (this->tell() + offset < 0 || this->tell() + offset > this->getFileSize()) {
				pmesg(ERRLEV_WARNING, "invalid position(not 0<=%ld<=%ld)\n", this->tell() + offset, this->getFileSize());
				throw ERR_INVALID_POSITION;
			}
			this->thePointer += offset;
			break;
		case FSEEK_BEGINNING:
			if (offset < 0 || offset > this->getFileSize()) {
				pmesg(ERRLEV_WARNING, "invalid position(not 0<=%ld<=%ld)\n", offset, this->getFileSize());
				throw ERR_INVALID_POSITION;
			}
			this->thePointer = this->theBuffer + offset;
			break;
		case FSEEK_END:
			if (offset < 0 || offset > this->getFileSize()) {
				pmesg(ERRLEV_WARNING, "invalid position(not 0<=%ld<=%ld)\n", offset, this->getFileSize());
				throw ERR_INVALID_POSITION;
			}
			this->thePointer = this->theBuffer + this->getFileSize() - offset;
			break;
		default:
			pmesg(ERRLEV_ERROR, "invalid parameter(%d)\n", origin);
			throw ERR_INVALID_PARAMETER;
	}
	if (oldPointer != this->thePointer) {
		pmesg(ERRLEV_NOTICE, "Warning: seeked in nonseekable stream. Next write operation will fail.(%8p -> %8p, offset:%ld, origin:%d)\n", oldPointer, this->thePointer, offset, origin);
		this->seeked = true;
	}
	return *this;
}

const IFile::OffsetType FileBufferedAdapter::tell() const {
	return static_cast<IFile::OffsetType>(this->thePointer - this->theBuffer);
}

IFile& FileBufferedAdapter::read(unsigned char* buffer, const size_t& size) {
	if (this->tell() + static_cast<IFile::OffsetType>(size) > this->getFileSize()) {
		pmesg(ERRLEV_WARNING, "insufficient content(%ld + %ld > %ld)\n", this->tell(), size, this->getFileSize());
		throw ERR_INSUFFICIENT_CONTENT;
	}
	for (size_t i = 0; i < size; i ++) {
		buffer[i] = *this->thePointer;
		++ this->thePointer;
	}
	return *this;
}

IFile& FileBufferedAdapter::write(unsigned char* buffer, const size_t& size) {
	if (this->seeked) {
		pmesg(ERRLEV_ERROR, "Seeked in nonseekable stream and trying to write.\n", __FILE__, __LINE__, __FUNCTION__);
		throw ERR_IO_ERROR;
	}
	IFile::OffsetType position1 = this->thePointer - this->theBuffer;
	IFile::OffsetType position2 = position1 + static_cast<IFile::OffsetType>(size);
	assert(position1 < position2);
	// reallocate buffer if needed
	while (position2 >= this->realSize) {
		pmesg(ERRLEV_DEBUG, "Increasing file buffer size(%ld >= %ld)\n", position2, this->realSize);
		this->realSize += this->getBufferSize();
		this->theBuffer =(unsigned char*) realloc(this->theBuffer, this->realSize);
	}
	pmesg(ERRLEV_DEBUG, "Buffer size now = %ld\n", this->realSize);
	assert(position2 < this->realSize);
	this->thePointer = this->theBuffer + position1;
	memcpy(this->thePointer, buffer, size);
	this->thePointer = this->theBuffer + position2;
	// write to stream
	if (fwrite(buffer, sizeof(unsigned char), size, this->fileHandle) != size) {
		pmesg(ERRLEV_ERROR, "I/O error\n");
		throw ERR_IO_ERROR;
	}
	return *this;
}

const size_t FileBufferedAdapter::getBufferSize() const {
	return 8192;
}

const IFile::OffsetType FileBufferedAdapter::getFileSize() const {
	return this->fileSize;
}
