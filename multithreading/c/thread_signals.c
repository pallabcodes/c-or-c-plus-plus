#define _GNU_SOURCE
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void signal_handler(int signo) {
    printf("Thread %ld received signal %d\n", pthread_self(), signo);
}

void* thread_function(void* arg) {
    // Set up signal mask for this thread
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    pthread_sigmask(SIG_BLOCK, &set, NULL);
    
    // Wait for signal
    int sig;
    sigwait(&set, &sig);
    printf("Thread received signal: %d\n", sig);
    
    return NULL;
}

int main() {
    pthread_t thread;
    
    // Set up signal handler
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);
    
    // Create thread
    pthread_create(&thread, NULL, thread_function, NULL);
    
    sleep(1);
    
    // Send signal to thread
    pthread_kill(thread, SIGUSR1);
    
    pthread_join(thread, NULL);
    return 0;
}