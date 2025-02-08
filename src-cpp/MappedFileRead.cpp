#include "MappedFileRead.hpp"

MappedFileRead::MappedFileRead(const char* path)
    : MappedFileBase<char>()
{
    this->open(path);
}

void MappedFileRead::open(const char* path) 
{
    bool res = map_entire_file(path, &fs, M_READ);
    if (!res)
        throw MappedFileOpenException(Last_Error_Str(), path);
}
