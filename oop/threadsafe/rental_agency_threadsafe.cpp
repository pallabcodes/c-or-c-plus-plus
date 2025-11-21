/*
 * Object-Oriented Programming: Thread-Safe Rental Agency
 *
 * Demonstrates thread-safe implementation of the RentalAgency class
 * with proper synchronization for concurrent access.
 */
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <cassert>

// Thread-safety: Thread-safe (immutable after construction)
// Ownership: Owns no resources
// Invariants: make_ and model_ must be non-empty, year_ > 0
// Failure modes: Undefined behavior if invariants violated
class Vehicle {
protected:
    std::string make_;
    std::string model_;
    int year_;

public:
    Vehicle(const std::string& make, const std::string& model, int year)
        : make_(make), model_(model), year_(year) {
        assert(!make.empty() && !model.empty() && "Make and model must be non-empty");
        assert(year > 0 && "Year must be positive");
    }

    virtual void displayInfo() const = 0;
    virtual double calculateRentalCost() const = 0;

    virtual ~Vehicle() = default;
};

class Car : public Vehicle {
private:
    int numOfDoors_;

public:
    Car(const std::string& make, const std::string& model, int year, int doors)
        : Vehicle(make, model, year), numOfDoors_(doors) {
        assert(doors > 0 && "Number of doors must be positive");
    }

    void displayInfo() const override {
        std::cout << year_ << " " << make_ << " " << model_
                  << " (Car, " << numOfDoors_ << " doors)" << std::endl;
    }

    double calculateRentalCost() const override {
        return 50.0 * numOfDoors_;
    }
};

class Motorcycle : public Vehicle {
private:
    bool hasSidecar_;

public:
    Motorcycle(const std::string& make, const std::string& model, int year, bool sidecar)
        : Vehicle(make, model, year), hasSidecar_(sidecar) {}

    void displayInfo() const override {
        std::cout << year_ << " " << make_ << " " << model_ << " (Motorcycle, "
                  << (hasSidecar_ ? "with" : "without") << " sidecar)" << std::endl;
    }

    double calculateRentalCost() const override {
        return hasSidecar_ ? 80.0 : 60.0;
    }
};

// Thread-safety: Thread-safe (uses shared_mutex for read-write lock)
// Ownership: Owns Vehicle objects via unique_ptr
// Invariants: inventory_ contains valid Vehicle pointers
// Failure modes: Undefined behavior if vehicle is null
class ThreadSafeRentalAgency {
private:
    mutable std::shared_mutex mutex_;  // Mutable for const methods
    std::vector<std::unique_ptr<Vehicle>> inventory_;

public:
    // Thread-safety: Thread-safe (locks mutex exclusively)
    // Ownership: Takes ownership of vehicle (transfers to unique_ptr)
    // Invariants: vehicle must not be null
    // Failure modes: Undefined behavior if vehicle is null
    void addVehicle(std::unique_ptr<Vehicle> vehicle) {
        assert(vehicle != nullptr && "Vehicle must not be null");
        std::unique_lock<std::shared_mutex> lock(mutex_);
        inventory_.push_back(std::move(vehicle));
    }

    // Thread-safety: Thread-safe (locks mutex for shared read access)
    // Ownership: None (read-only operation)
    // Invariants: None
    // Failure modes: None
    void displayInventory() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        for (const auto& vehicle : inventory_) {
            vehicle->displayInfo();
        }
    }

    // Thread-safety: Thread-safe (locks mutex for shared read access)
    // Ownership: None (read-only operation)
    // Invariants: None
    // Failure modes: None
    double calculateTotalRentalCost() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        double total = 0.0;
        for (const auto& vehicle : inventory_) {
            total += vehicle->calculateRentalCost();
        }
        return total;
    }

    // Thread-safety: Thread-safe (locks mutex for shared read access)
    // Ownership: None (read-only operation)
    // Invariants: None
    // Failure modes: None
    size_t getInventorySize() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return inventory_.size();
    }

    // Thread-safety: Thread-safe (locks mutex exclusively)
    // Ownership: None
    // Invariants: index must be valid
    // Failure modes: Returns nullptr if index is invalid
    std::unique_ptr<Vehicle> removeVehicle(size_t index) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        if (index >= inventory_.size()) {
            return nullptr;
        }
        auto it = inventory_.begin() + index;
        std::unique_ptr<Vehicle> vehicle = std::move(*it);
        inventory_.erase(it);
        return vehicle;
    }
};

void addVehiclesThread(ThreadSafeRentalAgency& agency, int threadId) {
    for (int i = 0; i < 3; ++i) {
        std::string make = "Make_" + std::to_string(threadId) + "_" + std::to_string(i);
        auto vehicle = std::make_unique<Car>(make, "Model", 2020 + i, 4);
        agency.addVehicle(std::move(vehicle));
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void readInventoryThread(const ThreadSafeRentalAgency& agency, int threadId) {
    for (int i = 0; i < 5; ++i) {
        std::cout << "[Reader Thread " << threadId << "] Inventory size: "
                  << agency.getInventorySize() << std::endl;
        double total = agency.calculateTotalRentalCost();
        std::cout << "[Reader Thread " << threadId << "] Total cost: $" << total << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main() {
    ThreadSafeRentalAgency agency;

    // Add some initial vehicles
    agency.addVehicle(std::make_unique<Car>("Toyota", "Camry", 2022, 4));
    agency.addVehicle(std::make_unique<Motorcycle>("Harley", "Street", 2021, false));

    // Create writer threads
    std::vector<std::thread> writers;
    for (int i = 0; i < 3; ++i) {
        writers.emplace_back(addVehiclesThread, std::ref(agency), i);
    }

    // Create reader threads (can read concurrently)
    std::vector<std::thread> readers;
    for (int i = 0; i < 2; ++i) {
        readers.emplace_back(readInventoryThread, std::cref(agency), i);
    }

    // Wait for all threads
    for (auto& t : writers) {
        t.join();
    }
    for (auto& t : readers) {
        t.join();
    }

    std::cout << "\nFinal inventory:" << std::endl;
    agency.displayInventory();
    std::cout << "Final total cost: $" << agency.calculateTotalRentalCost() << std::endl;

    return 0;
}

