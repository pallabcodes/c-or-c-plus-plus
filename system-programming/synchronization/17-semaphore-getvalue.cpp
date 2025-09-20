#include <semaphore.h>
#include <iostream>

int main() {
    sem_t sem;
    sem_init(&sem, 0, 3);
    int val;
    sem_getvalue(&sem, &val);
    std::cout << "Semaphore value: " << val << std::endl;
    sem_destroy(&sem);
    return 0;
}