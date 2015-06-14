#ifndef DEBUG_H
#define DEBUG_H

enum ErrorLevel
{
	ERRLEV_DEBUG,
	ERRLEV_INFO,
	ERRLEV_NOTICE,
	ERRLEV_WARNING,
	ERRLEV_ERROR,
	ERRLEV_CRITICAL
};

#if defined(NDEBUG) && defined(__GNUC__)
	#define pmesg(format, args...) ((void) 0)
#else
	#include <stdio.h>
	#include <stdarg.h>

	void __pmesg(
		int level,
		const char* file,
		unsigned int line,
		const char* function,
		const char* format,
		...);

	#define pmesg(level, format, ...) \
		__pmesg(level, \
			__FILE__, __LINE__, __FUNCTION__, \
			format, ## __VA_ARGS__)
#endif

#endif
