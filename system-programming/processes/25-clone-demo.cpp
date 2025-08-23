#define _GNU_SOURCE
#include <iostream>
#include <sched.h>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>

// Stack for the child process
char child_stack[1024 * 1024];

// Function executed by the child
int child_func(void *arg) {
    std::cout << "Child PID (clone): " << getpid() << ", arg: " << (char*)arg << std::endl;
    return 0;
}

int main() {
    const char *msg = "Hello from clone!";
    // CLONE_VM: share memory, CLONE_FS: share filesystem info, CLONE_FILES: share file descriptors
    int flags = SIGCHLD; // Only signal on child exit (like fork)
    pid_t pid = clone(child_func, child_stack + sizeof(child_stack), flags, (void*)msg);
    if (pid < 0) {
        perror("clone");
        return 1;
    }
    std::cout << "Parent PID: " << getpid() << ", clone child PID: " << pid << std::endl;
    waitpid(pid, nullptr, 0);
    return 0;
}