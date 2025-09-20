#include <iostream>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#define SEM_NAME "/google_multi_sem"

int main() {
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0666, 0);
    if (sem == SEM_FAILED) { perror("sem_open"); return 1; }

    for (int i = 0; i < 2; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            std::cout << "Child " << i << " waiting for semaphore...\n";
            sem_wait(sem);
            std::cout << "Child " << i << " proceeding!\n";
            sem_close(sem);
            exit(0);
        }
    }

    sleep(2);
    std::cout << "Parent posts semaphore twice\n";
    sem_post(sem);
    sem_post(sem);

    for (int i = 0; i < 2; ++i) wait(NULL);

    sem_close(sem);
    sem_unlink(SEM_NAME);
    return 0;
}
