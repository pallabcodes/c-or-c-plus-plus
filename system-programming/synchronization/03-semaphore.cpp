#include <semaphore.h>
#include <pthread.h>
#include <iostream>
#include <unistd.h>

sem_t sem;

void* worker(void* arg) {
    int id = *(int*)arg;
    sem_wait(&sem);
    std::cout << "Thread " << id << " entered critical section\n";
    sleep(1);
    std::cout << "Thread " << id << " leaving critical section\n";
    sem_post(&sem);
    return nullptr;
}

int main() {
    sem_init(&sem, 0, 2); // Allow 2 threads in critical section
    pthread_t t[4];
    int ids[4] = {1,2,3,4};
    for (int i = 0; i < 4; ++i)
        pthread_create(&t[i], nullptr, worker, &ids[i]);
    for (int i = 0; i < 4; ++i)
        pthread_join(t[i], nullptr);
    sem_destroy(&sem);
    return 0;
}