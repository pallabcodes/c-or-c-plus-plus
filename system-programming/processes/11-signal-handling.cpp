#include <iostream>
#include <signal.h>
#include <unistd.h>

void handler(int sig) {
    std::cout << "Caught signal " << sig << std::endl;
}

int main() {
    signal(SIGUSR1, handler);
    pid_t pid = fork();
    if (pid == 0) {
        sleep(1);
        kill(getppid(), SIGUSR1);
        exit(0);
    } else {
        pause(); // Wait for signal
    }
    return 0;
}