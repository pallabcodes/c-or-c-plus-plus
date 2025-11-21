/*
 * Creational Pattern: Factory Method
 *
 * Demonstrates the Factory Method pattern for creating burger objects
 * without specifying their exact classes.
 */
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <cassert>

enum class Burgers {
    CHEESE,
    DELUXECHEESE,
    VEGAN,
    DELUXEVEGAN
};

// Thread-safety: Not thread-safe (mutable state)
// Ownership: Abstract base class, does not own derived objects
// Invariants: name must be non-empty after construction
// Failure modes: Undefined behavior if name is empty
class Burger {
public:
    virtual ~Burger() = default;

    // Thread-safety: Not thread-safe (may modify state)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    virtual void prepare() {}

    // Thread-safety: Not thread-safe (may modify state)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    virtual void cook() {}

    // Thread-safety: Not thread-safe (may modify state)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    virtual void serve() {}

    // Thread-safety: Thread-safe (const method, returns copy)
    // Ownership: Returns copy of name
    // Invariants: None
    // Failure modes: None
    std::string getName() const {
        return name_;
    }

protected:
    std::string name_;
    std::string bread_;
    std::string sauce_;
    std::vector<std::string> toppings_;
};

// Thread-safety: Not thread-safe (mutable state)
// Ownership: Owns string members
// Invariants: name_ must be non-empty after construction
// Failure modes: Undefined behavior if name_ is empty
class CheeseBurger : public Burger {
public:
    // Thread-safety: Not thread-safe (constructor modifies object)
    // Ownership: Initializes string members
    // Invariants: name_ must be non-empty
    // Failure modes: None
    CheeseBurger() {
        name_ = "Cheese Burger";
        bread_ = "Sesame Bun";
        sauce_ = "Special Sauce";
    }
};

// Thread-safety: Not thread-safe (mutable state)
// Ownership: Owns string members
// Invariants: name_ must be non-empty after construction
// Failure modes: Undefined behavior if name_ is empty
class DeluxeCheeseBurger : public Burger {
public:
    // Thread-safety: Not thread-safe (constructor modifies object)
    // Ownership: Initializes string members
    // Invariants: name_ must be non-empty
    // Failure modes: None
    DeluxeCheeseBurger() {
        name_ = "Deluxe Cheese Burger";
        bread_ = "Artisan Bun";
        sauce_ = "Premium Sauce";
    }
};

// Thread-safety: Not thread-safe (mutable state)
// Ownership: Owns string members
// Invariants: name_ must be non-empty after construction
// Failure modes: Undefined behavior if name_ is empty
class VeganBurger : public Burger {
public:
    // Thread-safety: Not thread-safe (constructor modifies object)
    // Ownership: Initializes string members
    // Invariants: name_ must be non-empty
    // Failure modes: None
    VeganBurger() {
        name_ = "Vegan Burger";
        bread_ = "Whole Grain Bun";
        sauce_ = "Vegan Sauce";
    }
};

// Thread-safety: Not thread-safe (mutable state)
// Ownership: Owns string members
// Invariants: name_ must be non-empty after construction
// Failure modes: Undefined behavior if name_ is empty
class DeluxeVeganBurger : public Burger {
public:
    // Thread-safety: Not thread-safe (constructor modifies object)
    // Ownership: Initializes string members
    // Invariants: name_ must be non-empty
    // Failure modes: None
    DeluxeVeganBurger() {
        name_ = "Deluxe Vegan Burger";
        bread_ = "Artisan Whole Grain Bun";
        sauce_ = "Premium Vegan Sauce";
    }
};

// Thread-safety: Not thread-safe (abstract interface)
// Ownership: Abstract base class, does not own derived objects
// Invariants: None
// Failure modes: None
class BurgerStore {
public:
    virtual ~BurgerStore() = default;

    // Thread-safety: Not thread-safe (factory method)
    // Ownership: Returns unique_ptr owning Burger instance
    // Invariants: None
    // Failure modes: Returns nullptr if item type not supported
    virtual std::unique_ptr<Burger> createBurger(Burgers item) = 0;

    // Thread-safety: Not thread-safe (modifies state, calls virtual methods)
    // Ownership: Returns unique_ptr owning Burger instance
    // Invariants: type must be valid Burger type
    // Failure modes: Throws if createBurger returns nullptr
    std::unique_ptr<Burger> orderBurger(Burgers type) {
        auto burger = createBurger(type);
        if (!burger) {
            throw std::runtime_error("Failed to create burger: unsupported type");
        }
        std::cout << "--- Making a " << burger->getName() << " ---" << std::endl;
        burger->prepare();
        burger->cook();
        burger->serve();
        return burger;
    }
};

// Thread-safety: Not thread-safe (mutable state)
// Ownership: Owns no resources
// Invariants: None
// Failure modes: Returns nullptr for unsupported burger types
class CheeseBurgerStore : public BurgerStore {
public:
    // Thread-safety: Not thread-safe (factory method)
    // Ownership: Returns unique_ptr owning Burger instance
    // Invariants: None
    // Failure modes: Returns nullptr if item is not CHEESE or DELUXECHEESE
    std::unique_ptr<Burger> createBurger(Burgers item) override {
        if (item == Burgers::CHEESE) {
            return std::make_unique<CheeseBurger>();
        } else if (item == Burgers::DELUXECHEESE) {
            return std::make_unique<DeluxeCheeseBurger>();
        }
        return nullptr;
    }
};

// Thread-safety: Not thread-safe (mutable state)
// Ownership: Owns no resources
// Invariants: None
// Failure modes: Returns nullptr for unsupported burger types
class VeganBurgerStore : public BurgerStore {
public:
    // Thread-safety: Not thread-safe (factory method)
    // Ownership: Returns unique_ptr owning Burger instance
    // Invariants: None
    // Failure modes: Returns nullptr if item is not VEGAN or DELUXEVEGAN
    std::unique_ptr<Burger> createBurger(Burgers item) override {
        if (item == Burgers::VEGAN) {
            return std::make_unique<VeganBurger>();
        } else if (item == Burgers::DELUXEVEGAN) {
            return std::make_unique<DeluxeVeganBurger>();
        }
        return nullptr;
    }
};

int main() {
    try {
        auto cheeseStore = std::make_unique<CheeseBurgerStore>();
        auto veganStore = std::make_unique<VeganBurgerStore>();

        auto burger = cheeseStore->orderBurger(Burgers::CHEESE);
        std::cout << "Ethan ordered a " << burger->getName() << std::endl;

        burger = veganStore->orderBurger(Burgers::DELUXEVEGAN);
        std::cout << "Joel ordered a " << burger->getName() << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
