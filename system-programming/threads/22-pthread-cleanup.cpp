#include <pthread.h>
#include <iostream>
#include <unistd.h>

void cleanup(void* arg) {
    std::cout << "Cleanup handler called: " << (char*)arg << std::endl;
}

void* worker(void*) {
    pthread_cleanup_push(cleanup, (void*)"Thread exiting");
    std::cout << "Worker running\n";
    sleep(1);
    pthread_cleanup_pop(1); // Call cleanup handler
    return nullptr;
}

int main() {
    pthread_t t;
    pthread_create(&t, nullptr, worker, nullptr);
    pthread_join(t, nullptr);
    return 0;
}