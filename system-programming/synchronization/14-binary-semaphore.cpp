#include <semaphore.h>
#include <pthread.h>
#include <iostream>
#include <unistd.h>

sem_t sem;

void* notifier(void*) {
    sleep(1);
    std::cout << "Notifier: signaling event\n";
    sem_post(&sem);
    return nullptr;
}

void* waiter(void*) {
    std::cout << "Waiter: waiting for event\n";
    sem_wait(&sem);
    std::cout << "Waiter: event received\n";
    return nullptr;
}

int main() {
    sem_init(&sem, 0, 0); // Binary semaphore
    pthread_t t1, t2;
    pthread_create(&t1, nullptr, waiter, nullptr);
    pthread_create(&t2, nullptr, notifier, nullptr);
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    sem_destroy(&sem);
    return 0;
}