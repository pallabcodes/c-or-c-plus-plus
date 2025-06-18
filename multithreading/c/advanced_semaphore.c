#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFER_SIZE 5
#define PRODUCERS 2
#define CONSUMERS 3

// Circular buffer for producer-consumer
int buffer[BUFFER_SIZE];
int in = 0, out = 0;

// Semaphores
sem_t empty;      // Counts empty buffer slots
sem_t full;       // Counts full buffer slots
sem_t mutex;      // Binary semaphore for mutual exclusion

void* producer(void* arg) {
    int producer_id = *(int*)arg;
    
    while(1) {
        int item = rand() % 100;  // Generate random item
        
        sem_wait(&empty);     // Wait for empty slot
        sem_wait(&mutex);     // Enter critical section
        
        // Add item to buffer
        buffer[in] = item;
        printf("Producer %d: Inserted %d at position %d\n", producer_id, item, in);
        in = (in + 1) % BUFFER_SIZE;
        
        sem_post(&mutex);     // Exit critical section
        sem_post(&full);      // Signal one full slot
        
        sleep(rand() % 2);    // Random sleep
    }
    return NULL;
}

void* consumer(void* arg) {
    int consumer_id = *(int*)arg;
    
    while(1) {
        sem_wait(&full);      // Wait for full slot
        sem_wait(&mutex);     // Enter critical section
        
        // Remove item from buffer
        int item = buffer[out];
        printf("Consumer %d: Removed %d from position %d\n", consumer_id, item, out);
        out = (out + 1) % BUFFER_SIZE;
        
        sem_post(&mutex);     // Exit critical section
        sem_post(&empty);     // Signal one empty slot
        
        sleep(rand() % 3);    // Random sleep
    }
    return NULL;
}

int main() {
    // Initialize semaphores
    sem_init(&empty, 0, BUFFER_SIZE);  // Initially all slots are empty
    sem_init(&full, 0, 0);            // Initially no slots are full
    sem_init(&mutex, 0, 1);           // Binary semaphore for mutual exclusion
    
    pthread_t producers[PRODUCERS];
    pthread_t consumers[CONSUMERS];
    int producer_ids[PRODUCERS];
    int consumer_ids[CONSUMERS];
    
    // Create producer threads
    for(int i = 0; i < PRODUCERS; i++) {
        producer_ids[i] = i + 1;
        pthread_create(&producers[i], NULL, producer, &producer_ids[i]);
    }
    
    // Create consumer threads
    for(int i = 0; i < CONSUMERS; i++) {
        consumer_ids[i] = i + 1;
        pthread_create(&consumers[i], NULL, consumer, &consumer_ids[i]);
    }
    
    // Let it run for some time
    sleep(20);
    
    // Cleanup semaphores
    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);
    
    return 0;
}