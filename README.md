# map_entire_file.c

Cross-platform example of mapping files into memory written in C \
Partially inspired from [tsoding/nob.h](https://github.com/tsoding/nob.h/)

# Usage

``` c
// This if header-only library, so you need to define MAP_FILES_IMPLEMENTATION
// to use it
#define MAP_FILES_IMPLEMENTATION
#include "map_files.h"
#include <stdio.h>

int main(int argc, const char** argv)
{
    MappedFile file = { 0 };
    if (argc < 2) return -1;

    bool result = map_entire_file(argv[1], &file, M_READ | M_WRITE);
    if (!result) return -1;

    // We can now access file data as char []
    for (size_t i = 0; i < file.size; i++) {
        printf("%c", file.data[i]);
    }

    // We can also edit mapped file as char[] (Only if we have M_READ flag set)
    // Changes in the array will also affect file on disk
    file.data[2] = ';';


    // Unmap file and reset the structure
    unmap_file(&file);
}

```
