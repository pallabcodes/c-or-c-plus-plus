#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Demonstrates setsid, setpgid, getpgid, getpgrp for process group/session management

int main() {
    std::cout << "Parent PID: " << getpid() << ", PGID: " << getpgrp() << std::endl;

    pid_t pid = fork();
    if (pid == 0) {
        // Child process: create a new session and process group
        pid_t sid = setsid(); // Become session leader
        if (sid < 0) { perror("setsid"); _exit(1); }
        setpgid(0, 0); // Set PGID to own PID
        std::cout << "Child PID: " << getpid()
                  << ", SID: " << sid
                  << ", PGID: " << getpgrp()
                  << std::endl;
        _exit(0);
    } else if (pid > 0) {
        waitpid(pid, nullptr, 0);
    } else {
        perror("fork");
        return 1;
    }
    return 0;
}