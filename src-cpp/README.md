# About this class library
This is an abstraction layer over C functions for mapping files into memory.\
## This directory contains:
### Classes
- `MappedFileBase` (abstract class) - Base class for mapped files into memory
  - `MappedFileRead`  - Class for mapping files into memory with read/write permission.
  - `MappedFileWrite` - Class for mapping files into memory with readonly permission.
### Exceptions (./exceptions/): 
- `MappedIOException` - Base class for exceptions related to mapped IO 
  - `MappedFileCloseException` - Exception thrown on file unmapping errors 
  - `MappedFileOpenException`  - Exception thrown on file mapping errors. Contains const char* path.
