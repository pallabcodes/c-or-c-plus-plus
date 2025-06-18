#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

// Resources
pthread_mutex_t resourceA = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t resourceB = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t resourceC = PTHREAD_MUTEX_INITIALIZER;

// Deadlock Scenario 1: Circular Wait
void* thread1_circular(void* arg) {
    while(1) {
        printf("Thread 1 trying to acquire Resource A\n");
        pthread_mutex_lock(&resourceA);
        printf("Thread 1 acquired Resource A\n");
        sleep(1); // Simulate work
        
        printf("Thread 1 trying to acquire Resource B\n");
        pthread_mutex_lock(&resourceB);
        printf("Thread 1 acquired Resource B\n");
        
        // Critical section
        printf("Thread 1 using both resources\n");
        
        pthread_mutex_unlock(&resourceB);
        pthread_mutex_unlock(&resourceA);
    }
    return NULL;
}

void* thread2_circular(void* arg) {
    while(1) {
        printf("Thread 2 trying to acquire Resource B\n");
        pthread_mutex_lock(&resourceB);
        printf("Thread 2 acquired Resource B\n");
        sleep(1); // Simulate work
        
        printf("Thread 2 trying to acquire Resource A\n");
        pthread_mutex_lock(&resourceA);
        printf("Thread 2 acquired Resource A\n");
        
        // Critical section
        printf("Thread 2 using both resources\n");
        
        pthread_mutex_unlock(&resourceA);
        pthread_mutex_unlock(&resourceB);
    }
    return NULL;
}

// Deadlock Prevention: Resource Ordering
void* thread1_ordered(void* arg) {
    while(1) {
        // Always acquire resources in same order
        pthread_mutex_lock(&resourceA);
        pthread_mutex_lock(&resourceB);
        
        printf("Thread 1 using resources (ordered)\n");
        
        pthread_mutex_unlock(&resourceB);
        pthread_mutex_unlock(&resourceA);
        sleep(1);
    }
    return NULL;
}

void* thread2_ordered(void* arg) {
    while(1) {
        // Maintain same order as thread1
        pthread_mutex_lock(&resourceA);
        pthread_mutex_lock(&resourceB);
        
        printf("Thread 2 using resources (ordered)\n");
        
        pthread_mutex_unlock(&resourceB);
        pthread_mutex_unlock(&resourceA);
        sleep(1);
    }
    return NULL;
}

// Deadlock Prevention: Timeout
void* thread_timeout(void* arg) {
    struct timespec ts;
    int id = *(int*)arg;
    
    while(1) {
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 2; // 2 second timeout
        
        if (pthread_mutex_timedlock(&resourceA, &ts) == 0) {
            printf("Thread %d acquired Resource A\n", id);
            sleep(1);
            
            if (pthread_mutex_timedlock(&resourceB, &ts) == 0) {
                printf("Thread %d acquired Resource B\n", id);
                printf("Thread %d using both resources\n", id);
                pthread_mutex_unlock(&resourceB);
            } else {
                printf("Thread %d timeout on Resource B\n", id);
            }
            
            pthread_mutex_unlock(&resourceA);
        } else {
            printf("Thread %d timeout on Resource A\n", id);
        }
        
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    int id1 = 1, id2 = 2;
    int choice;
    
    printf("Select deadlock demonstration:\n");
    printf("1. Circular Wait (classic deadlock)\n");
    printf("2. Prevention by Ordering\n");
    printf("3. Prevention by Timeout\n");
    scanf("%d", &choice);
    
    switch(choice) {
        case 1:
            // Demonstrate circular wait deadlock
            pthread_create(&t1, NULL, thread1_circular, NULL);
            pthread_create(&t2, NULL, thread2_circular, NULL);
            break;
            
        case 2:
            // Demonstrate prevention by ordering
            pthread_create(&t1, NULL, thread1_ordered, NULL);
            pthread_create(&t2, NULL, thread2_ordered, NULL);
            break;
            
        case 3:
            // Demonstrate prevention by timeout
            pthread_create(&t1, NULL, thread_timeout, &id1);
            pthread_create(&t2, NULL, thread_timeout, &id2);
            break;
    }
    
    sleep(20); // Let the demonstration run for 20 seconds
    
    // Cleanup
    pthread_mutex_destroy(&resourceA);
    pthread_mutex_destroy(&resourceB);
    pthread_mutex_destroy(&resourceC);
    
    return 0;
}