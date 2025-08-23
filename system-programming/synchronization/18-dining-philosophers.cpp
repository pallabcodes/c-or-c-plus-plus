#include <pthread.h>
#include <iostream>
#include <unistd.h>

const int N = 5;
pthread_mutex_t forks[N];

void* philosopher(void* arg) {
    int id = *(int*)arg;
    int left = id;
    int right = (id + 1) % N;
    for (int i = 0; i < 3; ++i) {
        std::cout << "Philosopher " << id << " thinking\n";
        sleep(1);
        pthread_mutex_lock(&forks[left]);
        pthread_mutex_lock(&forks[right]);
        std::cout << "Philosopher " << id << " eating\n";
        sleep(1);
        pthread_mutex_unlock(&forks[right]);
        pthread_mutex_unlock(&forks[left]);
    }
    return nullptr;
}

int main() {
    pthread_t phils[N];
    int ids[N];
    for (int i = 0; i < N; ++i) {
        pthread_mutex_init(&forks[i], nullptr);
        ids[i] = i;
    }
    for (int i = 0; i < N; ++i)
        pthread_create(&phils[i], nullptr, philosopher, &ids[i]);
    for (int i = 0; i < N; ++i)
        pthread_join(phils[i], nullptr);
    for (int i = 0; i < N; ++i)
        pthread_mutex_destroy(&forks[i]);
    return 0;
}