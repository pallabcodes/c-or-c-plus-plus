#include <pthread.h>
#include <iostream>
#include <sched.h>

void* worker(void*) {
    int policy;
    sched_param param;
    pthread_getschedparam(pthread_self(), &policy, &param);
    std::cout << "Thread scheduling policy: " << (policy == SCHED_FIFO ? "FIFO" : policy == SCHED_RR ? "RR" : "OTHER")
              << ", priority: " << param.sched_priority << std::endl;
    return nullptr;
}

int main() {
    pthread_t t;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    sched_param param;
    param.sched_priority = 10;
    pthread_attr_setschedparam(&attr, &param);
    pthread_create(&t, &attr, worker, nullptr);
    pthread_join(t, nullptr);
    pthread_attr_destroy(&attr);
    return 0;
}