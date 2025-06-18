#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Different types of mutexes
pthread_mutex_t basic_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t recursive_mutex;
pthread_mutexattr_t recursive_attr;

// Shared resources
int shared_balance = 1000;
int transaction_count = 0;

// Basic mutex example
void* basic_mutex_thread(void* arg) {
    int thread_id = *(int*)arg;
    
    pthread_mutex_lock(&basic_mutex);
    printf("Thread %d entered critical section\n", thread_id);
    shared_balance += 100;
    printf("Thread %d updated balance to: %d\n", thread_id, shared_balance);
    pthread_mutex_unlock(&basic_mutex);
    
    return NULL;
}

// Recursive mutex example
void recursive_function(int depth) {
    pthread_mutex_lock(&recursive_mutex);
    printf("Entering depth %d\n", depth);
    
    if (depth > 0) {
        recursive_function(depth - 1);
    }
    
    printf("Exiting depth %d\n", depth);
    pthread_mutex_unlock(&recursive_mutex);
}

void* recursive_mutex_thread(void* arg) {
    recursive_function(3);
    return NULL;
}

// Timed mutex example
void* timed_mutex_thread(void* arg) {
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 2; // 2 second timeout
    
    if (pthread_mutex_timedlock(&basic_mutex, &timeout) == 0) {
        printf("Thread acquired mutex within timeout\n");
        sleep(1);
        pthread_mutex_unlock(&basic_mutex);
    } else {
        printf("Thread couldn't acquire mutex within timeout\n");
    }
    
    return NULL;
}

// Try-lock example
void* trylock_thread(void* arg) {
    while(1) {
        if (pthread_mutex_trylock(&basic_mutex) == 0) {
            printf("Thread acquired mutex with trylock\n");
            sleep(1);
            pthread_mutex_unlock(&basic_mutex);
            break;
        } else {
            printf("Mutex busy, trying again...\n");
            sleep(1);
        }
    }
    return NULL;
}

int main() {
    pthread_t threads[4];
    int thread_ids[4] = {1, 2, 3, 4};
    
    // Initialize recursive mutex
    pthread_mutexattr_init(&recursive_attr);
    pthread_mutexattr_settype(&recursive_attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&recursive_mutex, &recursive_attr);
    
    printf("\n=== Basic Mutex Example ===\n");
    pthread_create(&threads[0], NULL, basic_mutex_thread, &thread_ids[0]);
    pthread_create(&threads[1], NULL, basic_mutex_thread, &thread_ids[1]);
    
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    
    printf("\n=== Recursive Mutex Example ===\n");
    pthread_create(&threads[2], NULL, recursive_mutex_thread, NULL);
    pthread_join(threads[2], NULL);
    
    printf("\n=== TryLock Example ===\n");
    pthread_create(&threads[0], NULL, trylock_thread, NULL);
    pthread_create(&threads[1], NULL, trylock_thread, NULL);
    
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    
    printf("\n=== Timed Mutex Example ===\n");
    pthread_mutex_lock(&basic_mutex); // Lock the mutex
    pthread_create(&threads[3], NULL, timed_mutex_thread, NULL);
    pthread_join(threads[3], NULL);
    pthread_mutex_unlock(&basic_mutex);
    
    // Cleanup
    pthread_mutex_destroy(&basic_mutex);
    pthread_mutex_destroy(&recursive_mutex);
    pthread_mutexattr_destroy(&recursive_attr);
    
    return 0;
}