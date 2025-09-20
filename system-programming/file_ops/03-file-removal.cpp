
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main() {
    const char *filename = "testfile.txt";

    // Try to remove the file using the unlink system call
    if (unlink(filename) == 0) {
        printf("File '%s' removed successfully.\n", filename);
    } else {
        // If removal fails, print the error
        printf("Error removing file '%s': %s\n", filename, strerror(errno));
    }

    return 0;
}
