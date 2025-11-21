/*
 * Creational Pattern: Thread-Safe Singleton
 *
 * Demonstrates thread-safe Singleton pattern with proper synchronization
 * using C++11 static local variable initialization (Meyers' Singleton).
 * This version is fully thread-safe and suitable for production use.
 */
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <cassert>

// Thread-safety: Thread-safe (uses static local variable, C++11 guarantees)
// Ownership: Owns single instance (static lifetime)
// Invariants: Only one instance exists
// Failure modes: None (static initialization is exception-safe)
class ThreadSafePrinterService {
private:
    std::string mode_;
    mutable std::mutex mutex_;  // Mutable to allow const methods to lock

    // Thread-safety: Thread-safe (private constructor)
    // Ownership: Constructs instance
    // Invariants: None
    // Failure modes: None
    ThreadSafePrinterService() : mode_("GrayScale") {}

public:
    // Delete copy constructor and assignment operator
    ThreadSafePrinterService(const ThreadSafePrinterService&) = delete;
    ThreadSafePrinterService& operator=(const ThreadSafePrinterService&) = delete;

    // Thread-safety: Thread-safe (C++11 static local initialization is thread-safe)
    // Ownership: Returns reference to static instance
    // Invariants: Always returns same instance
    // Failure modes: None
    static ThreadSafePrinterService& getInstance() {
        static ThreadSafePrinterService instance;
        return instance;
    }

    // Thread-safety: Thread-safe (locks mutex, const method)
    // Ownership: Returns copy of mode_
    // Invariants: None
    // Failure modes: None
    std::string getPrinterStatus() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return mode_;
    }

    // Thread-safety: Thread-safe (locks mutex before modification)
    // Ownership: Takes ownership of newMode parameter (copies)
    // Invariants: newMode must be non-empty
    // Failure modes: Undefined behavior if newMode is empty
    void setMode(const std::string& newMode) {
        assert(!newMode.empty() && "Mode must be non-empty");
        std::lock_guard<std::mutex> lock(mutex_);
        mode_ = newMode;
        std::cout << "[Thread " << std::this_thread::get_id() << "] "
                  << "Mode changed to " << mode_ << std::endl;
    }

    // Thread-safety: Thread-safe (locks mutex)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void performOperation() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << "[Thread " << std::this_thread::get_id() << "] "
                  << "Performing operation in " << mode_ << " mode" << std::endl;
    }
};

// Thread-safety: Thread-safe (uses double-checked locking with atomic)
// Ownership: Owns single instance (static lifetime)
// Invariants: Only one instance exists
// Failure modes: None
class DoubleCheckedSingleton {
private:
    static std::atomic<DoubleCheckedSingleton*> instance_;
    static std::mutex mutex_;
    std::string data_;

    // Thread-safety: Thread-safe (private constructor)
    // Ownership: Constructs instance
    // Invariants: None
    // Failure modes: None
    DoubleCheckedSingleton() : data_("Initialized") {}

public:
    // Delete copy constructor and assignment operator
    DoubleCheckedSingleton(const DoubleCheckedSingleton&) = delete;
    DoubleCheckedSingleton& operator=(const DoubleCheckedSingleton&) = delete;

    // Thread-safety: Thread-safe (double-checked locking pattern)
    // Ownership: Returns pointer to singleton instance
    // Invariants: Always returns same instance
    // Failure modes: None
    static DoubleCheckedSingleton* getInstance() {
        DoubleCheckedSingleton* tmp = instance_.load(std::memory_order_acquire);
        if (tmp == nullptr) {
            std::lock_guard<std::mutex> lock(mutex_);
            tmp = instance_.load(std::memory_order_relaxed);
            if (tmp == nullptr) {
                tmp = new DoubleCheckedSingleton();
                instance_.store(tmp, std::memory_order_release);
            }
        }
        return tmp;
    }

    // Thread-safety: Thread-safe (locks mutex)
    // Ownership: Returns copy of data_
    // Invariants: None
    // Failure modes: None
    std::string getData() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_;
    }

    // Thread-safety: Thread-safe (locks mutex)
    // Ownership: Takes ownership of data parameter (copies)
    // Invariants: None
    // Failure modes: None
    void setData(const std::string& data) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_ = data;
    }
};

// Static member definitions
std::atomic<DoubleCheckedSingleton*> DoubleCheckedSingleton::instance_{nullptr};
std::mutex DoubleCheckedSingleton::mutex_;

void testMeyersSingleton(int threadId) {
    for (int i = 0; i < 5; ++i) {
        auto& service = ThreadSafePrinterService::getInstance();
        service.setMode("Mode_" + std::to_string(threadId) + "_" + std::to_string(i));
        service.performOperation();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void testDoubleCheckedSingleton(int threadId) {
    for (int i = 0; i < 5; ++i) {
        auto* instance = DoubleCheckedSingleton::getInstance();
        instance->setData("Data_" + std::to_string(threadId) + "_" + std::to_string(i));
        std::cout << "[Thread " << threadId << "] Data: " << instance->getData() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main() {
    std::cout << "=== Testing Meyers' Singleton (Recommended) ===" << std::endl;
    std::vector<std::thread> threads1;
    for (int i = 0; i < 3; ++i) {
        threads1.emplace_back(testMeyersSingleton, i);
    }
    for (auto& t : threads1) {
        t.join();
    }

    std::cout << "\n=== Testing Double-Checked Locking Singleton ===" << std::endl;
    std::vector<std::thread> threads2;
    for (int i = 0; i < 3; ++i) {
        threads2.emplace_back(testDoubleCheckedSingleton, i);
    }
    for (auto& t : threads2) {
        t.join();
    }

    // Verify singleton property
    auto& service1 = ThreadSafePrinterService::getInstance();
    auto& service2 = ThreadSafePrinterService::getInstance();
    std::cout << "\nSame instance: " << (&service1 == &service2 ? "Yes" : "No") << std::endl;

    return 0;
}

