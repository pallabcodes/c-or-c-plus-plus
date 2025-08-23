#include <pthread.h>
#include <iostream>
#include <vector>

pthread_rwlock_t rwlock;
std::vector<int> shared_data;

void* reader(void* arg) {
    int id = *(int*)arg;
    pthread_rwlock_rdlock(&rwlock);
    std::cout << "Reader " << id << " sees size: " << shared_data.size() << std::endl;
    pthread_rwlock_unlock(&rwlock);
    return nullptr;
}

void* writer(void* arg) {
    int id = *(int*)arg;
    pthread_rwlock_wrlock(&rwlock);
    shared_data.push_back(id);
    std::cout << "Writer " << id << " added data\n";
    pthread_rwlock_unlock(&rwlock);
    return nullptr;
}

int main() {
    pthread_rwlock_init(&rwlock, nullptr);
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