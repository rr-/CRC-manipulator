#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "FileDirectAdapter.h"
#include "FileBufferedAdapter.h"
#include "../debug.h"

/**
 * Lightweight, low level class encapsulating
 * basic file operations (kind of like std::ifstream).
 * Utilizes adapter pattern in order to provide
 * pseudo-seeking over nonseekable streams.
 */

class FileFactory {
	protected:
		FileFactory();
		~FileFactory();

	public:
		static IFile* fromFileHandle(FILE* fileHandle);
		static IFile* fromFileName(const char* fileName, int openMode);
};

#endif
