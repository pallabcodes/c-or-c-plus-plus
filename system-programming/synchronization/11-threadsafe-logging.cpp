#include <pthread.h>
#include <iostream>

pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

void log(const std::string& msg) {
    pthread_mutex_lock(&log_mutex);
    std::cout << msg << std::endl;
    pthread_mutex_unlock(&log_mutex);
}

void* worker(void* arg) {
    int id = *(int*)arg;
    log("Thread " + std::to_string(id) + " logging safely");
    return nullptr;
}

int main() {
    pthread_t t1, t2;
    int id1 = 1, id2 = 2;
    pthread_create(&t1, nullptr, worker, &id1);
    pthread_create(&t2, nullptr, worker, &id2);
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    pthread_mutex_destroy(&log_mutex);
    return 0;
}