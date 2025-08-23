#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        // Child process: use execv to run /bin/echo with arguments
        char *args[] = { (char*)"echo", (char*)"Hello from execv!", nullptr };
        execv("/bin/echo", args);
        // If execv fails
        perror("execv");
        _exit(1);
    } else {
        // Parent process: wait for child to finish
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            std::cout << "Child exited with status " << WEXITSTATUS(status) << std::endl;
        } else {
            std::cout << "Child did not exit normally" << std::endl;
        }
    }
    return 0;
}