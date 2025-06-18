#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define NUM_THREADS 3
pthread_barrier_t barrier;

void cleanup_handler(void* arg) {
    printf("Cleanup handler called\n");
}

void* thread_function(void* arg) {
    int id = *(int*)arg;
    printf("Thread %d starting...\n", id);
    sleep(id); // Simulate different work times
    
    printf("Thread %d reached barrier\n", id);
    pthread_barrier_wait(&barrier);
    printf("Thread %d passed barrier\n", id);
    
    pthread_cleanup_push(cleanup_handler, NULL);
    
    while(1) {
        printf("Thread %d working...\n", id);
        sleep(1);
        pthread_testcancel(); // Cancellation point
    }
    
    pthread_cleanup_pop(1);
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    
    pthread_barrier_init(&barrier, NULL, NUM_THREADS);
    
    for(int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i + 1;
        pthread_create(&threads[i], NULL, thread_function, &thread_ids[i]);
    }
    
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_cancel(threads[i]);
    }
    
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_barrier_destroy(&barrier);
    printf("Threads cancelled and joined\n");
    return 0;
}