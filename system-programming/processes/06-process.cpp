#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <cstring>
#include <cstdlib>
#include <sys/wait.h>

// Demonstrates process memory mapping and address space isolation
// to run ./process-demo 12 16.

/**
 * Process States: Start -> Ready -> Running -> Waiting -> Terminated
 * Start: this state represents the initial state of a process when a process is created and started.
 * Ready: this state represents a process that is ready to run but is not currently executing.
 * Running: this state represents a process that is currently being executed by the CPU.
 * Waiting: this state represents a process that is waiting for some event to occur (e.g., I/O completion).
 * Terminated: this state represents a process that has completed execution and is no longer active.
 */

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <val1> <val2>\n";
        return 1;
    }

    // Map two integers into memory (private to each process after fork)
    int *mapped_val1 = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    int *mapped_val2 = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (mapped_val1 == MAP_FAILED || mapped_val2 == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    *mapped_val1 = atoi(argv[1]);
    *mapped_val2 = atoi(argv[2]);

    std::cout << "Parent PID: " << getpid() << std::endl;
    std::cout << "Parent mapped_val1: " << *mapped_val1 << " at " << mapped_val1 << std::endl;
    std::cout << "Parent mapped_val2: " << *mapped_val2 << " at " << mapped_val2 << std::endl;

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        munmap(mapped_val1, sizeof(int));
        munmap(mapped_val2, sizeof(int));
        return 1;
    } else if (pid == 0) {
        // Child process
        std::cout << "\nChild PID: " << getpid() << std::endl;
        std::cout << "Child mapped_val1: " << *mapped_val1 << " at " << mapped_val1 << std::endl;
        std::cout << "Child mapped_val2: " << *mapped_val2 << " at " << mapped_val2 << std::endl;

        // Change values in child
        *mapped_val1 += 100;
        *mapped_val2 += 200;

        std::cout << "Child updated mapped_val1: " << *mapped_val1 << std::endl;
        std::cout << "Child updated mapped_val2: " << *mapped_val2 << std::endl;

        sleep(5); // Allow inspection
        munmap(mapped_val1, sizeof(int));
        munmap(mapped_val2, sizeof(int));
        exit(0);
    } else {
        // Parent process
        sleep(2); // Wait for child to update its memory
        std::cout << "\nParent after child update:" << std::endl;
        std::cout << "Parent mapped_val1: " << *mapped_val1 << std::endl;
        std::cout << "Parent mapped_val2: " << *mapped_val2 << std::endl;

        waitpid(pid, NULL, 0);

        munmap(mapped_val1, sizeof(int));
        munmap(mapped_val2, sizeof(int));
    }

    return 0;
}