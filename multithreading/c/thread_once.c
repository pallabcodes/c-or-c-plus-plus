#include <pthread.h>
#include <stdio.h>

// pthread_once variable
pthread_once_t init_done = PTHREAD_ONCE_INIT;

// Function that will be executed exactly once
void init_function(void) {
    printf("Initialization function executed exactly once\n");
}

void* thread_function(void* arg) {
    // Each thread tries to execute init_function, but only one will succeed
    pthread_once(&init_done, init_function);
    printf("Thread %ld finished\n", pthread_self());
    return NULL;
}

int main() {
    pthread_t threads[3];
    
    // Create multiple threads
    for(int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, thread_function, NULL);
    }
    
    // Wait for all threads to finish
    for(int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }
    
    return 0;
}