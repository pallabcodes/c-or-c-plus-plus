#include <pthread.h>
#include <iostream>

pthread_spinlock_t spin;
int counter = 0;

void* inc(void*) {
    for (int i = 0; i < 100000; ++i) {
        pthread_spin_lock(&spin);
        ++counter;
        pthread_spin_unlock(&spin);
    }
    return nullptr;
}

int main() {
    pthread_spin_init(&spin, PTHREAD_PROCESS_PRIVATE);
    pthread_t t1, t2;
    pthread_create(&t1, nullptr, inc, nullptr);
    pthread_create(&t2, nullptr, inc, nullptr);
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    std::cout << "Counter: " << counter << std::endl;
    pthread_spin_destroy(&spin);
    return 0;
}