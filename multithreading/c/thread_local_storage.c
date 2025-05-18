#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// Declare thread-local variable
__thread int tls_var = 0;

// Thread function
void* thread_function(void* arg) {
    tls_var = (int)(long)arg;  // Each thread gets its own version of tls_var
    printf("Thread %ld has tls_var = %d\n", pthread_self(), tls_var);
    sleep(1);
    return NULL;
}

int main() {
    pthread_t tid1, tid2;

    // Create two threads with different values
    pthread_create(&tid1, NULL, thread_function, (void*)10);
    pthread_create(&tid2, NULL, thread_function, (void*)20);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    return 0;
}
