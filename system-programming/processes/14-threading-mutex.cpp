#include <iostream>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t lock;
int shared_counter = 0;

void* thread_func(void* arg) {
    for (int i = 0; i < 100000; ++i) {
        pthread_mutex_lock(&lock);
        ++shared_counter;
        pthread_mutex_unlock(&lock);
    }
    return nullptr;
}

int main() {
    pthread_mutex_init(&lock, nullptr);
    pthread_t t1, t2;
    pthread_create(&t1, nullptr, thread_func, nullptr);
    pthread_create(&t2, nullptr, thread_func, nullptr);

    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);

    std::cout << "Final shared_counter: " << shared_counter << std::endl;
    pthread_mutex_destroy(&lock);
    return 0;
}