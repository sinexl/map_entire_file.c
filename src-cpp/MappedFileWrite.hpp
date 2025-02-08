#ifndef MAPPEDFILEWRITE_H_
#define MAPPEDFILEWRITE_H_

#include "MappedFileBase.hpp"
/*
 * Allows to modify contents of mapped file. Changes will affect file on disk.
 * Must be closed manually with .close()
 */
class MappedFileWrite : public MappedFileBase<char&> {
public:
    // Opens file automatically
    MappedFileWrite(const char* path);
    // In case you closed file manually and want to reuse the variable
    virtual void open(const char* path) override;
};

#endif // MAPPEDFILEWRITE_H_
