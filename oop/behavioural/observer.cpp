/*
 * Behavioral Pattern: Observer
 *
 * Demonstrates the Observer pattern for one-to-many dependency between objects,
 * where one object's state change notifies all dependent objects.
 */
#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <cassert>

class Observer;

// Thread-safety: Not thread-safe (abstract interface)
// Ownership: Abstract base class, does not own derived objects
// Invariants: None
// Failure modes: None
class Subject {
public:
    virtual ~Subject() = default;

    // Thread-safety: Not thread-safe (modifies observer list)
    // Ownership: Borrows observer pointer (does not own)
    // Invariants: observer must not be null
    // Failure modes: Undefined behavior if observer is null
    virtual void registerObserver(Observer* observer) = 0;

    // Thread-safety: Not thread-safe (modifies observer list)
    // Ownership: Borrows observer pointer (does not own)
    // Invariants: observer must not be null
    // Failure modes: Undefined behavior if observer is null
    virtual void removeObserver(Observer* observer) = 0;

    // Thread-safety: Not thread-safe (calls observer methods)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    virtual void notifyObservers() = 0;
};

// Thread-safety: Not thread-safe (abstract interface)
// Ownership: Abstract base class, does not own derived objects
// Invariants: None
// Failure modes: None
class Observer {
public:
    virtual ~Observer() = default;

    // Thread-safety: Not thread-safe (may modify state)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    virtual void update(int value) = 0;
};

// Thread-safety: Not thread-safe (mutable state, no synchronization)
// Ownership: Owns no resources (stores raw pointers, does not own)
// Invariants: observers contains valid pointers
// Failure modes: Undefined behavior if observers contain invalid pointers
class ConcreteSubject : public Subject {
private:
    std::vector<Observer*> observers_;
    int value_;

public:
    // Thread-safety: Not thread-safe (constructor modifies object)
    // Ownership: Initializes value_ to 0
    // Invariants: None
    // Failure modes: None
    ConcreteSubject() : value_(0) {}

    // Thread-safety: Not thread-safe (modifies observers_)
    // Ownership: Borrows observer pointer (does not own)
    // Invariants: observer must not be null
    // Failure modes: Undefined behavior if observer is null
    void registerObserver(Observer* observer) override {
        assert(observer != nullptr && "Observer must not be null");
        observers_.push_back(observer);
    }

    // Thread-safety: Not thread-safe (modifies observers_)
    // Ownership: Borrows observer pointer (does not own)
    // Invariants: observer must not be null
    // Failure modes: Undefined behavior if observer is null
    void removeObserver(Observer* observer) override {
        assert(observer != nullptr && "Observer must not be null");
        observers_.erase(
            std::remove(observers_.begin(), observers_.end(), observer),
            observers_.end());
    }

    // Thread-safety: Not thread-safe (calls observer methods)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void notifyObservers() override {
        for (Observer* observer : observers_) {
            observer->update(value_);
        }
    }

    // Thread-safety: Not thread-safe (modifies value_ and calls notifyObservers)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void setValue(int val) {
        value_ = val;
        notifyObservers();
    }
};

// Thread-safety: Not thread-safe (mutable state)
// Ownership: Owns no resources (stores raw pointer to Subject, does not own)
// Invariants: subject must be valid
// Failure modes: Undefined behavior if subject is invalid
class ConcreteObserver : public Observer {
private:
    int value_;
    Subject* subject_;

public:
    // Thread-safety: Not thread-safe (constructor registers observer)
    // Ownership: Borrows subject pointer (does not own)
    // Invariants: subject must not be null
    // Failure modes: Undefined behavior if subject is null
    explicit ConcreteObserver(Subject* subject) : value_(0), subject_(subject) {
        assert(subject != nullptr && "Subject must not be null");
        subject_->registerObserver(this);
    }

    // Thread-safety: Not thread-safe (modifies value_)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void update(int val) override {
        value_ = val;
        std::cout << "ConcreteObserver updated with value: " << value_ << std::endl;
    }
};

class Customer;

// Thread-safety: Not thread-safe (abstract interface)
// Ownership: Abstract base class, does not own derived objects
// Invariants: None
// Failure modes: None
class Store {
public:
    virtual ~Store() = default;

    // Thread-safety: Not thread-safe (modifies customer list)
    // Ownership: Borrows customer pointer (does not own)
    // Invariants: customer must not be null
    // Failure modes: Undefined behavior if customer is null
    virtual void addCustomer(Customer* customer) = 0;

    // Thread-safety: Not thread-safe (modifies customer list)
    // Ownership: Borrows customer pointer (does not own)
    // Invariants: customer must not be null
    // Failure modes: Undefined behavior if customer is null
    virtual void removeCustomer(Customer* customer) = 0;

    // Thread-safety: Not thread-safe (calls customer methods)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    virtual void notifyCustomers() = 0;

    // Thread-safety: Not thread-safe (modifies stock and calls notifyCustomers)
    // Ownership: None
    // Invariants: quantity >= 0
    // Failure modes: Undefined behavior if quantity < 0
    virtual void updateQuantity(int quantity) = 0;
};

// Thread-safety: Not thread-safe (abstract interface)
// Ownership: Abstract base class, does not own derived objects
// Invariants: None
// Failure modes: None
class Customer {
public:
    virtual ~Customer() = default;

    // Thread-safety: Not thread-safe (may modify state)
    // Ownership: None
    // Invariants: stockQuantity >= 0
    // Failure modes: Undefined behavior if stockQuantity < 0
    virtual void update(int stockQuantity) = 0;
};

// Thread-safety: Not thread-safe (mutable state, no synchronization)
// Ownership: Owns no resources (stores raw pointers, does not own)
// Invariants: stockQuantity >= 0, customers contains valid pointers
// Failure modes: Undefined behavior if invariants violated
class BookStore : public Store {
private:
    std::vector<Customer*> customers_;
    int stockQuantity_;

public:
    // Thread-safety: Not thread-safe (constructor modifies object)
    // Ownership: Initializes stockQuantity_ to 0
    // Invariants: None
    // Failure modes: None
    BookStore() : stockQuantity_(0) {}

    // Thread-safety: Not thread-safe (modifies customers_)
    // Ownership: Borrows customer pointer (does not own)
    // Invariants: customer must not be null
    // Failure modes: Undefined behavior if customer is null
    void addCustomer(Customer* customer) override {
        assert(customer != nullptr && "Customer must not be null");
        customers_.push_back(customer);
    }

    // Thread-safety: Not thread-safe (modifies customers_)
    // Ownership: Borrows customer pointer (does not own)
    // Invariants: customer must not be null
    // Failure modes: Undefined behavior if customer is null
    void removeCustomer(Customer* customer) override {
        assert(customer != nullptr && "Customer must not be null");
        customers_.erase(
            std::remove(customers_.begin(), customers_.end(), customer),
            customers_.end());
    }

    // Thread-safety: Not thread-safe (calls customer methods)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void notifyCustomers() override {
        for (Customer* customer : customers_) {
            customer->update(stockQuantity_);
        }
    }

    // Thread-safety: Not thread-safe (modifies stockQuantity_ and calls notifyCustomers)
    // Ownership: None
    // Invariants: quantity >= 0
    // Failure modes: Undefined behavior if quantity < 0
    void updateQuantity(int quantity) override {
        assert(quantity >= 0 && "Quantity must be non-negative");
        stockQuantity_ = quantity;
        notifyCustomers();
    }
};

// Thread-safety: Not thread-safe (mutable state)
// Ownership: Owns no resources (stores raw pointer to Store, does not own)
// Invariants: store must be valid
// Failure modes: Undefined behavior if store is invalid
class BookCustomer : public Customer {
private:
    int observedStockQuantity_;
    Store* store_;

public:
    // Thread-safety: Not thread-safe (constructor registers customer)
    // Ownership: Borrows store pointer (does not own)
    // Invariants: store must not be null
    // Failure modes: Undefined behavior if store is null
    explicit BookCustomer(Store* store) : observedStockQuantity_(0), store_(store) {
        assert(store != nullptr && "Store must not be null");
        store_->addCustomer(this);
    }

    // Thread-safety: Not thread-safe (modifies observedStockQuantity_)
    // Ownership: None
    // Invariants: stockQuantity >= 0
    // Failure modes: Undefined behavior if stockQuantity < 0
    void update(int stockQuantity) override {
        assert(stockQuantity >= 0 && "Stock quantity must be non-negative");
        observedStockQuantity_ = stockQuantity;
        if (stockQuantity > 0) {
            std::cout << "Hello, A book you are interested in is back in stock!" << std::endl;
        }
    }
};

int main() {
    auto store = std::make_unique<BookStore>();

    auto customer1 = std::make_unique<BookCustomer>(store.get());
    auto customer2 = std::make_unique<BookCustomer>(store.get());

    std::cout << "Setting stock to 0." << std::endl;
    store->updateQuantity(0);

    std::cout << "Setting stock to 5." << std::endl;
    store->updateQuantity(5);

    store->removeCustomer(customer1.get());

    std::cout << "\nSetting stock to 2." << std::endl;
    store->updateQuantity(2);

    return 0;
}
