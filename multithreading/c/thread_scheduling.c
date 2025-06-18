#include <pthread.h>
#include <stdio.h>
#include <sched.h>
#include <unistd.h>

void* thread_function(void* arg) {
    int policy;
    struct sched_param param;
    
    // Get current thread scheduling parameters
    pthread_getschedparam(pthread_self(), &policy, &param);
    
    printf("Thread priority: %d\n", param.sched_priority);
    printf("Thread policy: %s\n", 
           (policy == SCHED_FIFO)  ? "SCHED_FIFO" :
           (policy == SCHED_RR)    ? "SCHED_RR" :
           (policy == SCHED_OTHER) ? "SCHED_OTHER" :
                                    "UNKNOWN");
    return NULL;
}

int main() {
    pthread_t thread;
    pthread_attr_t attr;
    struct sched_param param;
    
    // Initialize attributes
    pthread_attr_init(&attr);
    
    // Set scheduling policy to SCHED_FIFO
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    
    // Set priority
    param.sched_priority = 50; // Mid-range priority
    pthread_attr_setschedparam(&attr, &param);
    
    // Create thread with custom scheduling
    pthread_create(&thread, &attr, thread_function, NULL);
    
    pthread_join(thread, NULL);
    pthread_attr_destroy(&attr);
    
    return 0;
}