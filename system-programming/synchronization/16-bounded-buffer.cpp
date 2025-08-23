#include <pthread.h>
#include <semaphore.h>
#include <queue>
#include <iostream>

const int BUFFER_SIZE = 5;
std::queue<int> buffer;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t empty, full;

void* producer(void*) {
    for (int i = 0; i < 10; ++i) {
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        buffer.push(i);
        std::cout << "Produced: " << i << std::endl;
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
    return nullptr;
}

void* consumer(void*) {
    for (int i = 0; i < 10; ++i) {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        int val = buffer.front(); buffer.pop();
        std::cout << "Consumed: " << val << std::endl;
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
    return nullptr;
}

int main() {
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_t prod, cons;
    pthread_create(&prod, nullptr, producer, nullptr);
    pthread_create(&cons, nullptr, consumer, nullptr);
    pthread_join(prod, nullptr);
    pthread_join(cons, nullptr);
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);
    return 0;
}