#include "FileFactory.h"

IFile* FileFactory::fromFileHandle(FILE* fileHandle) {
	assert(fileHandle != NULL);
	bool seekable = fseek(fileHandle, 0, SEEK_SET) == 0;
	pmesg(ERRLEV_INFO, "File %x is %s\n", fileno(fileHandle), seekable ? "seekable" : "NOT seekable");
	if (seekable)
		return new FileDirectAdapter(fileHandle);
	else
		return new FileBufferedAdapter(fileHandle);
}

IFile* FileFactory::fromFileName(const char* fileName, int openMode) {
	char modeString[5];
	strcpy(modeString, "");
	if ((openMode & IFile::FOPEN_WRITE) &&(openMode & IFile::FOPEN_READ)) {
		strcat(modeString, "r+");
	} else if (openMode & IFile::FOPEN_WRITE) {
		strcat(modeString, "w");
	} else if (openMode & IFile::FOPEN_READ) {
		strcat(modeString, "r");
	}
	if (openMode & IFile::FOPEN_BINARY) {
		strcat(modeString, "b");
	}


	assert(fileName != NULL);
	FILE* fileHandle = fopen(fileName, modeString);
	if (fileHandle == NULL) {
		pmesg(ERRLEV_ERROR, "I/O error\n");
		throw IFile::ERR_IO_ERROR;
	}
	return fromFileHandle(fileHandle);
}
