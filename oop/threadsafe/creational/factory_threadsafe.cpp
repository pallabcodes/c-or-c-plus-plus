/*
 * Creational Pattern: Thread-Safe Factory
 *
 * Demonstrates thread-safe Factory pattern implementation with proper
 * synchronization for concurrent object creation.
 */
#include <iostream>
#include <string>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>
#include <cassert>

enum class Burgers {
    CHEESE,
    DELUXECHEESE,
    VEGAN,
    DELUXEVEGAN
};

class Burger {
public:
    virtual ~Burger() = default;
    virtual void prepare() {}
    virtual void cook() {}
    virtual void serve() {}
    virtual std::string getName() const = 0;
};

class CheeseBurger : public Burger {
private:
    std::string name_;

public:
    CheeseBurger() : name_("Cheese Burger") {}
    std::string getName() const override { return name_; }
};

class DeluxeCheeseBurger : public Burger {
private:
    std::string name_;

public:
    DeluxeCheeseBurger() : name_("Deluxe Cheese Burger") {}
    std::string getName() const override { return name_; }
};

class VeganBurger : public Burger {
private:
    std::string name_;

public:
    VeganBurger() : name_("Vegan Burger") {}
    std::string getName() const override { return name_; }
};

class DeluxeVeganBurger : public Burger {
private:
    std::string name_;

public:
    DeluxeVeganBurger() : name_("Deluxe Vegan Burger") {}
    std::string getName() const override { return name_; }
};

// Thread-safety: Thread-safe (abstract interface)
// Ownership: Abstract base class, does not own derived objects
// Invariants: None
// Failure modes: None
class ThreadSafeBurgerStore {
public:
    virtual ~ThreadSafeBurgerStore() = default;

    // Thread-safety: Thread-safe (factory method, must be implemented thread-safely)
    // Ownership: Returns unique_ptr owning Burger instance
    // Invariants: None
    // Failure modes: Returns nullptr if item type not supported
    virtual std::unique_ptr<Burger> createBurger(Burgers item) = 0;

    // Thread-safety: Thread-safe (locks mutex, calls virtual method)
    // Ownership: Returns unique_ptr owning Burger instance
    // Invariants: type must be valid Burger type
    // Failure modes: Throws if createBurger returns nullptr
    std::unique_ptr<Burger> orderBurger(Burgers type) {
        std::lock_guard<std::mutex> lock(orderMutex_);
        auto burger = createBurger(type);
        if (!burger) {
            throw std::runtime_error("Failed to create burger: unsupported type");
        }
        std::cout << "[Thread " << std::this_thread::get_id() << "] "
                  << "--- Making a " << burger->getName() << " ---" << std::endl;
        burger->prepare();
        burger->cook();
        burger->serve();
        return burger;
    }

protected:
    mutable std::mutex orderMutex_;  // Protects orderBurger operations
};

// Thread-safety: Thread-safe (uses mutex for factory method)
// Ownership: Owns no resources
// Invariants: None
// Failure modes: Returns nullptr for unsupported burger types
class ThreadSafeCheeseBurgerStore : public ThreadSafeBurgerStore {
private:
    mutable std::mutex factoryMutex_;  // Protects factory operations

public:
    // Thread-safety: Thread-safe (locks mutex)
    // Ownership: Returns unique_ptr owning Burger instance
    // Invariants: None
    // Failure modes: Returns nullptr if item is not CHEESE or DELUXECHEESE
    std::unique_ptr<Burger> createBurger(Burgers item) override {
        std::lock_guard<std::mutex> lock(factoryMutex_);
        if (item == Burgers::CHEESE) {
            return std::make_unique<CheeseBurger>();
        } else if (item == Burgers::DELUXECHEESE) {
            return std::make_unique<DeluxeCheeseBurger>();
        }
        return nullptr;
    }
};

// Thread-safety: Thread-safe (uses mutex for factory method)
// Ownership: Owns no resources
// Invariants: None
// Failure modes: Returns nullptr for unsupported burger types
class ThreadSafeVeganBurgerStore : public ThreadSafeBurgerStore {
private:
    mutable std::mutex factoryMutex_;  // Protects factory operations

public:
    // Thread-safety: Thread-safe (locks mutex)
    // Ownership: Returns unique_ptr owning Burger instance
    // Invariants: None
    // Failure modes: Returns nullptr if item is not VEGAN or DELUXEVEGAN
    std::unique_ptr<Burger> createBurger(Burgers item) override {
        std::lock_guard<std::mutex> lock(factoryMutex_);
        if (item == Burgers::VEGAN) {
            return std::make_unique<VeganBurger>();
        } else if (item == Burgers::DELUXEVEGAN) {
            return std::make_unique<DeluxeVeganBurger>();
        }
        return nullptr;
    }
};

void orderBurgersThread(ThreadSafeBurgerStore& store, int threadId, Burgers type) {
    for (int i = 0; i < 3; ++i) {
        try {
            auto burger = store.orderBurger(type);
            std::cout << "[Thread " << threadId << "] Order " << i << ": "
                      << burger->getName() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[Thread " << threadId << "] Error: " << e.what() << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main() {
    auto cheeseStore = std::make_unique<ThreadSafeCheeseBurgerStore>();
    auto veganStore = std::make_unique<ThreadSafeVeganBurgerStore>();

    std::vector<std::thread> threads;

    // Create threads ordering from cheese store
    for (int i = 0; i < 2; ++i) {
        threads.emplace_back(orderBurgersThread, std::ref(*cheeseStore), i, Burgers::CHEESE);
    }

    // Create threads ordering from vegan store
    for (int i = 2; i < 4; ++i) {
        threads.emplace_back(orderBurgersThread, std::ref(*veganStore), i, Burgers::VEGAN);
    }

    // Wait for all threads
    for (auto& t : threads) {
        t.join();
    }

    return 0;
}

