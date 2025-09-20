#include <pthread.h>
#include <iostream>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int counter = 0;

void* worker(void*) {
    for (int i = 0; i < 100000; ++i) {
        pthread_mutex_lock(&mutex);
        ++counter;
        pthread_mutex_unlock(&mutex);
    }
    return nullptr;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, nullptr, worker, nullptr);
    pthread_create(&t2, nullptr, worker, nullptr);
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    std::cout << "Counter: " << counter << std::endl;
    pthread_mutex_destroy(&mutex);
    return 0;
}