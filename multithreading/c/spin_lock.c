#include <pthread.h>
#include <stdio.h>

int counter = 0;
pthread_spinlock_t spinlock;

void* increment_counter(void* arg) {
    for (int i = 0; i < 10000; i++) {
        pthread_spin_lock(&spinlock);     // Lock the counter
        counter++;                        // Safely increment
        pthread_spin_unlock(&spinlock);   // Unlock it
    }
    return NULL;
}

int main() {
    pthread_spin_init(&spinlock, 0);      // Initialize spinlock

    pthread_t t1, t2;

    pthread_create(&t1, NULL, increment_counter, NULL);  // Thread 1
    pthread_create(&t2, NULL, increment_counter, NULL);  // Thread 2

    pthread_join(t1, NULL);   // Wait for thread 1
    pthread_join(t2, NULL);   // Wait for thread 2

    printf("Final counter value: %d\n", counter);  // Should be 20000

    pthread_spin_destroy(&spinlock);  // Clean up
    return 0;
}
