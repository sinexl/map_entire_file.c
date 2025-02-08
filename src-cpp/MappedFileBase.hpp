#ifndef MAPPEDFILEBASE_H_
#define MAPPEDFILEBASE_H_
#include "MappedIOException.hpp"
#define MAP_FILES_IMPLEMENTATION
#include "map_files.h"
#include <stdexcept>

/*
 * Base class for mapped files (abstract)
 * Requires implementation of .open()
 *
 */
template <typename T>
class MappedFileBase { // Abstract class

protected:
    MappedFile fs;

public:
    // Does not open file automatically
    MappedFileBase();

    virtual void open(const char *path) = 0;
    // Should be closed manually for objects of derived class,
    // unless they override the destructor
    // Resets the variable
    virtual void close();
    T operator[](size_t index);
    size_t size() const noexcept;
    bool is_open() const noexcept;

    // Doesn't do anything in base class
    virtual ~MappedFileBase();
    MappedFileBase(MappedFileBase &&);
    MappedFileBase &operator=(MappedFileBase &&);

private:
    MappedFileBase(const MappedFileBase&) { };
    MappedFileBase& operator=(const MappedFileBase&) { }
};

template <typename T>
MappedFileBase<T>::MappedFileBase()
{
    fs.size = 0;
    fs.data = nullptr;
}

template <typename T>
void MappedFileBase<T>::close()
{
    bool res = unmap_file(&fs);
    if (!res)
        throw MappedFileCloseException();
}

template <typename T>
size_t MappedFileBase<T>::size() const noexcept
{
    return fs.size;
}

template <typename T>
T MappedFileBase<T>::operator[](size_t index)
{
    if (fs.data == nullptr)
        throw std::logic_error("File is not open");
    if (index > fs.size) {
        throw std::out_of_range("MappedFile: index out of range");
    }
    return fs.data[index];
}

template <typename T>
bool MappedFileBase<T>::is_open() const noexcept
{
    return this->fs.data != nullptr;
}

template <typename T>
MappedFileBase<T>::MappedFileBase(MappedFileBase<T>&& going_to_be_destroyed)
{
    this->fs = std::move(going_to_be_destroyed.fs);
    going_to_be_destroyed.fs.data = nullptr;
    going_to_be_destroyed.fs.size = 0;
}
template <typename T>
MappedFileBase<T>& MappedFileBase<T>::operator=(MappedFileBase<T>&& going_to_be_destroyed) {
    if (this != &going_to_be_destroyed)
    {
        this->fs = std::move(going_to_be_destroyed.fs);
        going_to_be_destroyed.fs.data = nullptr;
        going_to_be_destroyed.fs.size = 0;
    }
    return *this;
}

template <typename T>
MappedFileBase<T>::~MappedFileBase()
{
}
#endif // MAPPEDFILEBASE_H_
