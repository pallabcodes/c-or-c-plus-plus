#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define BUF_SIZE 4096

int main() {
    const char *filename = "input.txt";
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        printf("Error opening file '%s': %s\n", filename, strerror(errno));
        return 1;
    }

    char buffer[BUF_SIZE];
    ssize_t bytes_read;

    // Read file in chunks and print to stdout
    while ((bytes_read = read(fd, buffer, BUF_SIZE)) > 0) {
        ssize_t total_written = 0;
        while (total_written < bytes_read) {
            ssize_t bytes_written = write(STDOUT_FILENO, buffer + total_written, bytes_read - total_written);
            if (bytes_written < 0) {
                printf("Error writing to stdout: %s\n", strerror(errno));
                close(fd);
                return 1;
            }
            total_written += bytes_written;
        }
    }

    if (bytes_read < 0) {
        printf("Error reading from file '%s': %s\n", filename, strerror(errno));
    }

    close(fd);
    return 0;
}