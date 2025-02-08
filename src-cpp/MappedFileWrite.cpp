#include "MappedFileWrite.hpp"

MappedFileWrite::MappedFileWrite(const char* path)
    : MappedFileBase<char&>()
{
    this->open(path);
}

void MappedFileWrite::open(const char* path)
{
    bool res = map_entire_file(path, &fs, M_READ | M_WRITE);
    if (!res)
        throw MappedFileOpenException(Last_Error_Str(), path);
}
