#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main() {
    const char *filename = "seek-demo.txt";
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        printf("Error opening file '%s': %s\n", filename, strerror(errno));
        return 1;
    }

    // Write initial data
    const char *initial_text = "Hello, this is the original content.\n";
    ssize_t written = write(fd, initial_text, strlen(initial_text));
    if (written < 0) {
        printf("Error writing initial data: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    // Seek to a specific position (e.g., overwrite "original" with "updated")
    off_t seek_pos = 18; // Position after "Hello, this is the "
    if (lseek(fd, seek_pos, SEEK_SET) == (off_t)-1) {
        printf("Error seeking in file: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    // Overwrite part of the file
    const char *update_text = "updated";
    written = write(fd, update_text, strlen(update_text));
    if (written < 0) {
        printf("Error writing update: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    close(fd);
    printf("Successfully wrote and updated '%s' using seek.\n", filename);
    return 0;
}

