#ifndef MAPPEDFILEBASE_H_
#define MAPPEDFILEBASE_H_
#define MAP_FILES_IMPLEMENTATION
#include "map_files.h"
#include <exception>
#include <stdexcept>

class MappedIOException : public std::exception {
protected:
    std::string message;

public:
    explicit MappedIOException(const std::string& msg)
        : message(msg)
    {
    }

    const char* what() const noexcept override
    {
        return message.c_str();
    }
};

class MappedFileOpenException : public MappedIOException {
private:
    const char* path = nullptr;
public:
    MappedFileOpenException()
        : MappedIOException("Error: Unable to open mapped file.")
    {
    }

    explicit MappedFileOpenException(const std::string& msg, const std::string& path = "")
        : MappedIOException("Error: Unable to open mapped file: " + msg)
    {
        this->path = path.c_str();
    }
};

class MappedFileCloseException : public MappedIOException {
public:
    MappedFileCloseException()
        : MappedIOException("Error: Unable to close mapped file.")
    {
    }

    explicit MappedFileCloseException(const std::string& msg)
        : MappedIOException("Error: Unable to close mapped file: " + msg)
    {
    }
};

template <typename T>
class MappedFileBase { // Abstract class
protected:
    MappedFile fs;

public:
    MappedFileBase()
    {
        fs.size = 0;
        fs.data = nullptr;
    }

    virtual void open(const char* path) = 0;
    virtual void close()
    {
        bool res = unmap_file(&fs);
        if (!res)
            throw MappedFileCloseException();
    }

    size_t size() const
    {
        return fs.size;
    }

    T operator[](size_t index)
    {
        if (fs.data == nullptr)
            throw std::logic_error("File is not open");
        if (index > fs.size) {
            throw std::out_of_range("MappedFile: index out of range");
        }
        return fs.data[index];
    }
    virtual ~MappedFileBase()
    {
    }
};

class MappedFileRead : public MappedFileBase<char> {
public:
    MappedFileRead(const char* path)
        : MappedFileBase<char>()
    {
        this->open(path);
    }

    virtual void open(const char* path) override
    {
        bool res = map_entire_file(path, &fs, M_READ);
        if (!res)
            throw MappedFileOpenException(Last_Error_Str(), path);
    }
};

class MappedFileWrite : public MappedFileBase<char&> {
public:
    MappedFileWrite(const char* path)
        : MappedFileBase<char&>()
    {
        this->open(path);
    }

    virtual void open(const char* path) override
    {
        bool res = map_entire_file(path, &fs, M_READ | M_WRITE);
        if (!res)
            throw MappedFileOpenException(Last_Error_Str(), path);
    }
};
#endif // MAPPEDFILEBASE_H_
