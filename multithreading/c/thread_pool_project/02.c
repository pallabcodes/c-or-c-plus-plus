// How thread dispatch function (a.k.a. worker thread) should work in a thread pool ?

// This function is run by each thread in the thread pool
void* thread_function(void* arg) {
    // 1. Cast the argument to threadpool_t to access the thread pool structure
    threadpool_t* pool = (threadpool_t*)arg;

    // 2. Enter infinite loop: threads wait for and process tasks
    while (1) {
        pthread_mutex_lock(&pool->lock); // 3. Lock the mutex to protect shared data

        // 4. Wait for tasks if queue is empty and pool is not stopping
        while (pool->queued == 0 && !pool->stop) {
            // Wait on condition variable until a new task is added or stop is signaled
            pthread_cond_wait(&pool->notify, &pool->lock);
        }

        // 5. Check if the pool is stopping — if yes, exit the thread
        if (pool->stop) {
            pthread_mutex_unlock(&pool->lock); // Release lock before exiting
            pthread_exit(NULL);                // Gracefully exit thread
        }

        // 6. Retrieve the next task from the front of the queue
        task_t task = pool->task_queue[pool->queue_front];  // Get task
        pool->queue_front = (pool->queue_front + 1) % QUEUE_SIZE;  // Move front forward (circular buffer)
        pool->queued--;  // Decrease number of queued tasks

        pthread_mutex_unlock(&pool->lock); // 7. Unlock the mutex so other threads can access the queue

        // 8. Execute the task function, passing the argument
        task.fn(task.arg);
    }

    return NULL;
}


