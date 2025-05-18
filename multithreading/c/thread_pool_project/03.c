// Function to gracefully shut down the thread pool
void threadpool_destroy(threadpool_t* pool) {
    // 1. Lock the mutex to safely update the pool state
    pthread_mutex_lock(&pool->lock);

    // 2. Set the stop flag to indicate the pool is shutting down
    pool->stop = 1;

    // 3. Wake up all threads, including those waiting on tasks
    pthread_cond_broadcast(&pool->notify);

    // 4. Unlock the mutex to allow worker threads to proceed with shutdown
    pthread_mutex_unlock(&pool->lock);

    // 5. Wait for all worker threads to finish
    for (int i = 0; i < THREADS; i++) {
        pthread_join(pool->threads[i], NULL);
    }

    // 6. Clean up synchronization primitives
    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->notify);
}


