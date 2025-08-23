#include <atomic>
#include <thread>
#include <iostream>

std::atomic<int> acnt(0);

void inc() {
    for (int i = 0; i < 100000; ++i) acnt++;
}

int main() {
    std::thread t1(inc), t2(inc);
    t1.join(); t2.join();
    std::cout << "Atomic counter: " << acnt << std::endl;
    return 0;
}