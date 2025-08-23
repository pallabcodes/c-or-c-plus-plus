#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        // Fork failed
        perror("fork");
        return 1;
    } else if (pid == 0) {
        // Child process
        printf("Child process (PID: %d)\n", getpid());
        // Replace child process image with /bin/ls
        execl("/bin/ls", "ls", "-l", NULL);
        // If execl fails
        perror("execl");
        exit(1);
    } else {
        // Parent process
        printf("Parent process (PID: %d), waiting for child...\n", getpid());
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return 1;
        }
        if (WIFEXITED(status)) {
            printf("Child exited with status %d\n", WEXITSTATUS(status));
        } else {
            printf("Child did not exit normally\n");
        }
    }
    return 0;
}