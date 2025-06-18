#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
int shared_data = 0;

void* reader(void* arg) {
    int id = *(int*)arg;
    while(1) {
        pthread_rwlock_rdlock(&rwlock);
        printf("Reader %d read value: %d\n", id, shared_data);
        pthread_rwlock_unlock(&rwlock);
        sleep(1);
    }
    return NULL;
}

void* writer(void* arg) {
    int id = *(int*)arg;
    while(1) {
        pthread_rwlock_wrlock(&rwlock);
        shared_data++;
        printf("Writer %d wrote value: %d\n", id, shared_data);
        pthread_rwlock_unlock(&rwlock);
        sleep(2);
    }
    return NULL;
}

int main() {
    pthread_t readers[3], writers[2];
    int reader_ids[3] = {1, 2, 3};
    int writer_ids[2] = {1, 2};

    for(int i = 0; i < 3; i++)
        pthread_create(&readers[i], NULL, reader, &reader_ids[i]);
    for(int i = 0; i < 2; i++)
        pthread_create(&writers[i], NULL, writer, &writer_ids[i]);

    sleep(10); // Let it run for 10 seconds
    pthread_rwlock_destroy(&rwlock);
    return 0;
}