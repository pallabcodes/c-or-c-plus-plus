#include <pthread.h>
#include <iostream>
#include <ctime>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
bool ready = false;

void* worker(void*) {
    pthread_mutex_lock(&mutex);
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 2; // Wait for up to 2 seconds
    while (!ready) {
        int ret = pthread_cond_timedwait(&cond, &mutex, &ts);
        if (ret == ETIMEDOUT) {
            std::cout << "Timed out waiting for condition\n";
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
    return nullptr;
}

int main() {
    pthread_t t;
    pthread_create(&t, nullptr, worker, nullptr);
    pthread_join(t, nullptr);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    return 0;
}