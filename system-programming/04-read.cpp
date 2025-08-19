#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int main () {
    int fd;
    char buffer[100]; // this is where thay bytes that aread will be stored i.e. a char buffer of size 100

    fd = open("hello.txt", O_RDONLY, 0644);
    


    // when file is opened, if successful then 0 else -1
    if (fd < 0) {
        perror("Error opening the file\n");
        exit(1);
    }

    int readCount;

    readCount = read(fd, buffer, sizeof(buffer));

    while (readCount > 0) {
        // Print the bytes read
        printf("Read %d bytes: %.*s\n", readCount, readCount, buffer);
        // Read again
        readCount = read(fd, buffer, sizeof(buffer));
    }

    if (close(fd) < 0) {
        perror("Error closing the file\n");
        exit(1);
    } else {
        printf("File closed successfully\n");
    }

    return 0;
}