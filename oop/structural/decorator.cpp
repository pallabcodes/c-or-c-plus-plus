/*
 * Structural Pattern: Decorator
 *
 * Demonstrates the Decorator pattern for adding behavior to objects dynamically
 * by wrapping them with decorator objects.
 */
#include <iostream>
#include <string>
#include <memory>
#include <cassert>

// Thread-safety: Not thread-safe (abstract interface)
// Ownership: Abstract base class, does not own derived objects
// Invariants: None
// Failure modes: None
class Beverage {
public:
    virtual ~Beverage() = default;

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

// Thread-safety: Thread-safe (immutable state)
// Ownership: Owns no resources
// Invariants: None
// Failure modes: None
class DarkRoast : public Beverage {
public:
    // Thread-safety: Thread-safe (const method, pure calculation)
    // Ownership: None (pure calculation)
    // Invariants: None
    // Failure modes: None
    double cost() const override {
        return 3.45;
    }

    // Thread-safety: Thread-safe (const method, returns copy)
    // Ownership: Returns copy of string literal
    // Invariants: None
    // Failure modes: None
    std::string description() const override {
        return "Dark Roast";
    }
};

// Thread-safety: Thread-safe (immutable state)
// Ownership: Owns no resources
// Invariants: None
// Failure modes: None
class LightRoast : public Beverage {
public:
    // Thread-safety: Thread-safe (const method, pure calculation)
    // Ownership: None (pure calculation)
    // Invariants: None
    // Failure modes: None
    double cost() const override {
        return 3.45;
    }

    // Thread-safety: Thread-safe (const method, returns copy)
    // Ownership: Returns copy of string literal
    // Invariants: None
    // Failure modes: None
    std::string description() const override {
        return "Light Roast";
    }
};

// Thread-safety: Not thread-safe (mutable state)
// Ownership: Owns wrapped Beverage via unique_ptr
// Invariants: beverage_ must not be null
// Failure modes: Undefined behavior if beverage_ is null
class BeverageDecorator : public Beverage {
protected:
    std::unique_ptr<Beverage> beverage_;

public:
    // Thread-safety: Not thread-safe (constructor takes ownership)
    // Ownership: Takes ownership of beverage (transfers to unique_ptr)
    // Invariants: beverage must not be null
    // Failure modes: Undefined behavior if beverage is null
    explicit BeverageDecorator(std::unique_ptr<Beverage> beverage)
        : beverage_(std::move(beverage)) {
        assert(beverage_ != nullptr && "Beverage must not be null");
    }

    virtual ~BeverageDecorator() = default;
};

// Thread-safety: Not thread-safe (mutable state)
// Ownership: Owns wrapped Beverage via BeverageDecorator
// Invariants: Inherits BeverageDecorator invariants
// Failure modes: Inherits BeverageDecorator failure modes
class EspressoDecorator : public BeverageDecorator {
public:
    // Thread-safety: Not thread-safe (constructor takes ownership)
    // Ownership: Takes ownership of beverage (transfers to unique_ptr)
    // Invariants: beverage must not be null
    // Failure modes: Undefined behavior if beverage is null
    explicit EspressoDecorator(std::unique_ptr<Beverage> beverage)
        : BeverageDecorator(std::move(beverage)) {}

    // Thread-safety: Thread-safe (const method, calls const methods)
    // Ownership: None (pure calculation)
    // Invariants: None
    // Failure modes: None
    double cost() const override {
        return 0.5 + beverage_->cost();
    }

    // Thread-safety: Thread-safe (const method, returns copy)
    // Ownership: Returns copy of string
    // Invariants: None
    // Failure modes: None
    std::string description() const override {
        return beverage_->description() + ", Espresso";
    }
};

// Thread-safety: Not thread-safe (mutable state)
// Ownership: Owns wrapped Beverage via BeverageDecorator
// Invariants: Inherits BeverageDecorator invariants
// Failure modes: Inherits BeverageDecorator failure modes
class CreamDecorator : public BeverageDecorator {
public:
    // Thread-safety: Not thread-safe (constructor takes ownership)
    // Ownership: Takes ownership of beverage (transfers to unique_ptr)
    // Invariants: beverage must not be null
    // Failure modes: Undefined behavior if beverage is null
    explicit CreamDecorator(std::unique_ptr<Beverage> beverage)
        : BeverageDecorator(std::move(beverage)) {}

    // Thread-safety: Thread-safe (const method, calls const methods)
    // Ownership: None (pure calculation)
    // Invariants: None
    // Failure modes: None
    double cost() const override {
        return 0.3 + beverage_->cost();
    }

    // Thread-safety: Thread-safe (const method, returns copy)
    // Ownership: Returns copy of string
    // Invariants: None
    // Failure modes: None
    std::string description() const override {
        return beverage_->description() + ", Cream";
    }
};

// Thread-safety: Not thread-safe (mutable state)
// Ownership: Owns wrapped Beverage via BeverageDecorator
// Invariants: Inherits BeverageDecorator invariants
// Failure modes: Inherits BeverageDecorator failure modes
class FoamDecorator : public BeverageDecorator {
public:
    // Thread-safety: Not thread-safe (constructor takes ownership)
    // Ownership: Takes ownership of beverage (transfers to unique_ptr)
    // Invariants: beverage must not be null
    // Failure modes: Undefined behavior if beverage is null
    explicit FoamDecorator(std::unique_ptr<Beverage> beverage)
        : BeverageDecorator(std::move(beverage)) {}

    // Thread-safety: Thread-safe (const method, calls const methods)
    // Ownership: None (pure calculation)
    // Invariants: None
    // Failure modes: None
    double cost() const override {
        return 0.2 + beverage_->cost();
    }

    // Thread-safety: Thread-safe (const method, returns copy)
    // Ownership: Returns copy of string
    // Invariants: None
    // Failure modes: None
    std::string description() const override {
        return beverage_->description() + ", Foam";
    }
};

int main() {
    // Build decorator chain using smart pointers
    auto beverage = std::make_unique<FoamDecorator>(
        std::make_unique<CreamDecorator>(
            std::make_unique<EspressoDecorator>(
                std::make_unique<LightRoast>())));

    std::cout << beverage->description() << std::endl;
    std::cout << beverage->cost() << std::endl;

    return 0;
}
