#include <pthread.h>
#include <iostream>
pthread_mutex_t lock;
int counter = 0;
void* inc(void*) {
    for (int i = 0; i < 100000; ++i) {
        pthread_mutex_lock(&lock);
        ++counter;
        pthread_mutex_unlock(&lock);
    }
    return nullptr;
}
int main() {
    pthread_mutex_init(&lock, nullptr);
    pthread_t t1, t2;
    pthread_create(&t1, nullptr, inc, nullptr);
    pthread_create(&t2, nullptr, inc, nullptr);
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    std::cout << "Counter: " << counter << std::endl;
    pthread_mutex_destroy(&lock);
    return 0;
}