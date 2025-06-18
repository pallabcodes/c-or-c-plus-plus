#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Structure to pass multiple arguments
struct thread_args {
    int id;
    char* message;
    double value;
};

// Thread function accepting a single argument
void* simple_thread(void* arg) {
    int* value = (int*)arg;
    printf("Thread received value: %d\n", *value);
    return NULL;
}

// Thread function accepting multiple arguments via structure
void* complex_thread(void* arg) {
    struct thread_args* args = (struct thread_args*)arg;
    printf("Thread ID: %d\n", args->id);
    printf("Message: %s\n", args->message);
    printf("Value: %.2f\n", args->value);
    return NULL;
}

int main() {
    pthread_t thread1, thread2;
    
    // Example 1: Passing a single argument
    int number = 42;
    pthread_create(&thread1, NULL, simple_thread, (void*)&number);
    
    // Example 2: Passing multiple arguments using structure
    struct thread_args args = {
        .id = 1,
        .message = "Hello from thread!",
        .value = 3.14
    };
    pthread_create(&thread2, NULL, complex_thread, (void*)&args);
    
    // Wait for threads to complete
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    return 0;
}