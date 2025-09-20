#include <pthread.h>
#include <iostream>

// Demonstrates thread synchronization using pthread_barrier_t

pthread_barrier_t barrier;

// Worker function: waits at the barrier, then proceeds
void* worker(void* arg) {
    int id = *(int*)arg;
    std::cout << "Thread " << id << " waiting at barrier\n";
    // All threads wait here until the barrier count is reached
    pthread_barrier_wait(&barrier);
    std::cout << "Thread " << id << " passed barrier\n";
    return nullptr;
}

int main() {
    // Initialize barrier for 3 threads
    pthread_barrier_init(&barrier, nullptr, 3);

    pthread_t t[3];
    int ids[3] = {1, 2, 3};

    // Create 3 threads
    for (int i = 0; i < 3; ++i)
        pthread_create(&t[i], nullptr, worker, &ids[i]);

    // Wait for all threads to finish
    for (int i = 0; i < 3; ++i)
        pthread_join(t[i], nullptr);

    // Destroy the barrier
    pthread_barrier_destroy(&barrier);
    return 0;
}