/*
 * Creational Pattern: Builder
 *
 * Demonstrates the Builder pattern for constructing complex objects step by step.
 */
#include <iostream>
#include <memory>

enum class Starter {
    SALAD,
    SOUP,
    BRUSCHETTA,
    VEGGIE_STICKS,
    CHICKEN_WINGS,
};

enum class Main {
    GRILLED_CHICKEN,
    PASTA,
    VEGGIE_STIR_FRY,
    FISH,
    PIZZA,
};

enum class Dessert {
    FRUIT_SALAD,
    ICE_CREAM,
    CHOCOLATE_CAKE,
    VEGAN_PUDDING,
    CHEESECAKE,
};

enum class Drink {
    WATER,
    VEGAN_SHAKE,
    SODA,
    FRUIT_JUICE,
};

// Thread-safety: Not thread-safe (mutable state)
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
    // Thread-safety: Thread-safe (const method, returns copy)
    // Ownership: Returns copy of enum value
    // Invariants: None
    // Failure modes: None
    Starter getStarter() const {
        return starter_;
    }

    // Thread-safety: Thread-safe (const method, returns copy)
    // Ownership: Returns copy of enum value
    // Invariants: None
    // Failure modes: None
    Main getMain() const {
        return main_;
    }

    // Thread-safety: Thread-safe (const method, returns copy)
    // Ownership: Returns copy of enum value
    // Invariants: None
    // Failure modes: None
    Dessert getDessert() const {
        return dessert_;
    }

    // Thread-safety: Thread-safe (const method, returns copy)
    // Ownership: Returns copy of enum value
    // Invariants: None
    // Failure modes: None
    Drink getDrink() const {
        return drink_;
    }

    // Thread-safety: Not thread-safe (modifies starter_)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void setStarter(Starter s) {
        starter_ = s;
    }

    // Thread-safety: Not thread-safe (modifies main_)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void setMain(Main m) {
        main_ = m;
    }

    // Thread-safety: Not thread-safe (modifies dessert_)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void setDessert(Dessert d) {
        dessert_ = d;
    }

    // Thread-safety: Not thread-safe (modifies drink_)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void setDrink(Drink d) {
        drink_ = d;
    }
};

// Thread-safety: Not thread-safe (abstract interface)
// Ownership: Abstract base class, does not own derived objects
// Invariants: None
// Failure modes: None
class Builder {
public:
    virtual ~Builder() = default;

    // Thread-safety: Not thread-safe (modifies builder state)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    virtual void addStarter() = 0;

    // Thread-safety: Not thread-safe (modifies builder state)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    virtual void addMainCourse() = 0;

    // Thread-safety: Not thread-safe (modifies builder state)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    virtual void addDessert() = 0;

    // Thread-safety: Not thread-safe (modifies builder state)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    virtual void addDrink() = 0;

    // Thread-safety: Not thread-safe (returns built object)
    // Ownership: Returns Meal by value
    // Invariants: None
    // Failure modes: None
    virtual Meal build() = 0;
};

// Thread-safety: Not thread-safe (mutable state)
// Ownership: Owns Meal object
// Invariants: None
// Failure modes: None
class VeganMealBuilder : public Builder {
private:
    Meal meal_;

public:
    // Thread-safety: Not thread-safe (modifies meal_)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void addStarter() override {
        meal_.setStarter(Starter::SALAD);
    }

    // Thread-safety: Not thread-safe (modifies meal_)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void addMainCourse() override {
        meal_.setMain(Main::VEGGIE_STIR_FRY);
    }

    // Thread-safety: Not thread-safe (modifies meal_)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void addDessert() override {
        meal_.setDessert(Dessert::VEGAN_PUDDING);
    }

    // Thread-safety: Not thread-safe (modifies meal_)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void addDrink() override {
        meal_.setDrink(Drink::VEGAN_SHAKE);
    }

    // Thread-safety: Not thread-safe (returns copy of meal_)
    // Ownership: Returns Meal by value
    // Invariants: None
    // Failure modes: None
    Meal build() override {
        return meal_;
    }
};

// Thread-safety: Not thread-safe (mutable state)
// Ownership: Owns Meal object
// Invariants: None
// Failure modes: None
class HealthyMealBuilder : public Builder {
private:
    Meal meal_;

public:
    // Thread-safety: Not thread-safe (modifies meal_)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void addStarter() override {
        meal_.setStarter(Starter::SALAD);
    }

    // Thread-safety: Not thread-safe (modifies meal_)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void addMainCourse() override {
        meal_.setMain(Main::GRILLED_CHICKEN);
    }

    // Thread-safety: Not thread-safe (modifies meal_)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void addDessert() override {
        meal_.setDessert(Dessert::FRUIT_SALAD);
    }

    // Thread-safety: Not thread-safe (modifies meal_)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void addDrink() override {
        meal_.setDrink(Drink::WATER);
    }

    // Thread-safety: Not thread-safe (returns copy of meal_)
    // Ownership: Returns Meal by value
    // Invariants: None
    // Failure modes: None
    Meal build() override {
        return meal_;
    }
};

// Thread-safety: Not thread-safe (modifies builder)
// Ownership: Borrows builder reference
// Invariants: builder must be valid
// Failure modes: Undefined behavior if builder is invalid
class Director {
public:
    // Thread-safety: Not thread-safe (modifies builder)
    // Ownership: Borrows builder reference
    // Invariants: builder must be valid
    // Failure modes: Undefined behavior if builder is invalid
    void constructVeganMeal(Builder& builder) {
        builder.addStarter();
        builder.addMainCourse();
        builder.addDessert();
        builder.addDrink();
    }

    // Thread-safety: Not thread-safe (modifies builder)
    // Ownership: Borrows builder reference
    // Invariants: builder must be valid
    // Failure modes: Undefined behavior if builder is invalid
    void constructHealthyMeal(Builder& builder) {
        builder.addStarter();
        builder.addMainCourse();
        builder.addDessert();
        builder.addDrink();
    }
};

int main() {
    Director director;
    VeganMealBuilder veganBuilder;
    director.constructVeganMeal(veganBuilder);

    Meal veganMeal = veganBuilder.build();
    std::cout << "Vegan Meal constructed." << std::endl;

    HealthyMealBuilder healthyBuilder;
    director.constructHealthyMeal(healthyBuilder);
    Meal healthyMeal = healthyBuilder.build();
    std::cout << "Healthy Meal constructed." << std::endl;

    return 0;
}
