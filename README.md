# map_entire_file.c

Cross-platform example of mapping files into memory written in C \
Partially inspired from [tsoding/nob.h](https://github.com/tsoding/nob.h/)

# Contents of this repository
1. [`map_files.h`](https://github.com/sinexl/map_entire_file.c/blob/main/map_files.h)(C/C++) - Single-header library that abstracts from OS-specific way to map files into memory.
2. [`Map file class library`](https://github.com/sinexl/map_entire_file.c/tree/main/src-cpp)(C++ only) - Abstraction layer over `map_files.h` which provides classes for convenient working with mapped files.
***
- If you need first part of this library, just copy [`map_files.h`](https://github.com/sinexl/map_entire_file.c/blob/main/map_files.h) file to your project, define a macro and include the file (See example below)
- If you need second part of this library, you need to copy both [`src-cpp/`](https://github.com/sinexl/map_entire_file.c/tree/main/src-cpp) and [`map_files.h`](https://github.com/sinexl/map_entire_file.c/blob/main/map_files.h) to your project 
# Usage
``` c
// This is header-only library, so you need to define MAP_FILES_IMPLEMENTATION
// to use it
#define MAP_FILES_IMPLEMENTATION
#include "map_files.h"
#include <stdio.h>

int main(int argc, const char** argv)
{
    MappedFile file = { 0 };
    if (argc < 2) {
        printf("Usage: %s <path>\n", argv[0]);
        return 1;
    }

    bool result = map_entire_file(argv[1], &file, M_READ | M_WRITE);
    if (!result) return -1;

    // We can now access file data as char []
    for (size_t i = 0; i < file.size; i++) {
        printf("%c", file.data[i]);
    }

    // We can also edit mapped file as char[] (Only if we have M_READ flag set)
    // Changes in the array will also affect file on disk
    file.data[2] = ';';


    // Unmap file and reset the variable
    unmap_file(&file);
}
```
