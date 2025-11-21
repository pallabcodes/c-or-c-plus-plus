# SOLID Principles Standards

## Overview
SOLID principles are fundamental design principles that guide object oriented design. This document defines standards for applying SOLID principles in production grade code.

## Single Responsibility Principle (SRP)

### Definition
* **Single responsibility**: Class should have one reason to change
* **Cohesion**: High cohesion within class
* **Coupling**: Low coupling between classes
* **Rationale**: SRP enables maintainability

### Application
* **Identify responsibilities**: Identify class responsibilities
* **Separate concerns**: Separate different concerns
* **Refactor**: Refactor classes with multiple responsibilities
* **Rationale**: Application ensures SRP compliance

### Example SRP Violation
```cpp
// BAD: Multiple responsibilities
class User {
    void saveToDatabase() { /* Database logic */ }
    void sendEmail() { /* Email logic */ }
    void generateReport() { /* Report logic */ }
};
```

### Example SRP Compliance
```cpp
// GOOD: Single responsibility
class User {
    // User data only
};

class UserRepository {
    void save(User& user) { /* Database logic */ }
};

class EmailService {
    void send(User& user) { /* Email logic */ }
};
```

## Open/Closed Principle (OCP)

### Definition
* **Open for extension**: Open for extension
* **Closed for modification**: Closed for modification
* **Polymorphism**: Use polymorphism for extension
* **Rationale**: OCP enables extensibility

### Application
* **Abstract interfaces**: Use abstract interfaces
* **Polymorphism**: Use polymorphism
* **Avoid modification**: Avoid modifying existing code
* **Rationale**: Application ensures OCP compliance

### Example OCP Violation
```cpp
// BAD: Modification required for extension
class Shape {
    void draw() {
        if (type == CIRCLE) { /* Circle drawing */ }
        else if (type == SQUARE) { /* Square drawing */ }
        // Adding new shape requires modification
    }
};
```

### Example OCP Compliance
```cpp
// GOOD: Extension without modification
class Shape {
public:
    virtual void draw() = 0;
};

class Circle : public Shape {
public:
    void draw() override { /* Circle drawing */ }
};

class Square : public Shape {
public:
    void draw() override { /* Square drawing */ }
};
```

## Liskov Substitution Principle (LSP)

### Definition
* **Substitutability**: Subtypes must be substitutable for base types
* **Contract compliance**: Subtypes must honor base type contract
* **Behavior preservation**: Preserve expected behavior
* **Rationale**: LSP ensures correctness

### Application
* **Contract design**: Design clear contracts
* **Subtype compliance**: Ensure subtypes comply with contract
* **Behavior testing**: Test subtype behavior
* **Rationale**: Application ensures LSP compliance

### Example LSP Violation
```cpp
// BAD: Subtype violates contract
class Rectangle {
    virtual void setWidth(int w) { width = w; }
    virtual void setHeight(int h) { height = h; }
};

class Square : public Rectangle {
    void setWidth(int w) override {
        width = w;
        height = w;  // Violates rectangle contract
    }
};
```

## Interface Segregation Principle (ISP)

### Definition
* **Client specific interfaces**: Clients should not depend on unused interfaces
* **Lean interfaces**: Keep interfaces lean
* **Separation**: Separate interfaces by client needs
* **Rationale**: ISP enables better design

### Application
* **Identify clients**: Identify interface clients
* **Separate interfaces**: Separate interfaces by client needs
* **Avoid fat interfaces**: Avoid interfaces with many methods
* **Rationale**: Application ensures ISP compliance

### Example ISP Violation
```cpp
// BAD: Fat interface
class Worker {
    virtual void work() = 0;
    virtual void eat() = 0;
    virtual void sleep() = 0;
};
```

### Example ISP Compliance
```cpp
// GOOD: Segregated interfaces
class Workable {
    virtual void work() = 0;
};

class Eatable {
    virtual void eat() = 0;
};

class Sleepable {
    virtual void sleep() = 0;
};
```

## Dependency Inversion Principle (DIP)

### Definition
* **Abstraction dependency**: Depend on abstractions, not concretions
* **High level independence**: High level modules independent of low level
* **Inversion**: Invert dependency direction
* **Rationale**: DIP enables flexibility

### Application
* **Abstract interfaces**: Use abstract interfaces
* **Dependency injection**: Use dependency injection
* **Avoid concrete dependencies**: Avoid depending on concrete classes
* **Rationale**: Application ensures DIP compliance

### Example DIP Violation
```cpp
// BAD: Dependency on concrete class
class UserService {
    MySQLDatabase db;  // Concrete dependency
};
```

### Example DIP Compliance
```cpp
// GOOD: Dependency on abstraction
class UserService {
    Database& db;  // Abstract dependency
public:
    UserService(Database& d) : db(d) {}
};
```

## Implementation Standards

### Correctness
* **SRP compliance**: Ensure single responsibility
* **OCP compliance**: Ensure open/closed principle
* **LSP compliance**: Ensure Liskov substitution
* **ISP compliance**: Ensure interface segregation
* **DIP compliance**: Ensure dependency inversion
* **Rationale**: Correctness is critical

### Testing
* **SOLID tests**: Test SOLID principles compliance
* **Refactoring tests**: Test after refactoring
* **Rationale**: Testing ensures compliance

## Research Papers and References

### SOLID Principles
* "Clean Code" (Martin) - SOLID principles
* "Agile Software Development" (Martin) - SOLID principles
* SOLID principles research

## Implementation Checklist

- [ ] Understand SRP
- [ ] Understand OCP
- [ ] Understand LSP
- [ ] Understand ISP
- [ ] Understand DIP
- [ ] Apply SOLID principles
- [ ] Write SOLID compliance tests
- [ ] Document SOLID application

