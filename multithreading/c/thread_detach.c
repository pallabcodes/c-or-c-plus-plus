#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void* detached_thread(void* arg) {
    printf("Detached thread starting...\n");
    sleep(2);
    printf("Detached thread finishing...\n");
    return NULL;
}

int main() {
    pthread_t thread;
    pthread_attr_t attr;
    
    // Initialize thread attributes
    pthread_attr_init(&attr);
    
    // Set the detached state
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    
    // Create detached thread
    pthread_create(&thread, &attr, detached_thread, NULL);
    
    // Clean up attribute
    pthread_attr_destroy(&attr);
    
    printf("Main thread continuing...\n");
    sleep(3); // Wait longer than the detached thread
    printf("Main thread exiting...\n");
    
    return 0;
}