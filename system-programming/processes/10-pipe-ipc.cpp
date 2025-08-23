#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) { perror("pipe"); return 1; }

    pid_t pid = fork();
    if (pid == 0) {
        // Child: write to pipe
        close(pipefd[0]);
        const char *msg = "Hello from child!";
        write(pipefd[1], msg, strlen(msg));
        close(pipefd[1]);
        exit(0);
    } else {
        // Parent: read from pipe
        close(pipefd[1]);
        char buf[128] = {0};
        read(pipefd[0], buf, sizeof(buf)-1);
        std::cout << "Parent received: " << buf << std::endl;
        close(pipefd[0]);
        waitpid(pid, NULL, 0);
    }
    return 0;
}