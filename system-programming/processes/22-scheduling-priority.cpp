#include <iostream>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/wait.h>

// Demonstrates nice, setpriority, getpriority for process scheduling

int main() {
    pid_t pid = fork();
    if (pid == 0) {
        // Child: lower its priority
        int old_prio = getpriority(PRIO_PROCESS, 0);
        setpriority(PRIO_PROCESS, 0, old_prio + 10);
        int new_prio = getpriority(PRIO_PROCESS, 0);
        std::cout << "Child PID: " << getpid()
                  << ", Priority changed from " << old_prio << " to " << new_prio << std::endl;
        _exit(0);
    } else if (pid > 0) {
        waitpid(pid, nullptr, 0);
    } else {
        perror("fork");
        return 1;
    }
    return 0;
}