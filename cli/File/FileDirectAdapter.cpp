#include "FileDirectAdapter.h"

FileDirectAdapter::FileDirectAdapter(FILE* fileHandle) : fileHandle(NULL) {
	this->fileHandle = fileHandle;
	fseek(this->fileHandle, 0, SEEK_END);
	this->fileSize = ftell(this->fileHandle);
	fseek(this->fileHandle, 0, SEEK_SET);
}

FileDirectAdapter::~FileDirectAdapter() {
	fclose(this->fileHandle);
}

IFile& FileDirectAdapter::seek(const IFile::OffsetType& offset, IFile::SeekOrigin origin) {
	int result = 0;
	switch(origin) {
		case FSEEK_AHEAD:
			if (this->tell() + offset < 0 || this->tell() + offset > this->getFileSize()) {
				pmesg(ERRLEV_WARNING, "invalid position(not 0<=%ld<=%ld)\n", this->tell() + offset, this->getFileSize());
				throw ERR_INVALID_POSITION;
			}
			result = fseek(this->fileHandle, static_cast<long int>(offset), SEEK_CUR);
			break;
		case FSEEK_BEHIND:
			if (this->tell() - offset < 0 || this->tell() - offset > this->getFileSize()) {
				pmesg(ERRLEV_WARNING, "invalid position(not 0<=%ld<=%ld)\n", this->tell() - offset, this->getFileSize());
				throw ERR_INVALID_POSITION;
			}
			result = fseek(this->fileHandle, - static_cast<long int>(offset), SEEK_CUR);
			break;
		case FSEEK_BEGINNING:
			if (offset < 0 || offset > this->getFileSize()) {
				pmesg(ERRLEV_WARNING, "invalid position(not 0<=%ld<=%ld)\n", offset, this->getFileSize());
				throw ERR_INVALID_POSITION;
			}
			result = fseek(this->fileHandle, static_cast<long int>(offset), SEEK_SET);
			break;
		case FSEEK_END:
			if (offset < 0 || offset > this->getFileSize()) {
				pmesg(ERRLEV_WARNING, "invalid position(not 0<=%ld<=%ld)\n", offset, this->getFileSize());
				throw ERR_INVALID_POSITION;
			}
			result = fseek(this->fileHandle, static_cast<long int>(offset), SEEK_END);
			break;
		default:
			pmesg(ERRLEV_ERROR, "invalid parameter(%d)\n", origin);
			throw ERR_INVALID_PARAMETER;
	}
	if (result != 0) {
		pmesg(ERRLEV_ERROR, "I/O error\n");
		throw ERR_IO_ERROR;
	}
	return *this;
}

const IFile::OffsetType FileDirectAdapter::tell() const {
	int ret = ftell(this->fileHandle);
	if (ret == -1L) {
		pmesg(ERRLEV_ERROR, "I/O error\n");
		throw ERR_IO_ERROR;
	}
	return static_cast<IFile::OffsetType>(ret);
}

IFile& FileDirectAdapter::unlock() {
	#if defined(unix)
		if (!flock(this->fileHandle, LOCK_UN)) {
			pmesg(ERRLEV_ERROR, "I/O error\n");
			throw ERR_IO_ERROR;
		}
	#else
		throw ERR_NOT_IMPLEMENTED;
	#endif
	return *this;
}




IFile& FileDirectAdapter::lock(const IFile::LockType& type, bool block) {
	#if defined(unix)
		int operation;
		switch(type) {
			case FLOCK_EX:
				operation = LOCK_EX;
				break;
			case FLOCK_SH:
				operation = LOCK_SH;
				break;
			default:
				pmesg(ERRLEV_ERROR, "invalid parameter(%d)\n", operation);
				throw ERR_INVALID_PARAMETER;
		}
		if (!flock(this->fileHandle, operation)) {
			pmesg(ERRLEV_ERROR, "I/O error\n");
			throw ERR_IO_ERROR;
		}
	#else
		throw ERR_NOT_IMPLEMENTED;
	#endif
	return *this;
}

IFile& FileDirectAdapter::read(unsigned char* buffer, const size_t& size) {
	if (this->tell() + static_cast<IFile::OffsetType>(size) > this->getFileSize()) {
		pmesg(ERRLEV_WARNING, "insufficient content(%ld + %ld > %ld)\n", this->tell(), size, this->getFileSize());
		throw ERR_INSUFFICIENT_CONTENT;
	}
	if (fread(buffer, sizeof(unsigned char), size, this->fileHandle) != size) {
		pmesg(ERRLEV_ERROR, "I/O error(size: %ld, position: %ld)\n", size, this->tell());
		throw ERR_IO_ERROR;
	}
	return *this;
}

IFile& FileDirectAdapter::write(unsigned char* buffer, const size_t& size) {
	if (fwrite(buffer, sizeof(unsigned char), size, this->fileHandle) != size) {
		pmesg(ERRLEV_ERROR, "I/O error\n");
		throw ERR_IO_ERROR;
	}
	if (this->fileSize < this->tell())
		this->fileSize = this->tell();
	return *this;
}

const size_t FileDirectAdapter::getBufferSize() const {
	return 8192;
}

const IFile::OffsetType FileDirectAdapter::getFileSize() const {
	return this->fileSize;
}
