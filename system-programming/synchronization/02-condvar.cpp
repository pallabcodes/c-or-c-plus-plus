#include <pthread.h>
#include <queue>
#include <iostream>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
std::queue<int> q;
bool done = false;

void* producer(void*) {
    for (int i = 0; i < 10; ++i) {
        pthread_mutex_lock(&mutex);
        q.push(i);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
    pthread_mutex_lock(&mutex);
    done = true;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return nullptr;
}

void* consumer(void*) {
    while (true) {
        pthread_mutex_lock(&mutex);
        while (q.empty() && !done)
            pthread_cond_wait(&cond, &mutex);
        if (!q.empty()) {
            int val = q.front(); q.pop();
            std::cout << "Consumed: " << val << std::endl;
        } else if (done) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
    }
    return nullptr;
}

int main() {
    pthread_t prod, cons;
    pthread_create(&prod, nullptr, producer, nullptr);
    pthread_create(&cons, nullptr, consumer, nullptr);
    pthread_join(prod, nullptr);
    pthread_join(cons, nullptr);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    return 0;
}