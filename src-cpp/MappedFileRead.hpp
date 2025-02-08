#ifndef MAPPEDFILEREAD_H_
#define MAPPEDFILEREAD_H_
#include "MappedFileBase.hpp"
/*
 * Allows to read contents of file as a char[].
 * Must be closed manually with .close()
 */
class MappedFileRead : public MappedFileBase<char> {
public:
    // Opens file automatically
    MappedFileRead(const char* path);
    // In case you closed file manually and want to reuse the variable
    virtual void open(const char* path) override;
};

#endif // MAPPEDFILEREAD_H_
