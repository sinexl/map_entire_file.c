#include "MappedFileWrite.hpp"

MappedFileWrite::MappedFileWrite(const char* path)
    : MappedFileBase<char&>()
{
    this->open(path);
}

virtual void MappedFileWrite::open(const char* path) override
{
    bool res = map_entire_file(path, &fs, M_READ | M_WRITE);
    if (!res)
        throw MappedFileOpenException(Last_Error_Str(), path);
}
