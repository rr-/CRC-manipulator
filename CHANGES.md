### 0.3.4
- Fixed -p, -i and -o arguments
- Added unit tests so these options never break again

### 0.3.3
- Added unit tests using Catch framework

### 0.3.2
- Fixed broken CRC16CCITT support (even though it's currently unused)
  - Fixed patch computing
  - Changed the algorithm - turns out the parameters were wrong since day zero

### 0.3.1
- Fixed build problems

### 0.3
- Switched to waf build system (replaces shell scripts)
- Switched to Qt
- Improved overall code quality
  - Incorporated C++11 features:
    - smart pointers
    - throwing integers replaced with proper exceptions
    - improved type system
  - Removed debug macros
- Switched to semantic versioning
- Changed version maintenance to be based on git tags

### 0.2.6
- Fixed -p argument
- Fixed --overwrite argument
- Removed pipe support (its performance was deplorable, anyway)
- Simplified code
- Replaced makefile with a shell script that compiles fine under GNU/Linux,
  Cygwin and MinGW

### 0.2.5
- Support for files larger than 4GB

### 0.2.4
- Simplified CLI parameters
  - Removed CLI parameter support for types other than CRC32
  - Removed --compute option
- Fixed bug in --position
- Added sanity check: input path == output path

### 0.2.3
- Added warning when not specifying all CRC digits

### 0.2.2
- Fully reversed CRC16
  - Removed brute force routines
- Improved code quality
    - got rid of many unnecessary methods in CRC
    - added `FileFactory`
    - moved `FileAdapter` `IFile`
- Removed warning when printing to stdout without proper redirection

### 0.2.1
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
