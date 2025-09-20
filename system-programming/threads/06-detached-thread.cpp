#include <pthread.h>
#include <iostream>
void* work(void*) {
    std::cout << "Detached thread running\n";
    pthread_exit(nullptr);
}
int main() {
    pthread_t t;
    pthread_create(&t, nullptr, work, nullptr);
    pthread_detach(t); // Thread cleans up after itself
    sleep(1); // Give thread time to run
    std::cout << "Main thread exiting\n";
    return 0;
}