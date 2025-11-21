# OOP Fundamentals Standards

## Overview
OOP fundamentals form the foundation of object oriented programming. This document defines standards for implementing production grade OOP principles including encapsulation, inheritance, polymorphism, and abstraction.

## Encapsulation

### Access Control
* **private**: Implementation details, not accessible outside class
* **protected**: Accessible to derived classes
* **public**: Public interface, accessible everywhere
* **Rationale**: Access control enables data hiding

### Data Hiding
* **Member variables**: Make member variables private or protected
* **Accessors**: Provide getters/setters when needed
* **Implementation hiding**: Hide implementation details
* **Rationale**: Data hiding enables maintainability

### Example Encapsulation
```cpp
class BankAccount {
private:
    double balance_;  // Hidden implementation detail

public:
    double getBalance() const { return balance_; }
    void deposit(double amount) {
        if (amount > 0) {
            balance_ += amount;
        }
    }
};
```

## Inheritance

### Single Inheritance
* **Base class**: Define base class with common interface
* **Derived class**: Inherit from base class
* **Access specifier**: Use public inheritance for is a relationship
* **Rationale**: Single inheritance enables code reuse

### Virtual Functions
* **Virtual keyword**: Use virtual for runtime polymorphism
* **Pure virtual**: Use = 0 for abstract classes
* **Override**: Use override keyword (C++11)
* **Rationale**: Virtual functions enable polymorphism

### Abstract Classes
* **Pure virtual**: At least one pure virtual function
* **Cannot instantiate**: Cannot create instances
* **Interface**: Defines interface for derived classes
* **Rationale**: Abstract classes define contracts

### Example Inheritance
```cpp
class Shape {
public:
    virtual double area() const = 0;  // Pure virtual
    virtual ~Shape() = default;
};

class Circle : public Shape {
private:
    double radius_;

public:
    Circle(double radius) : radius_(radius) {}
    double area() const override {
        return 3.14159 * radius_ * radius_;
    }
};
```

## Polymorphism

### Runtime Polymorphism
* **Virtual functions**: Use virtual functions
* **Base pointers**: Use base class pointers/references
* **Dynamic dispatch**: Runtime function selection
* **Rationale**: Runtime polymorphism enables flexibility

### Compile Time Polymorphism
* **Function overloading**: Same name, different parameters
* **Templates**: Generic programming
* **Operator overloading**: Overload operators
* **Rationale**: Compile time polymorphism enables efficiency

### Example Polymorphism
```cpp
class Animal {
public:
    virtual void makeSound() const = 0;
};

class Dog : public Animal {
public:
    void makeSound() const override {
        std::cout << "Woof!" << std::endl;
    }
};

void processAnimal(const Animal& animal) {
    animal.makeSound();  // Polymorphic call
}
```

## Abstraction

### Abstract Classes
* **Pure virtual functions**: Define abstract interface
* **Implementation**: Derived classes provide implementation
* **Rationale**: Abstraction enables flexibility

### Interfaces
* **Pure abstract**: All functions pure virtual
* **No data members**: Typically no data members
* **Contract**: Defines contract for implementations
* **Rationale**: Interfaces define contracts

## Implementation Standards

### Correctness
* **Proper inheritance**: Use inheritance for is a relationship
* **Virtual destructors**: Use virtual destructors in base classes
* **Exception safety**: Maintain exception safety
* **Rationale**: Correctness is critical

### Performance
* **Virtual overhead**: Understand virtual function overhead
* **Use final**: Use final when inheritance not needed
* **Inline non virtual**: Prefer inline non virtual functions
* **Rationale**: Performance is critical

## Testing Requirements

### Unit Tests
* **Inheritance**: Test inheritance hierarchies
* **Polymorphism**: Test polymorphic behavior
* **Encapsulation**: Test access control
* **Edge cases**: Test boundary conditions
* **Rationale**: Comprehensive testing ensures correctness

## Research Papers and References

### OOP Fundamentals
* "Object Oriented Programming" - OOP principles
* "Clean Code" (Martin) - Code quality principles
* "SOLID Principles" - Design principles

## Implementation Checklist

- [ ] Understand encapsulation
- [ ] Learn inheritance
- [ ] Understand polymorphism
- [ ] Learn abstraction
- [ ] Practice OOP principles
- [ ] Write comprehensive unit tests
- [ ] Document OOP design

