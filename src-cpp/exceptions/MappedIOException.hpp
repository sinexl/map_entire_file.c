#ifndef MAPPEDIOEXCEPTION_H_
#define MAPPEDIOEXCEPTION_H_
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
public:
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

#endif // MAPPEDIOEXCEPTION_H_
