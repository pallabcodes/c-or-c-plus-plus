# Object-Oriented Programming Module Overview

## Context
This code is written by an SDE 2 backend and low level system engineer working with top tier product companies including Google, Atlassian, Bloomberg, PayPal, Stripe, Uber, Amazon, and other top tier silicon valley companies. This OOP implementation must meet enterprise production standards suitable for principal level engineering review and must be comparable to top tier implementations used in production systems at these companies.

## Purpose
This module covers the design and implementation of production grade object oriented programming in C++ and C. All code must follow production grade standards suitable for principal level code review and must demonstrate correct, efficient, and maintainable OOP patterns including design patterns, inheritance, polymorphism, encapsulation, and abstraction.

## Scope
* Applies to all C++ and C code in oop directory
* Extends repository root rules defined in the root `.cursor/rules/` files
* Covers all aspects of OOP from fundamentals to design patterns
* Code quality standards align with expectations from top tier companies like Google, Bloomberg, Uber, and Amazon

## Top Tier Product Comparisons

### Google Production Systems
* Clean code principles
* SOLID principles
* Design pattern usage
* Production tested at massive scale
* Efficient OOP patterns

### Bloomberg Terminal Systems
* High performance OOP for financial systems
* Design patterns for financial data
* Production tested in critical financial systems
* Efficient polymorphism patterns
* Thread safe OOP patterns

### Uber Production Systems
* Efficient OOP for real time systems
* Design patterns for microservices
* Production tested at scale
* Performance optimized patterns
* Thread safe implementations

### Amazon Production Systems
* High performance OOP for cloud services
* Design patterns for distributed systems
* Production tested at massive scale
* Scalable OOP patterns
* Performance critical implementations

### Standard Libraries
* C++ Standard Library OOP patterns
* STL container design
* Standard design patterns
* Production grade OOP usage

## OOP Principles

### Encapsulation
* **Data hiding**: Hide implementation details
* **Access control**: public, protected, private
* **Interfaces**: Define clear interfaces
* **Rationale**: Encapsulation enables maintainability

### Inheritance
* **Single inheritance**: C++ single inheritance
* **Virtual inheritance**: Multiple inheritance with virtual base
* **Abstract classes**: Pure virtual functions
* **Rationale**: Inheritance enables code reuse

### Polymorphism
* **Virtual functions**: Runtime polymorphism
* **Function overloading**: Compile time polymorphism
* **Templates**: Generic programming
* **Rationale**: Polymorphism enables flexibility

### Abstraction
* **Abstract classes**: Pure virtual base classes
* **Interfaces**: Abstract interfaces
* **Design patterns**: Pattern based abstractions
* **Rationale**: Abstraction enables flexibility

## Design Patterns

### Creational Patterns
* **Singleton**: Single instance pattern
* **Factory**: Object creation pattern
* **Builder**: Step by step construction
* **Prototype**: Clone existing objects
* **Rationale**: Creational patterns manage object creation

### Structural Patterns
* **Adapter**: Interface adaptation
* **Decorator**: Dynamic behavior extension
* **Facade**: Simplified interface
* **Proxy**: Control access to objects
* **Rationale**: Structural patterns compose objects

### Behavioral Patterns
* **Observer**: Event notification
* **Strategy**: Algorithm selection
* **Command**: Encapsulate requests
* **State**: State based behavior
* **Rationale**: Behavioral patterns manage object communication

## Thread Safety

### Thread Safe Patterns
* **Thread safe singleton**: Mutex protected singleton
* **Thread safe factory**: Synchronized factory
* **Thread safe observer**: Lock protected observer
* **Rationale**: Thread safety enables concurrent usage

## Production Standards

### Code Quality
* Functions limited to 50 lines
* Files limited to 200 lines
* Cyclomatic complexity ≤ 10
* Comprehensive error handling
* Input validation on all public APIs
* Memory safety and leak prevention

### Performance
* Efficient virtual function calls
* Minimize virtual function overhead
* Use final when appropriate
* Avoid unnecessary polymorphism
* Benchmark critical paths

### Correctness
* Proper inheritance hierarchies
* Correct virtual function usage
* Exception safety
* Resource management (RAII)
* Comprehensive test coverage

### Documentation
* API documentation for all public functions
* Inheritance relationships
* Virtual function contracts
* Design pattern usage
* Thread safety guarantees

## Research Papers and References

### OOP Design
* "Design Patterns: Elements of Reusable Object Oriented Software" (Gang of Four)
* "Clean Code" (Martin) - Code quality principles
* "SOLID Principles" - Object oriented design principles

### Design Patterns
* "Design Patterns" (Gamma, Helm, Johnson, Vlissides)
* "Pattern Oriented Software Architecture" (Buschmann et al.)
* Design pattern catalogs

### Open Source References
* Google C++ Style Guide
* Boost libraries design patterns
* Standard C++ Library patterns

## Implementation Goals

### Correctness
* Correct inheritance hierarchies
* Proper virtual function usage
* Exception safety
* Resource management
* Comprehensive testing

### Performance
* Efficient virtual calls
* Minimize overhead
* Appropriate pattern usage
* Benchmark and optimize
* Cache friendly design

### Maintainability
* Clean, readable code
* Comprehensive documentation
* Extensive test coverage
* Clear design patterns
* Well documented trade offs
