#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <atomic>
#include <thread>
#include <iostream>

int futex_wait(std::atomic<int>* addr, int val) {
    return syscall(SYS_futex, addr, FUTEX_WAIT, val, NULL, NULL, 0);
}
int futex_wake(std::atomic<int>* addr, int count) {
    return syscall(SYS_futex, addr, FUTEX_WAKE, count, NULL, NULL, 0);
}

std::atomic<int> futex_var(0);

void waiter() {
    std::cout << "Waiting on futex...\n";
    futex_wait(&futex_var, 0);
    std::cout << "Futex released!\n";
}

void waker() {
    sleep(1);
    futex_var = 1;
    futex_wake(&futex_var, 1);
}

int main() {
    std::thread t1(waiter), t2(waker);
    t1.join(); t2.join();
    return 0;
}