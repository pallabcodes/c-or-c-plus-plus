// custom_write.c
// Implementation of robust write for custom printf
// References: POSIX write, Google C++ Style Guide

#include "custom_write.h"
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

ssize_t my_write(int fd, const void *buf, size_t count) {
    const char *ptr = (const char *)buf;
    size_t total_written = 0;
    while (total_written < count) {
        ssize_t written = write(fd, ptr + total_written, count - total_written);
        if (written < 0) {
            if (errno == EINTR) {
                continue; // Retry on interrupt
            }
            return -1; // Error
        }
        if (written == 0) {
            break; // No more data can be written
        }
        total_written += written;
    }
    return total_written;
}
