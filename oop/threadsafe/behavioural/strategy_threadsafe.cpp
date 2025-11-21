/*
 * Behavioral Pattern: Thread-Safe Strategy
 *
 * Demonstrates thread-safe Strategy pattern implementation with proper
 * synchronization for concurrent strategy changes and operations.
 */
#include <iostream>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <cassert>

// Thread-safety: Thread-safe (abstract interface)
// Ownership: Abstract base class, does not own derived objects
// Invariants: None
// Failure modes: None
class Lockable {
public:
    virtual ~Lockable() = default;

    // Thread-safety: Thread-safe (may modify state, must be implemented thread-safely)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    virtual void lock() = 0;

    // Thread-safety: Thread-safe (may modify state, must be implemented thread-safely)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    virtual void unlock() = 0;
};

class NonLocking : public Lockable {
public:
    void lock() override {
        std::cout << "[Thread " << std::this_thread::get_id() << "] "
                  << "Door does not lock - ignoring" << std::endl;
    }

    void unlock() override {
        std::cout << "[Thread " << std::this_thread::get_id() << "] "
                  << "Door cannot unlock because it cannot lock" << std::endl;
    }
};

class Password : public Lockable {
public:
    void lock() override {
        std::cout << "[Thread " << std::this_thread::get_id() << "] "
                  << "Door locked using password!" << std::endl;
    }

    void unlock() override {
        std::cout << "[Thread " << std::this_thread::get_id() << "] "
                  << "Door unlocked using password!" << std::endl;
    }
};

class KeyCard : public Lockable {
public:
    void lock() override {
        std::cout << "[Thread " << std::this_thread::get_id() << "] "
                  << "Door locked using key card!" << std::endl;
    }

    void unlock() override {
        std::cout << "[Thread " << std::this_thread::get_id() << "] "
                  << "Door unlocked using key card!" << std::endl;
    }
};

// Thread-safety: Thread-safe (abstract interface)
// Ownership: Abstract base class, does not own derived objects
// Invariants: None
// Failure modes: None
class Openable {
public:
    virtual ~Openable() = default;

    // Thread-safety: Thread-safe (may modify state, must be implemented thread-safely)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    virtual void open() = 0;

    // Thread-safety: Thread-safe (may modify state, must be implemented thread-safely)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    virtual void close() = 0;
};

class Standard : public Openable {
public:
    void open() override {
        std::cout << "[Thread " << std::this_thread::get_id() << "] "
                  << "Pushing door open" << std::endl;
    }

    void close() override {
        std::cout << "[Thread " << std::this_thread::get_id() << "] "
                  << "Pulling door closed" << std::endl;
    }
};

class Revolving : public Openable {
public:
    void open() override {
        std::cout << "[Thread " << std::this_thread::get_id() << "] "
                  << "Revolving door opened" << std::endl;
    }

    void close() override {
        std::cout << "[Thread " << std::this_thread::get_id() << "] "
                  << "Revolving door closed" << std::endl;
    }
};

// Thread-safety: Thread-safe (uses shared_mutex for read-write operations)
// Ownership: Owns Lockable and Openable strategies via unique_ptr
// Invariants: lockBehavior_ and openBehavior_ may be null
// Failure modes: Undefined behavior if strategies are null when called
class ThreadSafeDoor {
private:
    mutable std::shared_mutex mutex_;  // Mutable for const methods
    std::unique_ptr<Lockable> lockBehavior_;
    std::unique_ptr<Openable> openBehavior_;

public:
    // Thread-safety: Thread-safe (constructor initializes pointers)
    // Ownership: Initializes unique_ptrs to null
    // Invariants: None
    // Failure modes: None
    ThreadSafeDoor() : lockBehavior_(nullptr), openBehavior_(nullptr) {}

    // Thread-safety: Thread-safe (locks mutex exclusively)
    // Ownership: Takes ownership of lock (transfers to unique_ptr)
    // Invariants: None
    // Failure modes: None
    void setLockBehavior(std::unique_ptr<Lockable> lock) {
        std::unique_lock<std::shared_mutex> lock_guard(mutex_);
        lockBehavior_ = std::move(lock);
    }

    // Thread-safety: Thread-safe (locks mutex exclusively)
    // Ownership: Takes ownership of open (transfers to unique_ptr)
    // Invariants: None
    // Failure modes: None
    void setOpenBehavior(std::unique_ptr<Openable> open) {
        std::unique_lock<std::shared_mutex> lock_guard(mutex_);
        openBehavior_ = std::move(open);
    }

    // Thread-safety: Thread-safe (locks mutex for shared read)
    // Ownership: None
    // Invariants: lockBehavior_ must not be null
    // Failure modes: Undefined behavior if lockBehavior_ is null
    void performLock() {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        if (lockBehavior_) {
            lockBehavior_->lock();
        }
    }

    // Thread-safety: Thread-safe (locks mutex for shared read)
    // Ownership: None
    // Invariants: lockBehavior_ must not be null
    // Failure modes: Undefined behavior if lockBehavior_ is null
    void performUnlock() {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        if (lockBehavior_) {
            lockBehavior_->unlock();
        }
    }

    // Thread-safety: Thread-safe (locks mutex for shared read)
    // Ownership: None
    // Invariants: openBehavior_ must not be null
    // Failure modes: Undefined behavior if openBehavior_ is null
    void performOpen() {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        if (openBehavior_) {
            openBehavior_->open();
        }
    }

    // Thread-safety: Thread-safe (locks mutex for shared read)
    // Ownership: None
    // Invariants: openBehavior_ must not be null
    // Failure modes: Undefined behavior if openBehavior_ is null
    void performClose() {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        if (openBehavior_) {
            openBehavior_->close();
        }
    }
};

class ThreadSafeClosetDoor : public ThreadSafeDoor {
};

void operateDoorThread(ThreadSafeDoor& door, int threadId) {
    for (int i = 0; i < 3; ++i) {
        door.performOpen();
        door.performClose();
        door.performLock();
        door.performUnlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void changeStrategyThread(ThreadSafeDoor& door, int threadId) {
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
    if (threadId % 2 == 0) {
        door.setLockBehavior(std::make_unique<Password>());
    } else {
        door.setLockBehavior(std::make_unique<KeyCard>());
    }
}

int main() {
    auto door = std::make_unique<ThreadSafeClosetDoor>();

    door->setOpenBehavior(std::make_unique<Standard>());
    door->setLockBehavior(std::make_unique<NonLocking>());

    // Create threads that operate the door
    std::vector<std::thread> operatorThreads;
    for (int i = 0; i < 3; ++i) {
        operatorThreads.emplace_back(operateDoorThread, std::ref(*door), i);
    }

    // Create threads that change strategies
    std::vector<std::thread> strategyThreads;
    for (int i = 0; i < 2; ++i) {
        strategyThreads.emplace_back(changeStrategyThread, std::ref(*door), i);
    }

    // Wait for all threads
    for (auto& t : operatorThreads) {
        t.join();
    }
    for (auto& t : strategyThreads) {
        t.join();
    }

    return 0;
}

