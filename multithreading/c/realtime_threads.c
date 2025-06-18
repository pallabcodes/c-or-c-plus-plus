#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>

void* realtime_thread(void* arg) {
    struct timespec ts;
    int policy;
    struct sched_param param;

    // Get and print current thread policy and priority
    pthread_getschedparam(pthread_self(), &policy, &param);
    printf("Real-time thread priority: %d\n", param.sched_priority);

    // Periodic real-time task
    while(1) {
        clock_gettime(CLOCK_MONOTONIC, &ts);
        ts.tv_sec += 1; // 1 second period
        
        // Do real-time work here
        printf("Real-time task executing\n");
        
        // Sleep until next period
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, NULL);
    }
    return NULL;
}

int main() {
    pthread_t thread;
    pthread_attr_t attr;
    struct sched_param param;
    
    // Initialize thread attributes
    pthread_attr_init(&attr);
    
    // Set scheduling policy to SCHED_FIFO (real-time)
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    
    // Set high priority
    param.sched_priority = 80;
    pthread_attr_setschedparam(&attr, &param);
    
    // Create real-time thread
    pthread_create(&thread, &attr, realtime_thread, NULL);
    
    sleep(5);
    return 0;
}