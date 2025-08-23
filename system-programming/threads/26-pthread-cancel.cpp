#include <pthread.h>
#include <iostream>
#include <unistd.h>

void* worker(void*) {
    std::cout << "Worker started, waiting for cancellation...\n";
    while (true) {
        pthread_testcancel(); // Cancellation point
        sleep(1);
    }
    return nullptr;
}

int main() {
    pthread_t t;
    pthread_create(&t, nullptr, worker, nullptr);
    sleep(2); // Let the thread run for a bit
    pthread_cancel(t); // Request cancellation
    pthread_join(t, nullptr);
    std::cout << "Thread cancelled and joined\n";
    return 0;
}