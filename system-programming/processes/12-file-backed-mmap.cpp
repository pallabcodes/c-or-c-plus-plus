#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>

int main() {
    int fd = open("mmap-demo.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
    const char *text = "Google mmap demo!";
    write(fd, text, strlen(text));
    lseek(fd, 0, SEEK_SET);

    char *mapped = (char*)mmap(NULL, strlen(text), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped == MAP_FAILED) { perror("mmap"); return 1; }

    std::cout << "Mapped file content: " << mapped << std::endl;
    strcpy(mapped, "Updated by mmap!");
    msync(mapped, strlen(text), MS_SYNC);

    munmap(mapped, strlen(text));
    close(fd);
    return 0;
}