#include <pthread.h>
#include <iostream>

pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;

void* thread1(void*) {
    pthread_mutex_lock(&m1);
    sleep(1);
    pthread_mutex_lock(&m2);
    std::cout << "Thread 1 acquired both locks\n";
    pthread_mutex_unlock(&m2);
    pthread_mutex_unlock(&m1);
    return nullptr;
}

void* thread2(void*) {
    pthread_mutex_lock(&m2);
    sleep(1);
    pthread_mutex_lock(&m1);
    std::cout << "Thread 2 acquired both locks\n";
    pthread_mutex_unlock(&m1);
    pthread_mutex_unlock(&m2);
    return nullptr;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, nullptr, thread1, nullptr);
    pthread_create(&t2, nullptr, thread2, nullptr);
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    std::cout << "If you see this, no deadlock occurred (but usually there will be!)\n";
    return 0;
}