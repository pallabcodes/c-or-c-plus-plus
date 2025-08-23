#include <pthread.h>
#include <iostream>

pthread_once_t once_control = PTHREAD_ONCE_INIT;

void init_func() {
    std::cout << "Initialized once!\n";
}

void* worker(void*) {
    pthread_once(&once_control, init_func);
    return nullptr;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, nullptr, worker, nullptr);
    pthread_create(&t2, nullptr, worker, nullptr);
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    return 0;
}