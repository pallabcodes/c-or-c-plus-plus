# Creational Design Patterns Standards

## Overview
Creational patterns manage object creation, providing flexibility in how objects are created and initialized. This document defines standards for implementing production grade creational design patterns.

## Singleton Pattern

### Purpose
* **Single instance**: Ensure only one instance exists
* **Global access**: Provide global access point
* **Lazy initialization**: Initialize on first access
* **Rationale**: Singleton ensures single instance

### Implementation
* **Private constructor**: Prevent external instantiation
* **Static instance**: Static instance variable
* **Static getter**: Static method to get instance
* **Thread safety**: Use mutex for thread safety
* **Rationale**: Implementation ensures single instance

### Example Singleton
```cpp
class Logger {
private:
    static Logger* instance_;
    static std::mutex mutex_;

    Logger() = default;  // Private constructor

public:
    static Logger* getInstance() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!instance_) {
            instance_ = new Logger();
        }
        return instance_;
    }

    void log(const std::string& message) {
        // Logging implementation
    }
};
```

## Factory Pattern

### Purpose
* **Object creation**: Create objects without specifying exact class
* **Encapsulation**: Encapsulate object creation logic
* **Flexibility**: Support multiple product types
* **Rationale**: Factory encapsulates creation logic

### Implementation
* **Product interface**: Abstract product interface
* **Concrete products**: Concrete product implementations
* **Factory class**: Factory class with creation method
* **Rationale**: Implementation enables flexible creation

### Example Factory
```cpp
class Burger {
public:
    virtual ~Burger() = default;
    virtual void prepare() = 0;
};

class CheeseBurger : public Burger {
public:
    void prepare() override {
        // Preparation logic
    }
};

class BurgerFactory {
public:
    static std::unique_ptr<Burger> createBurger(const std::string& type) {
        if (type == "cheese") {
            return std::make_unique<CheeseBurger>();
        }
        // Other burger types
        return nullptr;
    }
};
```

## Builder Pattern

### Purpose
* **Step by step**: Construct objects step by step
* **Complex objects**: Build complex objects
* **Flexibility**: Support different representations
* **Rationale**: Builder enables step by step construction

### Implementation
* **Builder class**: Builder class with construction methods
* **Director**: Optional director class
* **Product**: Product class to build
* **Rationale**: Implementation enables flexible construction

### Example Builder
```cpp
class Pizza {
private:
    std::string dough_;
    std::string sauce_;
    std::vector<std::string> toppings_;

public:
    void setDough(const std::string& dough) { dough_ = dough; }
    void setSauce(const std::string& sauce) { sauce_ = sauce; }
    void addTopping(const std::string& topping) {
        toppings_.push_back(topping);
    }
};

class PizzaBuilder {
private:
    Pizza pizza_;

public:
    PizzaBuilder& setDough(const std::string& dough) {
        pizza_.setDough(dough);
        return *this;
    }

    Pizza build() {
        return pizza_;
    }
};
```

## Implementation Standards

### Correctness
* **Thread safety**: Ensure thread safety for singletons
* **Resource management**: Proper resource management
* **Exception safety**: Maintain exception safety
* **Rationale**: Correctness is critical

### Performance
* **Lazy initialization**: Use lazy initialization when appropriate
* **Object pooling**: Consider object pooling for frequent creation
* **Rationale**: Performance is critical

## Testing Requirements

### Unit Tests
* **Singleton**: Test singleton instance uniqueness
* **Factory**: Test factory creation
* **Builder**: Test builder construction
* **Thread safety**: Test thread safety
* **Rationale**: Comprehensive testing ensures correctness

## Research Papers and References

### Creational Patterns
* "Design Patterns: Elements of Reusable Object Oriented Software" (Gang of Four)
* Creational pattern catalogs
* Pattern implementation guides

## Implementation Checklist

- [ ] Understand singleton pattern
- [ ] Understand factory pattern
- [ ] Understand builder pattern
- [ ] Implement creational patterns
- [ ] Add thread safety
- [ ] Write comprehensive unit tests
- [ ] Document pattern usage

