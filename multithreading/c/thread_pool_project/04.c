#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// ==== (Paste the threadpool_t, task_t structs, and all related functions here) ====
// Including: threadpool_init, thread_function, threadpool_add, threadpool_destroy

// ---- Example Task Function ----
void example_task(void* arg) {
    int num = *(int*)arg;
    printf("Thread %ld is processing task #%d\n", pthread_self(), num);
    sleep(1); // Simulate task work
}

// ---- Main Program to Test the Thread Pool ----
int main() {
    threadpool_t pool;
    threadpool_init(&pool);  // 1. Initialize the thread pool

    int args[15];
    for (int i = 0; i < 15; i++) {
        args[i] = i;
        threadpool_add(&pool, example_task, &args[i]);  // 2. Add 15 tasks
    }

    sleep(5); // 3. Allow time for tasks to be processed

    threadpool_destroy(&pool);  // 4. Gracefully shut down the thread pool

    printf("All tasks completed. Thread pool shut down.\n");
    return 0;
}
