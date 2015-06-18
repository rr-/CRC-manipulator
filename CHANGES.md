### Current
- Switched to waf build system (replaces shell scripts)
- Switched to Qt
- Improved overall code quality
  - Incorporated C++11 features:
    - smart pointers
    - throwing integers replaced with proper exceptions
    - improved type system
  - Removed debug macros

### 0.26
- Fixed -p argument
- Fixed --overwrite argument
- Removed pipe support (its performance was deplorable, anyway)
- Simplified code
- Replaced makefile with a shell script that compiles fine under GNU/Linux,
  Cygwin and MinGW

### 0.25
- Support for files larger than 4GB

### 0.24
- Simplified CLI parameters
  - Removed CLI parameter support for types other than CRC32
  - Removed --compute option
- Fixed bug in --position
- Added sanity check: input path == output path

### 0.23
- Added warning when not specifying all CRC digits

### 0.22
- Fully reversed CRC16
  - Removed brute force routines
- Improved code quality
    - got rid of many unnecessary methods in CRC
    - added `FileFactory`
    - moved `FileAdapter` `IFile`
- Removed warning when printing to stdout without proper redirection

### 0.21
- Optimized I/O wrappers (buffer size increased from 1B to 8K)
- Added simple progressbar
- Added support for negative --position
- Changed --help so that it fits to 80 characters
- Improved makefile

### 0.2
- Wrapped everything in `CRC` and `File` classes
- Introduced I/O buffered wrappers that replace reading whole file into RAM
- Added initial CRC16-CCITT support
- Added initial CRC16-IBM support
- Added fallback bruteforce approach
- Added polynomial inverse algorithm (not yet used)
- Added option to compute the checksum rather than patching the file
- Added nice debug everywhere

### 0.1
- CRC32 support
- Support for arbitrary position
- Option to overwrite or insert the patch
- Support for reading from stdin and writing to stdio
