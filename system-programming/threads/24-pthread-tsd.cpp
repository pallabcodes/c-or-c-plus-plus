#include <pthread.h>
#include <iostream>

pthread_key_t key;

void destructor(void* val) {
    std::cout << "Destructor called for value: " << *(int*)val << std::endl;
    delete (int*)val;
}

void* worker(void* arg) {
    int* val = new int(*(int*)arg);
    pthread_setspecific(key, val);
    std::cout << "Thread-specific value: " << *(int*)pthread_getspecific(key) << std::endl;
    return nullptr;
}

int main() {
    pthread_key_create(&key, destructor);
    int v1 = 42, v2 = 99;
    pthread_t t1, t2;
    pthread_create(&t1, nullptr, worker, &v1);
    pthread_create(&t2, nullptr, worker, &v2);
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    pthread_key_delete(key);
    return 0;
}