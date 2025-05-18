#include <stdio.h>
#include <pthread.h>

#define THREAD_COUNT 10

int shared_counter = 0;               // Shared resource

pthread_mutex_t lock;                 // Mutex to protect the shared resource i.e. shared_counter

// Thread function to increment the shared counter
void* thread_target(void* arg) {
    int thread_num = *(int*)arg;

    // Lock before accessing shared resource
    pthread_mutex_lock(&lock);

    // Critical section: only one thread at a time can be here
    shared_counter++;
    printf("Thread %d incremented counter to %d\n", thread_num, shared_counter);

    // Unlock so other threads can enter
    pthread_mutex_unlock(&lock);

    return NULL;
}

int main() {
    pthread_t threads[THREAD_COUNT];
    int thread_args[THREAD_COUNT];
    int i;

    // Initialize the mutex
    pthread_mutex_init(&lock, NULL);

    // Create threads
    for (i = 0; i < THREAD_COUNT; i++) {
        thread_args[i] = i + 1;
        if (pthread_create(&threads[i], NULL, thread_target, &thread_args[i])) {
            perror("pthread_create");
            return -1;
        }
    }

    // Wait for all threads to finish
    for (i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    // Destroy the mutex
    pthread_mutex_destroy(&lock);

    printf("All threads completed. Final counter value: %d\n", shared_counter);
    return 0;
}
