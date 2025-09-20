#include <iostream>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("lock-demo.txt", O_RDWR | O_CREAT, 0644);
    if (fd < 0) { perror("open"); return 1; }

    if (flock(fd, LOCK_EX | LOCK_NB) == 0) {
        std::cout << "File locked. Press Enter to release...\n";
        getchar();
        flock(fd, LOCK_UN);
    } else {
        std::cout << "Could not lock file (already locked).\n";
    }
    close(fd);
    return 0;
}