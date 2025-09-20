#include <semaphore.h>
#include <iostream>

int main() {
    sem_t sem;
    if (sem_init(&sem, 0, 1) != 0) {
        perror("sem_init");
        return 1;
    }
    if (sem_wait(&sem) != 0) {
        perror("sem_wait");
    }
    if (sem_post(&sem) != 0) {
        perror("sem_post");
    }
    if (sem_destroy(&sem) != 0) {
        perror("sem_destroy");
    }
    return 0;
}