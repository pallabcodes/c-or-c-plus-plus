#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <cstring>

#define SHM_NAME "/google_shm_demo"
#define SEM_NAME "/google_sem_demo"

int main() {
    // Create shared memory object
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) { perror("shm_open"); return 1; }
    ftruncate(shm_fd, sizeof(int));

    // Map shared memory
    int *shared_int = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_int == MAP_FAILED) { perror("mmap"); return 1; }
    *shared_int = 0;

    // Create semaphore for synchronization
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0666, 0);
    if (sem == SEM_FAILED) { perror("sem_open"); return 1; }

    pid_t pid = fork();
    if (pid == 0) {
        // Child: update shared memory
        *shared_int = 42;
        std::cout << "Child wrote 42 to shared memory\n";
        sem_post(sem); // Signal parent
        munmap(shared_int, sizeof(int));
        close(shm_fd);
        sem_close(sem);
        exit(0);
    } else {
        // Parent: wait for child
        sem_wait(sem);
        std::cout << "Parent read from shared memory: " << *shared_int << std::endl;
        waitpid(pid, NULL, 0);
        munmap(shared_int, sizeof(int));
        close(shm_fd);
        sem_close(sem);
        sem_unlink(SEM_NAME);
        shm_unlink(SHM_NAME);
    }
    return 0;
}