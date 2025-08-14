// custom_write.h
// Robust write implementation for custom printf
// References: POSIX write, Google C++ Style Guide

#ifndef CUSTOM_WRITE_H
#define CUSTOM_WRITE_H

#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Custom write function: handles partial writes, retries, error reporting
ssize_t my_write(int fd, const void *buf, size_t count);

#ifdef __cplusplus
}
#endif

#endif // CUSTOM_WRITE_H
