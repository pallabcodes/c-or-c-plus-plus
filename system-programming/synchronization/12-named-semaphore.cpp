#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>

int main() {
    sem_t* sem = sem_open("/google_sem", O_CREAT, 0666, 0);
    if (sem == SEM_FAILED) { perror("sem_open"); return 1; }

    pid_t pid = fork();
    if (pid == 0) {
        // Child: signal parent
        sleep(1);
        std::cout << "Child posting semaphore\n";
        sem_post(sem);
        sem_close(sem);
        _exit(0);
    } else {
        // Parent: wait for child
        std::cout << "Parent waiting on semaphore\n";
        sem_wait(sem);
        std::cout << "Parent received signal\n";
        waitpid(pid, nullptr, 0);
        sem_close(sem);
        sem_unlink("/google_sem");
    }
    return 0;
}