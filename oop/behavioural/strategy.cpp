/*
 * Behavioral Pattern: Strategy
 *
 * Demonstrates the Strategy pattern for encapsulating algorithms and making
 * them interchangeable at runtime.
 */
#include <iostream>
#include <memory>
#include <cassert>

// Thread-safety: Not thread-safe (abstract interface)
// Ownership: Abstract base class, does not own derived objects
// Invariants: None
// Failure modes: None
class Lockable {
public:
    virtual ~Lockable() = default;

    // Thread-safety: Not thread-safe (may modify state)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    virtual void lock() = 0;

    // Thread-safety: Not thread-safe (may modify state)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    virtual void unlock() = 0;
};

// Thread-safety: Thread-safe (no mutable state)
// Ownership: Owns no resources
// Invariants: None
// Failure modes: None
class NonLocking : public Lockable {
public:
    // Thread-safety: Thread-safe (no state modification)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void lock() override {
        std::cout << "Door does not lock - ignoring" << std::endl;
    }

    // Thread-safety: Thread-safe (no state modification)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void unlock() override {
        std::cout << "Door cannot unlock because it cannot lock" << std::endl;
    }
};

// Thread-safety: Thread-safe (no mutable state)
// Ownership: Owns no resources
// Invariants: None
// Failure modes: None
class Password : public Lockable {
public:
    // Thread-safety: Thread-safe (no state modification)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void lock() override {
        std::cout << "Door locked using password!" << std::endl;
    }

    // Thread-safety: Thread-safe (no state modification)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void unlock() override {
        std::cout << "Door unlocked using password!" << std::endl;
    }
};

// Thread-safety: Thread-safe (no mutable state)
// Ownership: Owns no resources
// Invariants: None
// Failure modes: None
class KeyCard : public Lockable {
public:
    // Thread-safety: Thread-safe (no state modification)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void lock() override {
        std::cout << "Door locked using key card!" << std::endl;
    }

    // Thread-safety: Thread-safe (no state modification)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void unlock() override {
        std::cout << "Door unlocked using key card!" << std::endl;
    }
};

// Thread-safety: Not thread-safe (abstract interface)
// Ownership: Abstract base class, does not own derived objects
// Invariants: None
// Failure modes: None
class Openable {
public:
    virtual ~Openable() = default;

    // Thread-safety: Not thread-safe (may modify state)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    virtual void open() = 0;

    // Thread-safety: Not thread-safe (may modify state)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    virtual void close() = 0;
};

// Thread-safety: Thread-safe (no mutable state)
// Ownership: Owns no resources
// Invariants: None
// Failure modes: None
class Standard : public Openable {
public:
    // Thread-safety: Thread-safe (no state modification)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void open() override {
        std::cout << "Pushing door open" << std::endl;
    }

    // Thread-safety: Thread-safe (no state modification)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void close() override {
        std::cout << "Pulling door closed" << std::endl;
    }
};

// Thread-safety: Thread-safe (no mutable state)
// Ownership: Owns no resources
// Invariants: None
// Failure modes: None
class Revolving : public Openable {
public:
    // Thread-safety: Thread-safe (no state modification)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void open() override {
        std::cout << "Revolving door opened" << std::endl;
    }

    // Thread-safety: Thread-safe (no state modification)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void close() override {
        std::cout << "Revolving door closed" << std::endl;
    }
};

// Thread-safety: Thread-safe (no mutable state)
// Ownership: Owns no resources
// Invariants: None
// Failure modes: None
class Sliding : public Openable {
public:
    // Thread-safety: Thread-safe (no state modification)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void open() override {
        std::cout << "Sliding door opened" << std::endl;
    }

    // Thread-safety: Thread-safe (no state modification)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void close() override {
        std::cout << "Sliding door closed" << std::endl;
    }
};

// Thread-safety: Not thread-safe (mutable state, no synchronization)
// Ownership: Owns Lockable and Openable strategies via unique_ptr
// Invariants: lockBehavior_ and openBehavior_ may be null
// Failure modes: Undefined behavior if strategies are null when called
class Door {
protected:
    std::unique_ptr<Lockable> lockBehavior_;
    std::unique_ptr<Openable> openBehavior_;

public:
    // Thread-safety: Not thread-safe (constructor modifies object)
    // Ownership: Initializes unique_ptrs to null
    // Invariants: None
    // Failure modes: None
    Door() : lockBehavior_(nullptr), openBehavior_(nullptr) {}

    // Thread-safety: Not thread-safe (modifies lockBehavior_)
    // Ownership: Takes ownership of lock (transfers to unique_ptr)
    // Invariants: None
    // Failure modes: None
    void setLockBehavior(std::unique_ptr<Lockable> lock) {
        lockBehavior_ = std::move(lock);
    }

    // Thread-safety: Not thread-safe (modifies openBehavior_)
    // Ownership: Takes ownership of open (transfers to unique_ptr)
    // Invariants: None
    // Failure modes: None
    void setOpenBehavior(std::unique_ptr<Openable> open) {
        openBehavior_ = std::move(open);
    }

    // Thread-safety: Not thread-safe (calls virtual method)
    // Ownership: None
    // Invariants: lockBehavior_ must not be null
    // Failure modes: Undefined behavior if lockBehavior_ is null
    void performLock() {
        if (lockBehavior_) {
            lockBehavior_->lock();
        }
    }

    // Thread-safety: Not thread-safe (calls virtual method)
    // Ownership: None
    // Invariants: lockBehavior_ must not be null
    // Failure modes: Undefined behavior if lockBehavior_ is null
    void performUnlock() {
        if (lockBehavior_) {
            lockBehavior_->unlock();
        }
    }

    // Thread-safety: Not thread-safe (calls virtual method)
    // Ownership: None
    // Invariants: openBehavior_ must not be null
    // Failure modes: Undefined behavior if openBehavior_ is null
    void performOpen() {
        if (openBehavior_) {
            openBehavior_->open();
        }
    }

    // Thread-safety: Not thread-safe (calls virtual method)
    // Ownership: None
    // Invariants: openBehavior_ must not be null
    // Failure modes: Undefined behavior if openBehavior_ is null
    void performClose() {
        if (openBehavior_) {
            openBehavior_->close();
        }
    }
};

// Thread-safety: Not thread-safe (inherits Door behavior)
// Ownership: Inherits Door ownership
// Invariants: Inherits Door invariants
// Failure modes: Inherits Door failure modes
class ClosetDoor : public Door {
};

// Thread-safety: Not thread-safe (inherits Door behavior)
// Ownership: Inherits Door ownership
// Invariants: Inherits Door invariants
// Failure modes: Inherits Door failure modes
class ExternalDoor : public Door {
};

// Thread-safety: Not thread-safe (inherits Door behavior)
// Ownership: Inherits Door ownership
// Invariants: Inherits Door invariants
// Failure modes: Inherits Door failure modes
class SafeDepositDoor : public Door {
};

// Thread-safety: Not thread-safe (inherits Door behavior)
// Ownership: Inherits Door ownership
// Invariants: Inherits Door invariants
// Failure modes: Inherits Door failure modes
class SlidingDoor : public Door {
};

int main() {
    auto door = std::make_unique<ClosetDoor>();

    door->setOpenBehavior(std::make_unique<Standard>());
    door->setLockBehavior(std::make_unique<NonLocking>());

    door->performOpen();
    door->performClose();
    door->performLock();
    door->performUnlock();

    door->setLockBehavior(std::make_unique<Password>());
    door->performLock();
    door->performUnlock();

    return 0;
}
