#include <pthread.h>
#include <iostream>
void* thread_func(void*) { std::cout << "Hello from thread!\n"; return nullptr; }
int main() {
    pthread_t t;
    pthread_create(&t, nullptr, thread_func, nullptr);
    pthread_join(t, nullptr);
    return 0;
}