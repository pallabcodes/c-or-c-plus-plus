#include <stdatomic.h>
#include <pthread.h>
#include <stdio.h>

atomic_int shared_counter = 0;
atomic_flag lock = ATOMIC_FLAG_INIT;

void* increment_thread(void* arg) {
    for(int i = 0; i < 1000000; i++) {
        // Atomic increment
        atomic_fetch_add(&shared_counter, 1);
        
        // Memory barrier example
        atomic_thread_fence(memory_order_release);
    }
    return NULL;
}

void* spinlock_thread(void* arg) {
    while(atomic_flag_test_and_set(&lock)) {
        // Spin wait
    }
    // Critical section
    printf("Thread %ld in critical section\n", pthread_self());
    atomic_flag_clear(&lock);
    return NULL;
}

int main() {
    pthread_t threads[2];
    
    // Test atomic operations
    pthread_create(&threads[0], NULL, increment_thread, NULL);
    pthread_create(&threads[1], NULL, increment_thread, NULL);
    
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    
    printf("Final counter value: %d\n", atomic_load(&shared_counter));
    
    return 0;
}