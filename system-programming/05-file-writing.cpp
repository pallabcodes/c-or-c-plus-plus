
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define BUF_SIZE 4096

int main() {
    const char *src_filename = "input.txt";
    const char *dst_filename = "output.txt";
    int src_fd, dst_fd;
    char buffer[BUF_SIZE];
    ssize_t bytes_read, bytes_written;

    // Open source file for reading
    src_fd = open(src_filename, O_RDONLY);
    if (src_fd < 0) {
        printf("Error opening source file '%s': %s\n", src_filename, strerror(errno));
        return 1;
    }

    // Open destination file for writing (create if it doesn't exist, truncate if it does)
    dst_fd = open(dst_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd < 0) {
        printf("Error opening destination file '%s': %s\n", dst_filename, strerror(errno));
        close(src_fd);
        return 1;
    }

    // Copy data from source to destination in chunks
    while ((bytes_read = read(src_fd, buffer, BUF_SIZE)) > 0) {
        ssize_t total_written = 0;
        while (total_written < bytes_read) {
            bytes_written = write(dst_fd, buffer + total_written, bytes_read - total_written);
            if (bytes_written < 0) {
                printf("Error writing to destination file '%s': %s\n", dst_filename, strerror(errno));
                close(src_fd);
                close(dst_fd);
                return 1;
            }
            total_written += bytes_written;
        }
    }
    if (bytes_read < 0) {
        printf("Error reading from source file '%s': %s\n", src_filename, strerror(errno));
        close(src_fd);
        close(dst_fd);
        return 1;
    }

    // Close both files
    close(src_fd);
    close(dst_fd);
    printf("Successfully copied '%s' to '%s'.\n", src_filename, dst_filename);
    return 0;
}

