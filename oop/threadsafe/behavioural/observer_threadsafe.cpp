/*
 * Behavioral Pattern: Thread-Safe Observer
 *
 * Demonstrates thread-safe Observer pattern implementation with proper
 * synchronization for concurrent observer registration and notification.
 */
#include <iostream>
#include <vector>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <algorithm>
#include <cassert>

class Observer;

// Thread-safety: Thread-safe (abstract interface)
// Ownership: Abstract base class, does not own derived objects
// Invariants: None
// Failure modes: None
class ThreadSafeSubject {
public:
    virtual ~ThreadSafeSubject() = default;

    // Thread-safety: Thread-safe (must be implemented thread-safely)
    // Ownership: Borrows observer pointer (does not own)
    // Invariants: observer must not be null
    // Failure modes: Undefined behavior if observer is null
    virtual void registerObserver(Observer* observer) = 0;

    // Thread-safety: Thread-safe (must be implemented thread-safely)
    // Ownership: Borrows observer pointer (does not own)
    // Invariants: observer must not be null
    // Failure modes: Undefined behavior if observer is null
    virtual void removeObserver(Observer* observer) = 0;

    // Thread-safety: Thread-safe (must be implemented thread-safely)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    virtual void notifyObservers() = 0;
};

// Thread-safety: Thread-safe (abstract interface)
// Ownership: Abstract base class, does not own derived objects
// Invariants: None
// Failure modes: None
class Observer {
public:
    virtual ~Observer() = default;

    // Thread-safety: Thread-safe (may modify state, must be implemented thread-safely)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    virtual void update(int value) = 0;
};

// Thread-safety: Thread-safe (uses shared_mutex for read-write operations)
// Ownership: Owns no resources (stores raw pointers, does not own)
// Invariants: observers_ contains valid pointers
// Failure modes: Undefined behavior if observers_ contain invalid pointers
class ThreadSafeConcreteSubject : public ThreadSafeSubject {
private:
    mutable std::shared_mutex mutex_;  // Mutable for const methods
    std::vector<Observer*> observers_;
    int value_;

public:
    // Thread-safety: Thread-safe (constructor initializes value_)
    // Ownership: Initializes value_ to 0
    // Invariants: None
    // Failure modes: None
    ThreadSafeConcreteSubject() : value_(0) {}

    // Thread-safety: Thread-safe (locks mutex exclusively)
    // Ownership: Borrows observer pointer (does not own)
    // Invariants: observer must not be null
    // Failure modes: Undefined behavior if observer is null
    void registerObserver(Observer* observer) override {
        assert(observer != nullptr && "Observer must not be null");
        std::unique_lock<std::shared_mutex> lock(mutex_);
        observers_.push_back(observer);
    }

    // Thread-safety: Thread-safe (locks mutex exclusively)
    // Ownership: Borrows observer pointer (does not own)
    // Invariants: observer must not be null
    // Failure modes: Undefined behavior if observer is null
    void removeObserver(Observer* observer) override {
        assert(observer != nullptr && "Observer must not be null");
        std::unique_lock<std::shared_mutex> lock(mutex_);
        observers_.erase(
            std::remove(observers_.begin(), observers_.end(), observer),
            observers_.end());
    }

    // Thread-safety: Thread-safe (locks mutex for shared read, then notifies)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void notifyObservers() override {
        std::vector<Observer*> observersCopy;
        {
            std::shared_lock<std::shared_mutex> lock(mutex_);
            observersCopy = observers_;  // Copy observers list
        }
        // Notify outside lock to avoid deadlock if observer methods lock
        for (Observer* observer : observersCopy) {
            observer->update(value_);
        }
    }

    // Thread-safety: Thread-safe (locks mutex exclusively)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void setValue(int val) {
        {
            std::unique_lock<std::shared_mutex> lock(mutex_);
            value_ = val;
        }
        notifyObservers();  // Notify outside lock
    }

    // Thread-safety: Thread-safe (locks mutex for shared read)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    int getValue() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return value_;
    }
};

// Thread-safety: Thread-safe (uses mutex for state protection)
// Ownership: Owns no resources (stores raw pointer to Subject, does not own)
// Invariants: subject_ must be valid
// Failure modes: Undefined behavior if subject_ is invalid
class ThreadSafeConcreteObserver : public Observer {
private:
    mutable std::mutex mutex_;
    int value_;
    ThreadSafeSubject* subject_;

public:
    // Thread-safety: Thread-safe (constructor registers observer)
    // Ownership: Borrows subject pointer (does not own)
    // Invariants: subject must not be null
    // Failure modes: Undefined behavior if subject is null
    explicit ThreadSafeConcreteObserver(ThreadSafeSubject* subject)
        : value_(0), subject_(subject) {
        assert(subject != nullptr && "Subject must not be null");
        subject_->registerObserver(this);
    }

    // Thread-safety: Thread-safe (locks mutex)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void update(int val) override {
        std::lock_guard<std::mutex> lock(mutex_);
        value_ = val;
        std::cout << "[Observer " << std::this_thread::get_id() << "] "
                  << "Updated with value: " << value_ << std::endl;
    }

    // Thread-safety: Thread-safe (locks mutex)
    // Ownership: Returns copy of value_
    // Invariants: None
    // Failure modes: None
    int getValue() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return value_;
    }
};

void updateSubjectThread(ThreadSafeConcreteSubject& subject, int threadId) {
    for (int i = 0; i < 5; ++i) {
        int value = threadId * 10 + i;
        subject.setValue(value);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    auto subject = std::make_unique<ThreadSafeConcreteSubject>();

    // Create observers
    std::vector<std::unique_ptr<ThreadSafeConcreteObserver>> observers;
    for (int i = 0; i < 3; ++i) {
        observers.push_back(
            std::make_unique<ThreadSafeConcreteObserver>(subject.get()));
    }

    // Create threads that update the subject
    std::vector<std::thread> updaterThreads;
    for (int i = 0; i < 2; ++i) {
        updaterThreads.emplace_back(updateSubjectThread, std::ref(*subject), i);
    }

    // Wait for updater threads
    for (auto& t : updaterThreads) {
        t.join();
    }

    // Remove one observer
    subject->removeObserver(observers[0].get());

    // Update one more time
    subject->setValue(999);

    std::cout << "\nFinal observer values:" << std::endl;
    for (size_t i = 0; i < observers.size(); ++i) {
        std::cout << "Observer " << i << ": " << observers[i]->getValue() << std::endl;
    }

    return 0;
}

