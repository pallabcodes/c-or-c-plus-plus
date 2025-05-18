#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_key_t key;  // Key for thread-specific data

// Cleanup function called when a thread exits
void cleanup(void *ptr) {
    free(ptr);  // Free the memory
    printf("Cleaned up thread-specific data.\n");
}

// Thread function
void* thread_function(void *arg) {
    int *my_data = malloc(sizeof(int));  // Allocate memory for thread-specific data
    *my_data = (int)(long)arg;           // Store the value passed to the thread

    pthread_setspecific(key, my_data);   // Associate data with the key for this thread

    // Retrieve the data back (just to show it works)
    int *retrieved = (int*)pthread_getspecific(key);
    printf("Thread %ld has data %d\n", pthread_self(), *retrieved);

    sleep(1);  // Simulate some work
    return NULL;
}

int main() {
    pthread_t tid1, tid2;

    // Create a key with a cleanup function that runs when a thread ends
    pthread_key_create(&key, cleanup);

    // Start two threads, passing different data to each
    pthread_create(&tid1, NULL, thread_function, (void*)1);
    pthread_create(&tid2, NULL, thread_function, (void*)2);

    // Wait for both threads to finish
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    // Delete the key after all threads are done
    pthread_key_delete(key);

    return 0;
}
