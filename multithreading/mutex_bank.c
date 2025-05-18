#include <stdio.h>
#include <pthread.h>
#include <unistd.h>  // for sleep

#define THREAD_COUNT 10
#define TRANSACTIONS_PER_THREAD 1000

int account_balance = 0;               // Shared bank account balance
pthread_mutex_t balance_lock;          // Mutex to protect the balance

// Thread function to perform deposits and withdrawals
void* transaction_thread(void* arg) {
    int thread_id = *(int*)arg;

    for (int i = 0; i < TRANSACTIONS_PER_THREAD; i++) {
        // Lock before modifying the balance
        pthread_mutex_lock(&balance_lock);

        // Simulate deposit and withdrawal
        account_balance += 100;       // deposit
        account_balance -= 100;       // withdraw

        // Unlock after done modifying
        pthread_mutex_unlock(&balance_lock);
    }

    return NULL;
}

int main() {
    pthread_t threads[THREAD_COUNT];
    int thread_ids[THREAD_COUNT];

    // Initialize the mutex
    pthread_mutex_init(&balance_lock, NULL);

    // Create threads
    for (int i = 0; i < THREAD_COUNT; i++) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, transaction_thread, &thread_ids[i])) {
            perror("pthread_create");
            return -1;
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    // Destroy the mutex
    pthread_mutex_destroy(&balance_lock);

    // Final balance should be 0 (every deposit had a matching withdrawal)
    printf("Final account balance: %d\n", account_balance);

    return 0;
}
