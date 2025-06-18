#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sched.h>

void* thread_function(void* arg) {
    int cpu_id = *(int*)arg;
    cpu_set_t cpuset;
    
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);
    
    // Set thread affinity
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    
    // Verify the affinity setting
    CPU_ZERO(&cpuset);
    pthread_getaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    
    printf("Thread running on CPU %d\n", sched_getcpu());
    
    while(1) {
        // Keep thread running
        sleep(1);
    }
    
    return NULL;
}

int main() {
    pthread_t threads[2];
    int cpu_ids[2] = {0, 1};  // Assign to CPU 0 and 1
    
    // Create threads with specific CPU affinity
    for(int i = 0; i < 2; i++) {
        pthread_create(&threads[i], NULL, thread_function, &cpu_ids[i]);
    }
    
    sleep(5);  // Let threads run for 5 seconds
    
    // Clean up
    for(int i = 0; i < 2; i++) {
        pthread_cancel(threads[i]);
        pthread_join(threads[i], NULL);
    }
    
    return 0;
}