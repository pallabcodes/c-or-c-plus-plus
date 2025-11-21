/*
 * Creational Pattern: Thread-Safe Builder
 *
 * Demonstrates thread-safe Builder pattern implementation with proper
 * synchronization for concurrent meal construction.
 */
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

enum class Starter {
    SALAD,
    SOUP,
    BRUSCHETTA,
};

enum class Main {
    GRILLED_CHICKEN,
    PASTA,
    VEGGIE_STIR_FRY,
};

enum class Dessert {
    FRUIT_SALAD,
    ICE_CREAM,
    CHOCOLATE_CAKE,
};

enum class Drink {
    WATER,
    SODA,
    FRUIT_JUICE,
};

// Thread-safety: Thread-safe (immutable after construction)
// Ownership: Owns no resources
// Invariants: None
// Failure modes: None
class Meal {
private:
    Starter starter_;
    Main main_;
    Dessert dessert_;
    Drink drink_;

public:
    Starter getStarter() const { return starter_; }
    Main getMain() const { return main_; }
    Dessert getDessert() const { return dessert_; }
    Drink getDrink() const { return drink_; }

    void setStarter(Starter s) { starter_ = s; }
    void setMain(Main m) { main_ = m; }
    void setDessert(Dessert d) { dessert_ = d; }
    void setDrink(Drink d) { drink_ = d; }
};

// Thread-safety: Thread-safe (abstract interface)
// Ownership: Abstract base class, does not own derived objects
// Invariants: None
// Failure modes: None
class ThreadSafeBuilder {
public:
    virtual ~ThreadSafeBuilder() = default;

    // Thread-safety: Thread-safe (must be implemented thread-safely)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    virtual void addStarter() = 0;

    // Thread-safety: Thread-safe (must be implemented thread-safely)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    virtual void addMainCourse() = 0;

    // Thread-safety: Thread-safe (must be implemented thread-safely)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    virtual void addDessert() = 0;

    // Thread-safety: Thread-safe (must be implemented thread-safely)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    virtual void addDrink() = 0;

    // Thread-safety: Thread-safe (must be implemented thread-safely)
    // Ownership: Returns Meal by value
    // Invariants: None
    // Failure modes: None
    virtual Meal build() = 0;
};

// Thread-safety: Thread-safe (uses mutex for state protection)
// Ownership: Owns Meal object
// Invariants: None
// Failure modes: None
class ThreadSafeVeganMealBuilder : public ThreadSafeBuilder {
private:
    mutable std::mutex mutex_;
    Meal meal_;

public:
    // Thread-safety: Thread-safe (locks mutex)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void addStarter() override {
        std::lock_guard<std::mutex> lock(mutex_);
        meal_.setStarter(Starter::SALAD);
    }

    // Thread-safety: Thread-safe (locks mutex)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void addMainCourse() override {
        std::lock_guard<std::mutex> lock(mutex_);
        meal_.setMain(Main::VEGGIE_STIR_FRY);
    }

    // Thread-safety: Thread-safe (locks mutex)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void addDessert() override {
        std::lock_guard<std::mutex> lock(mutex_);
        meal_.setDessert(Dessert::FRUIT_SALAD);
    }

    // Thread-safety: Thread-safe (locks mutex)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void addDrink() override {
        std::lock_guard<std::mutex> lock(mutex_);
        meal_.setDrink(Drink::WATER);
    }

    // Thread-safety: Thread-safe (locks mutex, returns copy)
    // Ownership: Returns Meal by value
    // Invariants: None
    // Failure modes: None
    Meal build() override {
        std::lock_guard<std::mutex> lock(mutex_);
        return meal_;
    }
};

// Thread-safety: Thread-safe (uses mutex for operations)
// Ownership: Borrows builder reference
// Invariants: builder must be valid
// Failure modes: Undefined behavior if builder is invalid
class ThreadSafeDirector {
private:
    mutable std::mutex mutex_;

public:
    // Thread-safety: Thread-safe (locks mutex)
    // Ownership: Borrows builder reference
    // Invariants: builder must be valid
    // Failure modes: Undefined behavior if builder is invalid
    void constructVeganMeal(ThreadSafeBuilder& builder) {
        std::lock_guard<std::mutex> lock(mutex_);
        builder.addStarter();
        builder.addMainCourse();
        builder.addDessert();
        builder.addDrink();
    }

    // Thread-safety: Thread-safe (locks mutex)
    // Ownership: Borrows builder reference
    // Invariants: builder must be valid
    // Failure modes: Undefined behavior if builder is invalid
    void constructHealthyMeal(ThreadSafeBuilder& builder) {
        std::lock_guard<std::mutex> lock(mutex_);
        builder.addStarter();
        builder.addMainCourse();
        builder.addDessert();
        builder.addDrink();
    }
};

void buildMealThread(ThreadSafeDirector& director, ThreadSafeBuilder& builder, int threadId) {
    director.constructVeganMeal(builder);
    Meal meal = builder.build();
    std::cout << "[Thread " << threadId << "] Meal constructed" << std::endl;
}

int main() {
    ThreadSafeDirector director;
    ThreadSafeVeganMealBuilder builder;

    std::vector<std::thread> threads;
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back(buildMealThread, std::ref(director), std::ref(builder), i);
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}

