// buffer_manager.h
// Efficient output buffering for custom printf
// References: Efficient Buffering Algorithms (ACM)

#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include <stddef.h>

#define BUFFER_SIZE 4096

// Buffer structure
typedef struct {
    char data[BUFFER_SIZE];
    size_t pos;
} buffer_t;

// Initialize buffer
void buffer_init(buffer_t *buf);
// Write data to buffer, flush if needed
void buffer_write(buffer_t *buf, const char *data, size_t len, int fd);
// Flush buffer to file descriptor
void buffer_flush(buffer_t *buf, int fd);

#endif // BUFFER_MANAGER_H
