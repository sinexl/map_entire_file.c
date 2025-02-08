#ifndef MAP_FILES_H_
#define MAP_FILES_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

#ifndef _WIN32
#    include <errno.h>
#    include <fcntl.h>
#    include <string.h>
#    include <sys/mman.h>
#    include <sys/stat.h>
#    include <unistd.h>
#else
#    define WIN32_LEAN_AND_MEAN
#    include "windows.h"
#endif // _WIN32

#ifndef _WIN32 // POSIX
typedef int fd_t;
#    define Last_Error_Str() (strerror(errno))
#    define INVALID_FD (-1)
typedef enum {
    F_READ      = O_RDONLY,
    F_WRITE     = O_WRONLY,
    F_READWRITE = O_RDWR,
} FileOpenPermission;
typedef enum {
    M_READ  = PROT_READ,
    M_WRITE = PROT_WRITE,
} MapPermission;

#else // Windows
typedef HANDLE fd_t;
#    define Last_Error_Str() (win32_err_to_str(GetLastError()))
#    define INVALID_FD INVALID_HANDLE_VALUE
typedef enum {
    F_READ      = GENERIC_READ,
    F_WRITE     = GENERIC_WRITE,
    F_READWRITE = F_READ | F_WRITE,
} FileOpenPermission;
typedef enum {
    M_READ  = FILE_MAP_READ,
    M_WRITE = FILE_MAP_WRITE,
} MapPermission;
#ifdef __cplusplus
extern "C"{
#endif
char* win32_err_to_str(DWORD err);
#ifdef __cplusplus
}
#endif
#    ifndef WIN32_MSG_SIZE
#        define WIN32_MSG_SIZE (4 * 1024)
#    endif // WIN32_MSG_SIZE
#endif // _WIN32

#define return_defer(value) \
    do {                    \
        result = (value);   \
        goto defer;         \
    } while (0)

typedef enum {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_NOTHING,
} Log_Level;
typedef struct {
    char* data;
    size_t size;
} MappedFile;

#ifdef __cplusplus
extern "C" {
#endif
static void log_print(Log_Level level, const char* fmt, ...);
static fd_t fd_open(const char* path, int flags);
static void fd_close(fd_t fd);
static bool unmap_file(MappedFile* fm);
static bool map_entire_file(const char* path, MappedFile* fm, int permissions);
#ifdef MAP_FILES_IMPLEMENTATION

#ifdef _WIN32
char* win32_err_to_str(DWORD err)
{
    static char win32ErrMsg[WIN32_MSG_SIZE] = { 0 };
    DWORD errMsgSize = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, LANG_USER_DEFAULT, win32ErrMsg,
        WIN32_MSG_SIZE, NULL);

    if (errMsgSize == 0) {
        if (GetLastError() != ERROR_MR_MID_NOT_FOUND) {
            if (sprintf(win32ErrMsg, "Could not get error message for 0x%lX", err) > 0) {
                return (char*)&win32ErrMsg;
            } else {
                return NULL;
            }
        } else {
            if (sprintf(win32ErrMsg, "Invalid Windows Error code (0x%lX)", err) > 0) {
                return (char*)&win32ErrMsg;
            } else {
                return NULL;
            }
        }
    }

    while (errMsgSize > 1 && isspace(win32ErrMsg[errMsgSize - 1])) {
        win32ErrMsg[--errMsgSize] = '\0';
    }

    return win32ErrMsg;
}
#endif

void log_print(Log_Level level, const char* fmt, ...)
{
    switch (level) {
    case LOG_INFO:
        fprintf(stderr, "[INFO] ");
        break;
    case LOG_WARNING:
        fprintf(stderr, "[WARNING] ");
        break;
    case LOG_ERROR:
        fprintf(stderr, "[ERROR] ");
        break;
    case LOG_NOTHING:
        return;
    default:
        fprintf(stderr, "Unreachable: log_print\n");
        exit(1);
    }

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}

fd_t fd_open(const char* path, int flags)
{
#ifndef _WIN32
    if (flags & (F_READ | F_WRITE)) {
        flags &= ~(F_READ | F_WRITE);
        flags |= F_READWRITE;
    }
    fd_t result = open(path, flags);
    if (result < 0) {
        log_print(LOG_ERROR, "Could not open file %s: %s", path, Last_Error_Str());
        return INVALID_FD;
    }
    return result;
#else
    // https://docs.microsoft.com/en-us/windows/win32/fileio/opening-a-file-for-reading-or-writing
    SECURITY_ATTRIBUTES saAttr = { 0 };
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    wchar_t* wpath = malloc(sizeof(wchar_t) * (strlen(path) + 1));
    mbstowcs(wpath, path, strlen(path) + 1);
    DWORD acess = GENERIC_READ; 

    if (flags & F_WRITE) acess |= GENERIC_WRITE; 
    fd_t result = CreateFileW(
        wpath,
        acess,
        0,
        &saAttr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_READONLY,
        NULL);

    if (result == INVALID_HANDLE_VALUE) {
        log_print(LOG_ERROR, "Could not open file %s: %s", path, Last_Error_Str());
        free(wpath);
        return INVALID_FD;
    }
    free(wpath);
    return result;
#endif // _WIN32
}

void fd_close(fd_t fd)
{
#ifdef _WIN32
    CloseHandle(fd);
#else
    close(fd);
#endif // _WIN32
}


bool unmap_file(MappedFile* fm)
{
    if (fm->data == NULL) {
        log_print(LOG_ERROR, "Could not unmap file %p: fm->data is NULL", fm);
        return false;
    }
    bool result = true;

#ifndef _WIN32
    if (munmap((void*)fm->data, fm->size) == -1)
        return_defer(false);
    return_defer(true);

#else
    // https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-unmapviewoffile
    if (!UnmapViewOfFile(fm->data))
        return_defer(false);
    return_defer(true);
#endif

defer:
    if (!result)
        log_print(LOG_ERROR, "Could not unmap file %p: %s", fm->data, Last_Error_Str());
    if (result) {
        fm->data = NULL;
        fm->size = 0;
    }
    return result;
}


bool map_entire_file(const char* path, MappedFile* fm, int permissions)
{
#ifndef _WIN32
    char* mapped;
#else
    HANDLE mappingHandle;
    LARGE_INTEGER size;
#endif
    int custom_flags = 0;
    int open_flags = 0;
    if (permissions & M_READ)
        open_flags |= F_READ;
    if (permissions & M_WRITE)
        open_flags |= F_WRITE;
    bool result = true;
    fd_t fd = fd_open(path, open_flags);
    if (fd == INVALID_FD)
        return_defer(false);

#ifndef _WIN32
    struct stat finfo;
    if (fstat(fd, &finfo) == -1)
        return_defer(false);
    custom_flags = MAP_PRIVATE;
    if (permissions & M_WRITE)
        custom_flags = MAP_SHARED;
    mapped = (char*)mmap(NULL, finfo.st_size, permissions, custom_flags, fd, 0);
    if (mapped == MAP_FAILED)
        return_defer(false);

    fm->data = mapped;
    fm->size = finfo.st_size;

    return_defer(true);

#else
    // https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-createfilemappingw
    custom_flags = 0;
    if (permissions & M_WRITE)
        custom_flags = PAGE_READWRITE;
    else
        custom_flags = PAGE_READONLY;
    mappingHandle = CreateFileMappingW(fd, NULL, custom_flags, 0, 0, NULL);
    if (mappingHandle == NULL)
        return_defer(false);

    if (permissions & (M_READ | M_WRITE)) {
        custom_flags = FILE_MAP_ALL_ACCESS;
    } else if (permissions & M_READ) {
        custom_flags = FILE_MAP_READ;
    } else if (permissions & M_WRITE) {
        custom_flags = FILE_MAP_WRITE;
    }

    // https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-mapviewoffile
    fm->data = (char*)MapViewOfFile(mappingHandle, custom_flags, 0, 0, 0);
    if (fm->data == NULL)
        return_defer(false);

    if (!GetFileSizeEx(fd, &size))
        return_defer(false);
    fm->size = (size_t)size.QuadPart;

    return_defer(true);
#endif // _WIN32
defer: if (!result){
        log_print(LOG_ERROR, "Could not map file %s: %s", path, Last_Error_Str());
        memset(fm, 0, sizeof(*fm));
    }
    if (fd != INVALID_FD)
        fd_close(fd);
    return result;
}


#endif // MAP_FILES_IMPLEMENTATION
#ifdef __cplusplus
}
#endif // __cplusplus
#endif // MAP_FILES_H_
