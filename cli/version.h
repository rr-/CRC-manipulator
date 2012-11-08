#ifndef VERSION_H
#define VERSION_H

#define MAJOR_VERSION 0
#define MINOR_VERSION 24

/*
 * Version 0.24:
 * - Simplified CLI parameters
 * - Removed CLI parameter support for types other than CRC32
 * - Removed --compute option
 * - Fixed bug in --position
 * - Added sanity check: input path == output path

 * Version 0.23:
 * - Added warning when not specifying all CRC digits

 * Version 0.22:
 * - CRC16-CCITT is now fully reversed (-> no brute force approach)
 * - CRC16-IBM as well :3
 * - Somewhat better OO design:
 *   - got rid of many unnecessary methods in CRC
 *   - made FileFactory, FileAdapter moved to IFile
 *     ...but still relying on "CRCType" >_____>
 * - Removed warning when printing to stdout without proper redirection

 * Version 0.21:
 * - Added progress functionality
 * - Greatly optimized I/O wrappers (buffer size increased from 1B to 8K)
 * - Negative file position support
 * - Added test suite
 * - Better makefile
 * - --help fits to 80 characters

 * Version 0.2:
 * - All CRC and File classes
 * - I/O buffered wrappers as opposed to reading whole thing into RAM
 * - CRC16-CCITT support
 * - CRC16-IBM support
 * - Fallback bruteforce approach
 * - Polynomial inverse algorithm (not used, though)
 * - Option to perform only checksum computation added
 * - Nice debug everywhere

 * Version 0.1:
 * - CRC32 support
 * - Arbitrary positionsupport
 * - Overwrite / insert options
 * - Reading from stdin and writing to stdio
 */

#endif
