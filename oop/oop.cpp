/*
 * Object-Oriented Programming: Fundamentals
 *
 * Demonstrates inheritance, polymorphism, encapsulation, and abstract classes
 * using a vehicle rental system example.
 */
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cassert>

// Thread-safety: Not thread-safe (mutable state)
// Ownership: Abstract base class, does not own derived objects
// Invariants: make and model must be non-empty, year > 0
// Failure modes: Undefined behavior if invariants violated
class Vehicle {
protected:
    std::string make_;
    std::string model_;
    int year_;

public:
    // Thread-safety: Not thread-safe (constructor modifies object)
    // Ownership: Takes ownership of string parameters (copies)
    // Invariants: make and model must be non-empty, year > 0
    // Failure modes: Throws if year <= 0
    Vehicle(const std::string& make, const std::string& model, int year)
        : make_(make), model_(model), year_(year) {
        assert(!make.empty() && !model.empty() && "Make and model must be non-empty");
        assert(year > 0 && "Year must be positive");
    }

    virtual void displayInfo() const = 0;
    virtual double calculateRentalCost() const = 0;

    virtual ~Vehicle() = default;
};

// Thread-safety: Not thread-safe (mutable state)
// Ownership: Owns no resources
// Invariants: numOfDoors > 0
// Failure modes: Undefined behavior if numOfDoors <= 0
class Car : public Vehicle {
private:
    int numOfDoors_;

public:
    // Thread-safety: Not thread-safe (constructor modifies object)
    // Ownership: Takes ownership of string parameters (copies)
    // Invariants: doors > 0
    // Failure modes: Undefined behavior if doors <= 0
    Car(const std::string& make, const std::string& model, int year, int doors)
        : Vehicle(make, model, year), numOfDoors_(doors) {
        assert(doors > 0 && "Number of doors must be positive");
    }

    // Thread-safety: Thread-safe (const method, no shared state)
    // Ownership: None (read-only operation)
    // Invariants: None
    // Failure modes: None
    void displayInfo() const override {
        std::cout << year_ << " " << make_ << " " << model_
                  << " (Car, " << numOfDoors_ << " doors)" << std::endl;
    }

    // Thread-safety: Thread-safe (const method, no shared state)
    // Ownership: None (pure calculation)
    // Invariants: None
    // Failure modes: None
    double calculateRentalCost() const override {
        return 50.0 * numOfDoors_;
    }
};

// Thread-safety: Not thread-safe (mutable state)
// Ownership: Owns no resources
// Invariants: None
// Failure modes: None
class Motorcycle : public Vehicle {
private:
    bool hasSidecar_;

public:
    // Thread-safety: Not thread-safe (constructor modifies object)
    // Ownership: Takes ownership of string parameters (copies)
    // Invariants: None
    // Failure modes: None
    Motorcycle(const std::string& make, const std::string& model, int year, bool sidecar)
        : Vehicle(make, model, year), hasSidecar_(sidecar) {}

    // Thread-safety: Thread-safe (const method, no shared state)
    // Ownership: None (read-only operation)
    // Invariants: None
    // Failure modes: None
    void displayInfo() const override {
        std::cout << year_ << " " << make_ << " " << model_ << " (Motorcycle, "
                  << (hasSidecar_ ? "with" : "without") << " sidecar)" << std::endl;
    }

    // Thread-safety: Thread-safe (const method, no shared state)
    // Ownership: None (pure calculation)
    // Invariants: None
    // Failure modes: None
    double calculateRentalCost() const override {
        return hasSidecar_ ? 80.0 : 60.0;
    }
};

// Thread-safety: Not thread-safe (mutable state, no synchronization)
// Ownership: Owns Vehicle objects via unique_ptr
// Invariants: inventory contains valid Vehicle pointers
// Failure modes: Undefined behavior if vehicle is null
class RentalAgency {
private:
    std::vector<std::unique_ptr<Vehicle>> inventory_;

public:
    // Thread-safety: Not thread-safe (modifies inventory_)
    // Ownership: Takes ownership of vehicle (transfers to unique_ptr)
    // Invariants: vehicle must not be null
    // Failure modes: Undefined behavior if vehicle is null
    void addVehicle(std::unique_ptr<Vehicle> vehicle) {
        assert(vehicle != nullptr && "Vehicle must not be null");
        inventory_.push_back(std::move(vehicle));
    }

    // Thread-safety: Thread-safe (const method, but Vehicle methods may not be)
    // Ownership: None (read-only operation)
    // Invariants: None
    // Failure modes: None
    void displayInventory() const {
        for (const auto& vehicle : inventory_) {
            vehicle->displayInfo();
        }
    }

    // Thread-safety: Thread-safe (const method, but Vehicle methods may not be)
    // Ownership: None (read-only operation)
    // Invariants: None
    // Failure modes: None
    double calculateTotalRentalCost() const {
        double total = 0.0;
        for (const auto& vehicle : inventory_) {
            total += vehicle->calculateRentalCost();
        }
        return total;
    }
};

// Thread-safety: Not thread-safe (abstract interface)
// Ownership: Abstract base class, does not own derived objects
// Invariants: None
// Failure modes: None
class AbstractEmployee {
public:
    virtual void askForPermission() = 0;
    virtual ~AbstractEmployee() = default;
};

// Thread-safety: Not thread-safe (mutable state)
// Ownership: Owns string members
// Invariants: age > 0, rollNo > 0
// Failure modes: Undefined behavior if invariants violated
class Student : public AbstractEmployee {
private:
    std::string name_;
    std::string address_;
    int rollNo_;
    std::string dept_;
    int age_;

public:
    // Thread-safety: Not thread-safe (constructor modifies object)
    // Ownership: Takes ownership of string parameters (copies)
    // Invariants: age > 0, rollNo > 0
    // Failure modes: Undefined behavior if age <= 0 or rollNo <= 0
    Student(const std::string& name, const std::string& address, int rollNo,
            const std::string& dept, int age)
        : name_(name), address_(address), rollNo_(rollNo), dept_(dept), age_(age) {
        assert(age > 0 && "Age must be positive");
        assert(rollNo > 0 && "Roll number must be positive");
    }

    // Thread-safety: Not thread-safe (may modify state via output)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void askForPermission() override {
        if (age_ > 30) {
            std::cout << "Getting promoted" << std::endl;
        } else {
            std::cout << "Negotiating" << std::endl;
        }
    }

    // Thread-safety: Not thread-safe (may modify state via output)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void introduceYourself() const {
        std::cout << "Hello, My name is " << name_ << std::endl;
    }

    // Thread-safety: Not thread-safe (modifies name_)
    // Ownership: Takes ownership of name parameter (copies)
    // Invariants: name must be non-empty
    // Failure modes: Undefined behavior if name is empty
    void setName(const std::string& name) {
        assert(!name.empty() && "Name must be non-empty");
        name_ = name;
    }

    // Thread-safety: Thread-safe (const method, returns copy)
    // Ownership: Returns copy of name
    // Invariants: None
    // Failure modes: None
    std::string getName() const {
        return name_;
    }
};

// Thread-safety: Not thread-safe (mutable state)
// Ownership: Owns string members
// Invariants: Inherits Student invariants
// Failure modes: Inherits Student failure modes
class Developer : public Student {
private:
    std::string favoriteProgrammingLang_;

public:
    // Thread-safety: Not thread-safe (constructor modifies object)
    // Ownership: Takes ownership of string parameters (copies)
    // Invariants: Inherits Student invariants
    // Failure modes: Inherits Student failure modes
    Developer(const std::string& name, const std::string& address, int rollNo,
              const std::string& dept, const std::string& favoriteProgrammingLang, int age)
        : Student(name, address, rollNo, dept, age),
          favoriteProgrammingLang_(favoriteProgrammingLang) {}

    // Thread-safety: Not thread-safe (may modify state via output)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void fixBug() const {
        std::cout << getName() << " fixed the bug using " << favoriteProgrammingLang_ << std::endl;
    }
};

int main() {
    // Creating instances of Student
    Student student1("John", "Boston", 30, "Wrestling", 29);
    Student student2("Jose", "Madrid", 20, "Football", 25);

    // Demonstrating functionality
    student1.askForPermission();
    student2.introduceYourself();

    // Creating an instance of Developer
    Developer developer("Johnson", "UK", 40, "Engineering", "C++", 35);
    developer.fixBug();

    RentalAgency agency;

    // Adding vehicles to the inventory using smart pointers
    agency.addVehicle(std::make_unique<Car>("Toyota", "Camry", 2022, 4));
    agency.addVehicle(std::make_unique<Car>("Honda", "Civic", 2023, 2));
    agency.addVehicle(std::make_unique<Motorcycle>("Harley-Davidson", "Street 750", 2021, false));
    agency.addVehicle(std::make_unique<Motorcycle>("BMW", "R1250GS", 2023, true));

    std::cout << "Rental Agency Inventory:" << std::endl;
    agency.displayInventory();

    std::cout << "\nTotal rental cost for all vehicles: $"
              << agency.calculateTotalRentalCost() << std::endl;

    return 0;
}