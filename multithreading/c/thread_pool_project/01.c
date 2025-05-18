#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Number of worker threads in the pool
#define THREADS 8

// Max number of tasks in the queue
#define QUEUE_SIZE 100

// below would look like this in TS

// type Task = {
//   fn: (arg: any) => void;  // function to run
//   arg: any;                // argument passed to the function
// };

// Task structure: stores a function pointer and its argument
typedef struct {
    void (*fn)(void* arg);  // task_t.fn is a variable that stores a pointer to a function — and that function can accept any kind of data (via void*) and returns nothing (void).
    void* arg;              // Argument to pass to the function
} task_t;

// Thread pool structure
typedef struct {
    pthread_mutex_t lock;               // Lock to protect shared data
    pthread_cond_t notify;              // Condition variable to signal threads
    pthread_t threads[THREADS];         // Array of worker threads
    task_t task_queue[QUEUE_SIZE];      // Circular task queue
    int queued;                         // Number of tasks currently in the queue
    int queue_front;                    // Index of the first task
    int queue_back;                     // Index of the last task
    int stop;                           // Flag to stop all threads
} threadpool_t;

// Function that each thread runs — waits for tasks and runs them
void* thread_function(void* arg) {
    threadpool_t* pool = (threadpool_t*)arg;

    while (1) {
        pthread_mutex_lock(&pool->lock);

        // Wait if no tasks and not shutting down
        while (pool->queued == 0 && !pool->stop) {
            pthread_cond_wait(&pool->notify, &pool->lock);
        }

        // If shutting down, exit thread
        if (pool->stop) {
            pthread_mutex_unlock(&pool->lock);
            break;
        }

        // Get the next task from the front of the queue
        task_t task = pool->task_queue[pool->queue_front];
        pool->queue_front = (pool->queue_front + 1) % QUEUE_SIZE;
        pool->queued--;  // Decrease number of queued tasks

        pthread_mutex_unlock(&pool->lock);

        // Run the task outside the lock
        task.fn(task.arg);
    }

    return NULL;
}

// Initializes the thread pool
void threadpool_init(threadpool_t* pool) {
    pool->queued = 0;
    pool->queue_front = 0;
    pool->queue_back = 0;
    pool->stop = 0;

    pthread_mutex_init(&pool->lock, NULL);
    pthread_cond_init(&pool->notify, NULL);

    // Create worker threads
    for (int i = 0; i < THREADS; i++) {
        pthread_create(&pool->threads[i], NULL, thread_function, pool);
    }
}

// Adds a task to the thread pool's task queue
void threadpool_add(threadpool_t* pool, void (*fn)(void*), void* arg) {
    pthread_mutex_lock(&pool->lock);

    int next = (pool->queue_back + 1) % QUEUE_SIZE;

    // If the queue is full, reject the task
    if (next == pool->queue_front) {
        printf("Task queue is full!\n");
        pthread_mutex_unlock(&pool->lock);
        return;
    }

    // Add the task at the back of the queue
    pool->task_queue[pool->queue_back].fn = fn;
    pool->task_queue[pool->queue_back].arg = arg;
    pool->queue_back = next;
    pool->queued++;  // Increase the task count

    // Signal one waiting thread to wake up
    pthread_cond_signal(&pool->notify);
    pthread_mutex_unlock(&pool->lock);
}

// Shuts down the thread pool and cleans up
void threadpool_destroy(threadpool_t* pool) {
    pthread_mutex_lock(&pool->lock);
    pool->stop = 1;  // Set stop flag
    pthread_cond_broadcast(&pool->notify);  // Wake up all threads
    pthread_mutex_unlock(&pool->lock);

    // Join all threads
    for (int i = 0; i < THREADS; i++) {
        pthread_join(pool->threads[i], NULL);
    }

    // Clean up resources
    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->notify);
}

// A simple example task function
void print_task(void* arg) {
    int num = *(int*)arg;
    printf("Thread %ld is processing task: %d\n", pthread_self(), num);
    sleep(1);  // Simulate work
}

// Main function to demonstrate the thread pool
int main() {
    threadpool_t pool;
    threadpool_init(&pool);  // Set up the thread pool

    int args[20];
    for (int i = 0; i < 20; i++) {
        args[i] = i;
        threadpool_add(&pool, print_task, &args[i]);  // Add tasks
    }

    sleep(5);  // Let threads finish work
    threadpool_destroy(&pool);  // Clean up
    return 0;
}
