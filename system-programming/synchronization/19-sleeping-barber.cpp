#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#include <unistd.h>

const int CHAIRS = 3;
sem_t customers, barbers, mutex;
int waiting = 0;

void* barber(void*) {
    while (true) {
        sem_wait(&customers);
        sem_wait(&mutex);
        --waiting;
        sem_post(&mutex);
        std::cout << "Barber is cutting hair\n";
        sleep(2);
        sem_post(&barbers);
    }
    return nullptr;
}

void* customer(void* arg) {
    int id = *(int*)arg;
    sem_wait(&mutex);
    if (waiting < CHAIRS) {
        ++waiting;
        std::cout << "Customer " << id << " waiting\n";
        sem_post(&customers);
        sem_post(&mutex);
        sem_wait(&barbers);
        std::cout << "Customer " << id << " getting haircut\n";
    } else {
        sem_post(&mutex);
        std::cout << "Customer " << id << " leaving (no chair)\n";
    }
    return nullptr;
}

int main() {
    sem_init(&customers, 0, 0);
    sem_init(&barbers, 0, 0);
    sem_init(&mutex, 0, 1);
    pthread_t b;
    pthread_create(&b, nullptr, barber, nullptr);
    pthread_t c[5];
    int ids[5] = {1,2,3,4,5};
    for (int i = 0; i < 5; ++i) {
        pthread_create(&c[i], nullptr, customer, &ids[i]);
        sleep(1);
    }
    for (int i = 0; i < 5; ++i)
        pthread_join(c[i], nullptr);
    // Barber thread runs infinitely for demo
    sem_destroy(&customers);
    sem_destroy(&barbers);
    sem_destroy(&mutex);
    return 0;
}