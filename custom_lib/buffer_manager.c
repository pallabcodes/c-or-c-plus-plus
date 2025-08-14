// buffer_manager.c
// Implementation of efficient output buffering for custom printf
// References: Efficient Buffering Algorithms (ACM)

#include "buffer_manager.h"
#include <unistd.h>
#include <string.h>

void buffer_init(buffer_t *buf) {
    buf->pos = 0;
}

void buffer_write(buffer_t *buf, const char *data, size_t len, int fd) {
    size_t remaining = BUFFER_SIZE - buf->pos;
    if (len > remaining) {
        memcpy(buf->data + buf->pos, data, remaining);
        buf->pos += remaining;
        buffer_flush(buf, fd);
        data += remaining;
        len -= remaining;
        // Write remaining data recursively
        buffer_write(buf, data, len, fd);
    } else {
        memcpy(buf->data + buf->pos, data, len);
        buf->pos += len;
        if (buf->pos == BUFFER_SIZE) {
            buffer_flush(buf, fd);
        }
    }
}

void buffer_flush(buffer_t *buf, int fd) {
    if (buf->pos > 0) {
        write(fd, buf->data, buf->pos);
        buf->pos = 0;
    }
}
