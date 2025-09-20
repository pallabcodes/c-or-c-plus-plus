#include <iostream>
#include <sys/select.h>
#include <unistd.h>

int main() {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    std::cout << "Type something within 5 seconds: ";
    fflush(stdout);

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    int ret = select(STDIN_FILENO + 1, &readfds, nullptr, nullptr, &timeout);
    if (ret > 0 && FD_ISSET(STDIN_FILENO, &readfds)) {
        char buf[128];
        ssize_t n = read(STDIN_FILENO, buf, sizeof(buf)-1);
        if (n > 0) {
            buf[n] = '\0';
            std::cout << "You typed: " << buf;
        }
    } else {
        std::cout << "\nTimeout or error.\n";
    }
    return 0;
}