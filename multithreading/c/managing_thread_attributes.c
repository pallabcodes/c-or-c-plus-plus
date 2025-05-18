#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>

void* thread_function(void* arg) {
    printf("Thread running on CPU %d\n", sched_getcpu());
    return NULL;
}

int main() {
    pthread_t thread;
    pthread_attr_t attr;

    cpu_set_t cpus;
    CPU_ZERO(&cpus);        // Clear the set
    CPU_SET(0, &cpus);      // Add CPU 0 to the set

    pthread_attr_init(&attr);  // Initialize thread attributes

    // Set CPU affinity: bind thread to CPU 0
    pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);

    // Create thread using the customized attributes
    pthread_create(&thread, &attr, thread_function, NULL);

    // Wait for thread to finish
    pthread_join(thread, NULL);

    // Clean up attribute object
    pthread_attr_destroy(&attr);

    return 0;
}
