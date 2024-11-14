#include <ctype.h>
#include <stdio.h>
// This is header-only library, so you need to define MAP_FILES_IMPLEMENTATION
// to use it
#define MAP_FILES_IMPLEMENTATION
#include "map_files.h"

int main(void)
{
    MappedFile f = {0};
    if(!map_entire_file("./hello.txt", &f, M_READ | M_WRITE))
    {
        return 1;
    }

    // read test
    for (size_t i = 0; i < f.size; i++) {
        printf("%c", f.data[i]);
    }
    printf("Size: %zu\n", f.size);


    //write test
    for (size_t i = 0; i < f.size; i++) {
        f.data[i] = toupper(f.data[i]);
    }


    // unmap test
    unmap_file(&f);

    return 0;
}
