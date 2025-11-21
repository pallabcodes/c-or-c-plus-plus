# Design Patterns in OOP

## Scope
Applies to creational, structural, and behavioral design patterns implemented using object-oriented programming principles.

## Creational Patterns

### Singleton Pattern
* Ensure single instance of a class
* Global access point
* Lazy or eager initialization
* Thread safety considerations
* Use sparingly (prefer dependency injection)

### Factory Pattern
* Create objects without specifying exact class
* Centralize object creation logic
* Support multiple product types
* Abstract Factory for families of products
* Reduces coupling to concrete classes

### Builder Pattern
* Construct complex objects step by step
* Separate construction from representation
* Fluent interface for readability
* Useful for objects with many parameters
* Immutable object construction

### Prototype Pattern
* Clone existing objects
* Avoid expensive object creation
* Registry of prototypes
* Shallow vs deep copy considerations
* Useful for object initialization costs

## Structural Patterns

### Adapter Pattern
* Convert interface of one class to another
* Enable incompatible classes to work together
* Object adapter vs class adapter
* Wrapper pattern
* Useful for integrating third-party libraries

### Decorator Pattern
* Add behavior to objects dynamically
* Alternative to subclassing
* Compose behaviors flexibly
* Maintains object interface
* Useful for adding features without modifying classes

### Facade Pattern
* Provide simplified interface to complex subsystem
* Hide subsystem complexity
* Single entry point
* Reduces coupling to subsystems
* Useful for API simplification

### Proxy Pattern
* Provide surrogate or placeholder for another object
* Control access to original object
* Lazy initialization, access control, logging
* Virtual proxy, protection proxy, remote proxy
* Useful for resource management

## Behavioral Patterns

### Observer Pattern
* One-to-many dependency between objects
* Notify dependents of state changes
* Loose coupling between subject and observers
* Event-driven architecture
* Useful for model-view separation

### Strategy Pattern
* Define family of algorithms
* Encapsulate each algorithm
* Make algorithms interchangeable
* Eliminate conditional statements
* Useful for algorithm selection at runtime

### Command Pattern
* Encapsulate requests as objects
* Parameterize clients with requests
* Queue, log, and undo operations
* Decouple invoker from receiver
* Useful for undo/redo functionality

### State Pattern
* Allow object to alter behavior when state changes
* Object appears to change its class
* State-specific behavior
* Eliminate large conditional statements
* Useful for state machines

## Pattern Implementation Standards

### Code Quality
* Follow SOLID principles
* Use appropriate access control
* Document pattern usage and rationale
* Provide clear examples
* Consider performance implications

### Documentation
* Explain which pattern is used and why
* Document pattern participants
* Note trade-offs and alternatives
* Provide usage examples
* Reference Gang of Four or other sources

### Testing
* Test pattern behavior
* Test all participants
* Test edge cases
* Verify pattern correctness
* Test performance if critical

## Modern C++ Patterns

### RAII Pattern
* Resource Acquisition Is Initialization
* Automatic resource management
* Exception safety
* Use smart pointers
* Foundation of modern C++

### Pimpl Idiom
* Pointer to implementation
* Hide implementation details
* Reduce compilation dependencies
* Binary compatibility
* Performance trade-off

### CRTP (Curiously Recurring Template Pattern)
* Compile-time polymorphism
* Static polymorphism
* Zero-cost abstractions
* Used in many standard library components
* Advanced template technique

## Code Examples

### Factory Pattern Example
```cpp
class Product {
public:
    virtual ~Product() = default;
    virtual void operation() = 0;
};

class ConcreteProductA : public Product {
public:
    void operation() override {
        std::cout << "Product A operation" << std::endl;
    }
};

class Factory {
public:
    static std::unique_ptr<Product> createProduct(const std::string& type) {
        if (type == "A") {
            return std::make_unique<ConcreteProductA>();
        }
        // ... other products
        return nullptr;
    }
};
```

### Observer Pattern Example
```cpp
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const std::string& message) = 0;
};

class Subject {
private:
    std::vector<Observer*> observers_;
public:
    void attach(Observer* observer) {
        observers_.push_back(observer);
    }
    void notify(const std::string& message) {
        for (auto* obs : observers_) {
            obs->update(message);
        }
    }
};
```

## Related Topics
* Fundamentals: Basic OOP concepts used in patterns
* Inheritance: Patterns using inheritance
* Polymorphism: Patterns using polymorphism
* Modern C++: Modern pattern implementations

