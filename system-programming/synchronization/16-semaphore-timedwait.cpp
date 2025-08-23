#include <semaphore.h>
#include <iostream>
#include <ctime>

int main() {
    sem_t sem;
    sem_init(&sem, 0, 0);

    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 2; // Wait up to 2 seconds

    std::cout << "Waiting for semaphore (timeout 2s)...\n";
    int ret = sem_timedwait(&sem, &ts);
    if (ret == -1) {
        perror("sem_timedwait");
    } else {
        std::cout << "Semaphore acquired!\n";
    }
    sem_destroy(&sem);
    return 0;
}