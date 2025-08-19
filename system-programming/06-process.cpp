#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>


int val1;
int val2;

int main (int argc, char *argv[]) {
    val1 = atoi(argv[1]);
    val2 = atoi(argv[2]);

    while(1) {
        // Simulate some work
        sleep(1);
        std::cout << "Working... (PID: " << getpid() << ")" << std::endl;

        printf("Value 1: %d \t location: %p \t Value 2: %d \t location: %p\n", val1, (void*)&val1, val2, (void*)&val2);
    }

    // to see all the processes in the system, run `ps -ef` or `ps -A` in another terminal
    return 0;
}