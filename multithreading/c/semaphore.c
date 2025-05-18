#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

sem_t semaphore;           // Semaphore used to signal subscriber
char message[100];         // Shared message between publisher and subscriber

void* publisher(void* arg) {
    sprintf(message, "Data published");   // Publisher sets the message
    sem_post(&semaphore);                 // Signal the subscriber: "Hey, message is ready!"
    return NULL;
}

void* subscriber(void* arg) {
    sem_wait(&semaphore);                 // Wait for signal from publisher
    printf("Received message: %s\n", message);  // Read and print the message
    return NULL;
}

int main() {
    pthread_t pub, sub;

    sem_init(&semaphore, 0, 0);  // Initialize semaphore with 0 (locked)

    pthread_create(&pub, NULL, publisher, NULL);     // Start publisher thread
    pthread_create(&sub, NULL, subscriber, NULL);    // Start subscriber thread

    pthread_join(pub, NULL);     // Wait for publisher to finish
    pthread_join(sub, NULL);     // Wait for subscriber to finish

    sem_destroy(&semaphore);     // Clean up the semaphore

    return 0;
}
