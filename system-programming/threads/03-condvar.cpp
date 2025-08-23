#include <pthread.h>
#include <queue>
#include <iostream>
pthread_mutex_t lock;
pthread_cond_t cond;
std::queue<int> q;
bool done = false;
void* producer(void*) {
    for (int i = 0; i < 10; ++i) {
        pthread_mutex_lock(&lock);
        q.push(i);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&lock);
    }
    pthread_mutex_lock(&lock);
    done = true;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&lock);
    return nullptr;
}
void* consumer(void*) {
    while (true) {
        pthread_mutex_lock(&lock);
        while (q.empty() && !done) pthread_cond_wait(&cond, &lock);
        if (!q.empty()) {
            int val = q.front(); q.pop();
            std::cout << "Consumed: " << val << std::endl;
        } else if (done) {
            pthread_mutex_unlock(&lock);
            break;
        }
        pthread_mutex_unlock(&lock);
    }
    return nullptr;
}
int main() {
    pthread_mutex_init(&lock, nullptr);
    pthread_cond_init(&cond, nullptr);
    pthread_t prod, cons;
    pthread_create(&prod, nullptr, producer, nullptr);
    pthread_create(&cons, nullptr, consumer, nullptr);
    pthread_join(prod, nullptr);
    pthread_join(cons, nullptr);
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);
    return 0;
}