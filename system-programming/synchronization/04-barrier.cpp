#include <pthread.h>
#include <iostream>

pthread_barrier_t barrier;

void* worker(void* arg) {
    int id = *(int*)arg;
    std::cout << "Thread " << id << " waiting at barrier\n";
    pthread_barrier_wait(&barrier);
    std::cout << "Thread " << id << " passed barrier\n";
    return nullptr;
}

int main() {
    pthread_barrier_init(&barrier, nullptr, 3);
    pthread_t t[3];
    int ids[3] = {1,2,3};
    for (int i = 0; i < 3; ++i)
        pthread_create(&t[i], nullptr, worker, &ids[i]);
    for (int i = 0; i < 3; ++i)
        pthread_join(t[i], nullptr);
    pthread_barrier_destroy(&barrier);
    return 0;
}