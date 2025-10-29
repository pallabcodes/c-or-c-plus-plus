#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <unistd.h>

static void print_error(const char* context) {
    std::fprintf(stderr, "%s failed: %s\n", context, std::strerror(errno));
}

int main() {
    const char* shm_name = "/ipc_template_shm";
    const char* sem_name = "/ipc_template_sem";

    int shm_fd = -1;
    sem_t* sem = SEM_FAILED;
    int* shared_int = nullptr;

    // create shared memory
    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0660);
    if (shm_fd == -1) { print_error("shm_open"); goto cleanup; }
    if (ftruncate(shm_fd, sizeof(int)) == -1) { print_error("ftruncate"); goto cleanup; }

    shared_int = static_cast<int*>(mmap(nullptr, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    if (shared_int == MAP_FAILED) { shared_int = nullptr; print_error("mmap"); goto cleanup; }
    *shared_int = 0;

    sem = sem_open(sem_name, O_CREAT, 0660, 0);
    if (sem == SEM_FAILED) { print_error("sem_open"); goto cleanup; }

    pid_t pid = fork();
    if (pid == -1) {
        print_error("fork");
        goto cleanup;
    }

    if (pid == 0) {
        // child process
        *shared_int = 1;
        sem_post(sem);
        _exit(0);
    }

    if (sem_wait(sem) == -1) { print_error("sem_wait"); goto cleanup; }
    std::cout << "value in shared memory: " << *shared_int << std::endl;
    waitpid(pid, nullptr, 0);

cleanup:
    if (shared_int) munmap(shared_int, sizeof(int));
    if (shm_fd != -1) close(shm_fd);
    if (sem != SEM_FAILED) sem_close(sem);
    sem_unlink(sem_name);
    shm_unlink(shm_name);
    return 0;
}


