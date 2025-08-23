#include <iostream>
#include <atomic>
#include <thread>

std::atomic<int> atomic_counter(0);

void increment() {
    for (int i = 0; i < 100000; ++i) {
        atomic_counter.fetch_add(1, std::memory_order_relaxed);
    }
}

int main() {
    std::thread t1(increment), t2(increment);
    t1.join();
    t2.join();
    std::cout << "Final atomic_counter: " << atomic_counter << std::endl;
    return 0;
}