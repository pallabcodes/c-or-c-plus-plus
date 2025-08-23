#include <pthread.h>
#include <iostream>
#include <unistd.h>

pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
int shared_data = 0;

void* reader(void* arg) {
    int id = *(int*)arg;
    pthread_rwlock_rdlock(&rwlock);
    std::cout << "Reader " << id << " reads: " << shared_data << std::endl;
    pthread_rwlock_unlock(&rwlock);
    return nullptr;
}

void* writer(void* arg) {
    int id = *(int*)arg;
    pthread_rwlock_wrlock(&rwlock);
    ++shared_data;
    std::cout << "Writer " << id << " writes: " << shared_data << std::endl;
    pthread_rwlock_unlock(&rwlock);
    return nullptr;
}

int main() {
    pthread_t r1, r2, w1;
    int id1 = 1, id2 = 2, id3 = 3;
    pthread_create(&w1, nullptr, writer, &id1);
    pthread_create(&r1, nullptr, reader, &id2);
    pthread_create(&r2, nullptr, reader, &id3);
    pthread_join(w1, nullptr);
    pthread_join(r1, nullptr);
    pthread_join(r2, nullptr);
    pthread_rwlock_destroy(&rwlock);
    return 0;
}