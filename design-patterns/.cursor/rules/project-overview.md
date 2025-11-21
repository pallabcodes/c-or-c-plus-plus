# Design Patterns Module Overview

## Context
This code is written by an SDE 2 backend and low level system engineer working with top tier product companies including Google, Atlassian, Bloomberg, PayPal, Stripe, Uber, Amazon, and other top tier silicon valley companies. This design patterns implementation must meet enterprise production standards suitable for principal level engineering review and must be comparable to top tier implementations used in production systems at these companies.

## Purpose
This module covers the design and implementation of production grade design patterns in C++. All code must follow production grade standards suitable for principal level code review and must demonstrate correct, efficient, and maintainable pattern implementations including creational, structural, and behavioral patterns from the Gang of Four catalog and modern C++ patterns.

## Scope
* Applies to all C++ code in design-patterns directory
* Extends repository root rules defined in the root `.cursor/rules/` files
* Covers all aspects of design patterns from fundamentals to advanced implementations
* Code quality standards align with expectations from top tier companies like Google, Bloomberg, Uber, and Amazon

## Top Tier Product Comparisons

### Google Production Systems
* Clean code principles
* SOLID principles
* Design pattern usage in production
* Production tested at massive scale
* Efficient pattern implementations

### Bloomberg Terminal Systems
* High performance patterns for financial systems
* Design patterns for financial data structures
* Production tested in critical financial systems
* Efficient pattern implementations
* Thread safe pattern implementations

### Uber Production Systems
* Efficient patterns for real time systems
* Design patterns for microservices
* Production tested at scale
* Performance optimized patterns
* Scalable pattern implementations

### Amazon Production Systems
* High performance patterns for cloud services
* Design patterns for distributed systems
* Production tested at massive scale
* Scalable pattern implementations
* Performance critical implementations

### Standard Libraries
* C++ Standard Library pattern usage
* STL design patterns
* Standard pattern implementations
* Production grade pattern practices

## Design Pattern Categories

### Creational Patterns
* **Singleton**: Single instance pattern
* **Factory**: Object creation pattern
* **Abstract Factory**: Factory of factories
* **Builder**: Step by step construction
* **Prototype**: Clone existing objects
* **Rationale**: Creational patterns manage object creation

### Structural Patterns
* **Adapter**: Interface adaptation
* **Bridge**: Decouple abstraction from implementation
* **Composite**: Tree structure of objects
* **Decorator**: Dynamic behavior extension
* **Facade**: Simplified interface
* **Flyweight**: Memory efficient sharing
* **Proxy**: Control access to objects
* **Rationale**: Structural patterns compose objects

### Behavioral Patterns
* **Chain of Responsibility**: Request handling chain
* **Command**: Encapsulate requests
* **Interpreter**: Language interpretation
* **Iterator**: Traverse collections
* **Mediator**: Centralized communication
* **Memento**: State capture and restoration
* **Observer**: Event notification
* **State**: State based behavior
* **Strategy**: Algorithm selection
* **Template Method**: Algorithm skeleton
* **Visitor**: Operations on object structure
* **Rationale**: Behavioral patterns manage object communication

## SOLID Principles

### Single Responsibility Principle
* **Definition**: Class should have one reason to change
* **Benefits**: Easier maintenance, clearer design
* **Rationale**: SRP enables maintainability

### Open/Closed Principle
* **Definition**: Open for extension, closed for modification
* **Benefits**: Extensibility without breaking existing code
* **Rationale**: OCP enables extensibility

### Liskov Substitution Principle
* **Definition**: Subtypes must be substitutable for base types
* **Benefits**: Correct inheritance hierarchies
* **Rationale**: LSP ensures correctness

### Interface Segregation Principle
* **Definition**: Clients should not depend on unused interfaces
* **Benefits**: Leaner interfaces, better decoupling
* **Rationale**: ISP enables better design

### Dependency Inversion Principle
* **Definition**: Depend on abstractions, not concretions
* **Benefits**: Loose coupling, testability
* **Rationale**: DIP enables flexibility

## Modern C++ Patterns

### RAII
* **Definition**: Resource Acquisition Is Initialization
* **Benefits**: Automatic resource management
* **Use cases**: Memory, file handles, locks
* **Rationale**: RAII prevents resource leaks

### Smart Pointers
* **Definition**: Automatic memory management
* **Types**: unique_ptr, shared_ptr, weak_ptr
* **Benefits**: Memory safety without GC
* **Rationale**: Smart pointers enable safe memory management

### Move Semantics
* **Definition**: Efficient resource transfer
* **Benefits**: Avoids copying, improves performance
* **Use cases**: Large objects, temporary objects
* **Rationale**: Move semantics improve performance

### Templates and Generic Programming
* **Definition**: Type generic code
* **Benefits**: Code reuse, type safety
* **Use cases**: Containers, algorithms
* **Rationale**: Templates enable generic programming

## Production Standards

### Code Quality
* Functions limited to 50 lines
* Files limited to 200 lines
* Cyclomatic complexity ≤ 10
* Comprehensive error handling
* Input validation on all public APIs
* Memory safety and leak prevention

### Performance
* Efficient pattern implementations
* Minimize pattern overhead
* Use modern C++ features appropriately
* Benchmark critical paths
* Profile pattern usage

### Correctness
* Proper pattern implementation
* SOLID principles compliance
* Exception safety
* Resource management (RAII)
* Comprehensive test coverage

### Documentation
* API documentation for all public functions
* Pattern usage documentation
* SOLID principles application
* Thread safety guarantees
* Performance characteristics

## Research Papers and References

### Design Patterns
* "Design Patterns: Elements of Reusable Object Oriented Software" (Gang of Four)
* "Pattern Oriented Software Architecture" (Buschmann et al.)
* Design pattern catalogs

### SOLID Principles
* "Clean Code" (Martin) - SOLID principles
* "Agile Software Development" (Martin) - SOLID principles
* SOLID principles research

### Modern C++
* "Effective Modern C++" (Meyers) - Modern C++ patterns
* "C++ Core Guidelines" - Modern C++ best practices
* Modern C++ pattern research

### Open Source References
* Google C++ Style Guide
* Boost libraries design patterns
* Standard C++ Library patterns

## Implementation Goals

### Correctness
* Correct pattern implementation
* SOLID principles compliance
* Exception safety
* Resource management
* Comprehensive testing

### Performance
* Efficient pattern implementations
* Minimize overhead
* Use modern C++ appropriately
* Benchmark and optimize
* Profile critical paths

### Maintainability
* Clean, readable code
* Comprehensive documentation
* Extensive test coverage
* Clear pattern usage
* Well documented trade offs

