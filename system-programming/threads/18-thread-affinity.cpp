#define _GNU_SOURCE
#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <sched.h>

void* work(void* arg) {
    int cpu = *(int*)arg;
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    std::cout << "Thread running on CPU " << cpu << std::endl;
    sleep(1);
    return nullptr;
}

int main() {
    int cpu0 = 0, cpu1 = 1;
    pthread_t t1, t2;
    pthread_create(&t1, nullptr, work, &cpu0);
    pthread_create(&t2, nullptr, work, &cpu1);
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    return 0;
}