/*
 * Structural Pattern: Thread-Safe Decorator
 *
 * Demonstrates thread-safe Decorator pattern implementation. While decorator
 * methods are typically const and thread-safe, this version adds protection
 * for cases where decorators are shared or need additional synchronization.
 */
#include <iostream>
#include <string>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>
#include <cassert>

// Thread-safety: Thread-safe (abstract interface)
// Ownership: Abstract base class, does not own derived objects
// Invariants: None
// Failure modes: None
class ThreadSafeBeverage {
public:
    virtual ~ThreadSafeBeverage() = default;

    // Thread-safety: Thread-safe (const method, pure calculation)
    // Ownership: None (pure calculation)
    // Invariants: None
    // Failure modes: None
    virtual double cost() const = 0;

    // Thread-safety: Thread-safe (const method, returns copy)
    // Ownership: Returns copy of description
    // Invariants: None
    // Failure modes: None
    virtual std::string description() const = 0;
};

class ThreadSafeLightRoast : public ThreadSafeBeverage {
public:
    double cost() const override {
        return 3.45;
    }

    std::string description() const override {
        return "Light Roast";
    }
};

// Thread-safety: Thread-safe (uses shared_mutex for read operations)
// Ownership: Owns wrapped Beverage via unique_ptr
// Invariants: beverage_ must not be null
// Failure modes: Undefined behavior if beverage_ is null
class ThreadSafeBeverageDecorator : public ThreadSafeBeverage {
protected:
    mutable std::shared_mutex mutex_;  // Mutable for const methods
    std::unique_ptr<ThreadSafeBeverage> beverage_;

public:
    // Thread-safety: Thread-safe (constructor takes ownership)
    // Ownership: Takes ownership of beverage (transfers to unique_ptr)
    // Invariants: beverage must not be null
    // Failure modes: Undefined behavior if beverage is null
    explicit ThreadSafeBeverageDecorator(std::unique_ptr<ThreadSafeBeverage> beverage)
        : beverage_(std::move(beverage)) {
        assert(beverage_ != nullptr && "Beverage must not be null");
    }

    virtual ~ThreadSafeBeverageDecorator() = default;
};

class ThreadSafeEspressoDecorator : public ThreadSafeBeverageDecorator {
public:
    explicit ThreadSafeEspressoDecorator(std::unique_ptr<ThreadSafeBeverage> beverage)
        : ThreadSafeBeverageDecorator(std::move(beverage)) {}

    // Thread-safety: Thread-safe (locks mutex for shared read)
    // Ownership: None (pure calculation)
    // Invariants: None
    // Failure modes: None
    double cost() const override {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return 0.5 + beverage_->cost();
    }

    // Thread-safety: Thread-safe (locks mutex for shared read)
    // Ownership: Returns copy of string
    // Invariants: None
    // Failure modes: None
    std::string description() const override {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return beverage_->description() + ", Espresso";
    }
};

class ThreadSafeCreamDecorator : public ThreadSafeBeverageDecorator {
public:
    explicit ThreadSafeCreamDecorator(std::unique_ptr<ThreadSafeBeverage> beverage)
        : ThreadSafeBeverageDecorator(std::move(beverage)) {}

    double cost() const override {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return 0.3 + beverage_->cost();
    }

    std::string description() const override {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return beverage_->description() + ", Cream";
    }
};

class ThreadSafeFoamDecorator : public ThreadSafeBeverageDecorator {
public:
    explicit ThreadSafeFoamDecorator(std::unique_ptr<ThreadSafeBeverage> beverage)
        : ThreadSafeBeverageDecorator(std::move(beverage)) {}

    double cost() const override {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return 0.2 + beverage_->cost();
    }

    std::string description() const override {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return beverage_->description() + ", Foam";
    }
};

void queryBeverageThread(const ThreadSafeBeverage& beverage, int threadId) {
    for (int i = 0; i < 3; ++i) {
        double cost = beverage.cost();
        std::string desc = beverage.description();
        std::cout << "[Thread " << threadId << "] " << desc
                  << " costs $" << cost << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main() {
    auto beverage = std::make_shared<ThreadSafeFoamDecorator>(
        std::make_unique<ThreadSafeCreamDecorator>(
            std::make_unique<ThreadSafeEspressoDecorator>(
                std::make_unique<ThreadSafeLightRoast>())));

    std::vector<std::thread> threads;
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back(queryBeverageThread, std::cref(*beverage), i);
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}

