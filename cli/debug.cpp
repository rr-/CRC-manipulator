#include "debug.h"

extern int msglevel; /* the higher, the more messages... */

#if defined(NDEBUG) && defined(__GNUC__)
#else
	void __pmesg (int level, const char* file, unsigned int line, const char* function, const char* format, ...) {
		#ifdef NDEBUG
		#else
			if (level != ERRLEV_INFO/* && level != ERRLEV_DEBUG*/) {
				fprintf (stderr, "%s:%d in %s():\n", file, line, function);
			}
			va_list args;
			va_start (args, format);
			vfprintf (stderr, format, args);
			va_end (args);
			fflush (stderr);
		#endif
	}
#endif
