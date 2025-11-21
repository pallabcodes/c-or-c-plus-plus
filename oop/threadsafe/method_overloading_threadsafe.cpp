/*
 * Object-Oriented Programming: Thread-Safe Method Overloading
 *
 * Demonstrates thread-safe method overloading with proper synchronization
 * for concurrent method calls.
 */
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <cassert>

// Thread-safety: Thread-safe (uses mutex for state protection)
// Ownership: Owns no resources
// Invariants: None
// Failure modes: Undefined behavior if arr is null and size > 0
class ThreadSafeMyClass {
private:
    mutable std::mutex mutex_;
    int callCount_;  // Example mutable state

public:
    // Thread-safety: Thread-safe (constructor initializes state)
    // Ownership: Initializes callCount_ to 0
    // Invariants: None
    // Failure modes: None
    ThreadSafeMyClass() : callCount_(0) {}

    // Thread-safety: Thread-safe (locks mutex, calls overloaded method)
    // Ownership: Borrows arr (read-only)
    // Invariants: arr must be valid for size elements if size > 0
    // Failure modes: Undefined behavior if arr is null and size > 0
    int doSomething(int arr[], int size) {
        assert(arr != nullptr || size == 0);
        return doSomething(arr, size, true);
    }

    // Thread-safety: Thread-safe (locks mutex)
    // Ownership: Borrows arr (read-only)
    // Invariants: arr must be valid for size elements if size > 0
    // Failure modes: Undefined behavior if arr is null and size > 0
    int doSomething(int arr[], int size, bool flag) {
        assert(arr != nullptr || size == 0);
        std::lock_guard<std::mutex> lock(mutex_);
        ++callCount_;
        // Implementation
        std::cout << "[Thread " << std::this_thread::get_id() << "] "
                  << "doSomething called (size=" << size << ", flag=" << flag
                  << ", callCount=" << callCount_ << ")" << std::endl;
        return 0;
    }

    // Thread-safety: Thread-safe (locks mutex)
    // Ownership: Returns copy of callCount_
    // Invariants: None
    // Failure modes: None
    int getCallCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return callCount_;
    }
};

void callMethodsThread(ThreadSafeMyClass& obj, int threadId) {
    int arr[] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; ++i) {
        obj.doSomething(arr, 5);
        obj.doSomething(arr, 5, (i % 2 == 0));
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main() {
    ThreadSafeMyClass obj;

    std::vector<std::thread> threads;
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back(callMethodsThread, std::ref(obj), i);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Total calls: " << obj.getCallCount() << std::endl;

    return 0;
}

