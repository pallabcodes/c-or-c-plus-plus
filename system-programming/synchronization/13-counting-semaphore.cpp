#include <semaphore.h>
#include <pthread.h>
#include <iostream>
#include <unistd.h>

sem_t sem;

void* worker(void* arg) {
    int id = *(int*)arg;
    sem_wait(&sem);
    std::cout << "Thread " << id << " acquired resource\n";
    sleep(2);
    std::cout << "Thread " << id << " releasing resource\n";
    sem_post(&sem);
    return nullptr;
}

int main() {
    sem_init(&sem, 0, 3); // Resource pool of size 3
    pthread_t t[6];
    int ids[6] = {1,2,3,4,5,6};
    for (int i = 0; i < 6; ++i)
        pthread_create(&t[i], nullptr, worker, &ids[i]);
    for (int i = 0; i < 6; ++i)
        pthread_join(t[i], nullptr);
    sem_destroy(&sem);
    return 0;
}