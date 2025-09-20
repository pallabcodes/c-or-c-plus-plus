#include <atomic>
#include <thread>
#include <iostream>

// Demonstrates thread-safe atomic counter using std::atomic

std::atomic<int> acnt(0);

// Increment function to be run by multiple threads
void inc() {
    for (int i = 0; i < 100000; ++i) {
        acnt++; // Atomic increment, safe from race conditions
    }
}

int main() {
    // Create two threads that increment the atomic counter
    std::thread t1(inc), t2(inc);

    // Wait for both threads to finish
    t1.join();
    t2.join();

    // Print the final value of the atomic counter
    std::cout << "Atomic counter: " << acnt << std::endl;
    return 0;
}