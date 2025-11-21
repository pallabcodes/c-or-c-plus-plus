# Encapsulation and Access Control

## Scope
Applies to encapsulation principles, access control, data hiding, and interface design in C++.

## Encapsulation Principles

### Concept
* Bundling data and methods that operate on that data
* Hiding internal implementation details
* Exposing only necessary interface
* Controlling access to internal state
* Foundation of object-oriented design

### Benefits
* Data protection and validation
* Implementation flexibility
* Easier maintenance and modification
* Reduced coupling between components
* Better abstraction and modularity

## Access Specifiers

### Private Members
* Accessible only within the class
* Default access in classes
* Hide implementation details
* Protect internal state
* Use for data members and helper methods

### Protected Members
* Accessible within class and derived classes
* Used in inheritance hierarchies
* Balance between encapsulation and inheritance
* Document protected interface clearly
* Consider if protected is truly needed

### Public Members
* Accessible from anywhere
* Use for class interface
* Keep public interface minimal
* Document public API thoroughly
* Consider const correctness

### Code Example
```cpp
class BankAccount {
private:
    double balance_;  // Hidden internal state
    
protected:
    void logTransaction(const std::string& type) {
        // Accessible to derived classes
    }
    
public:
    BankAccount(double initial) : balance_(initial) {}
    
    double getBalance() const {  // Controlled access
        return balance_;
    }
    
    void deposit(double amount) {  // Validated access
        if (amount > 0) {
            balance_ += amount;
            logTransaction("deposit");
        }
    }
};
```

## Data Hiding

### Principles
* Hide implementation details
* Expose only what's necessary
* Use accessors for read access
* Use mutators for write access with validation
* Avoid exposing raw pointers/references to internals

### Accessor Patterns
* Getter methods for read access
* Const methods for non-modifying operations
* Return by value or const reference
* Avoid returning non-const references to internals
* Consider performance implications

### Mutator Patterns
* Setter methods for write access
* Validate inputs before modification
* Maintain class invariants
* Consider transaction semantics
* Document side effects

## Interface Design

### Public Interface
* Minimal and focused interface
* Clear method names and purposes
* Consistent naming conventions
* Well-documented API
* Stable interface (avoid breaking changes)

### Internal Implementation
* Private helper methods
* Implementation details hidden
* Can change without affecting users
* Optimize internal implementation freely
* Document only if non-obvious

### Code Example
```cpp
class Stack {
private:
    std::vector<int> data_;  // Hidden implementation
    
    void ensureCapacity() {  // Private helper
        if (data_.size() == data_.capacity()) {
            data_.reserve(data_.capacity() * 2);
        }
    }
    
public:
    void push(int value) {  // Public interface
        ensureCapacity();
        data_.push_back(value);
    }
    
    int pop() {
        if (data_.empty()) {
            throw std::runtime_error("Stack is empty");
        }
        int value = data_.back();
        data_.pop_back();
        return value;
    }
    
    bool empty() const {
        return data_.empty();
    }
};
```

## Const Correctness

### Const Member Functions
* Don't modify object state
* Can be called on const objects
* Enables const references in parameters
* Better optimization opportunities
* Clearer code intent

### Const Objects
* Cannot call non-const methods
* Can only call const methods
* Immutable objects
* Thread-safe for read operations
* Useful for function parameters

### Mutable Keyword
* Exception to const correctness
* Use for caching, logging, mutexes
* Document why mutable is needed
* Use sparingly
* Consider alternatives

## Friend Functions and Classes

### Friend Functions
* Non-member functions with special access
* Can access private/protected members
* Use for operator overloading
* Use sparingly (breaks encapsulation)
* Document friend relationships

### Friend Classes
* One class grants access to another
* Use for tightly coupled classes
* Consider if composition would work better
* Document friend relationships clearly
* Prefer public interface when possible

## Code Quality Standards

### Documentation
* Document public interface thoroughly
* Explain access control decisions
* Note invariants and constraints
* Document friend relationships
* Provide usage examples

### Error Handling
* Validate inputs in mutators
* Maintain class invariants
* Use exceptions for error conditions
* Document exception specifications
* Consider noexcept for performance

### Testing
* Test all public methods
* Test access control (private inaccessible)
* Test const correctness
* Test invariants are maintained
* Test error conditions

## Best Practices

### Access Control Guidelines
* Make data members private by default
* Provide accessors only when needed
* Use protected sparingly (prefer composition)
* Keep public interface minimal
* Document access patterns

### Encapsulation Patterns
* Pimpl idiom for implementation hiding
* Interface classes for abstraction
* Factory patterns for object creation
* Builder patterns for complex construction
* RAII for resource management

## Related Topics
* Fundamentals: Basic class design
* Inheritance: Protected access in inheritance
* Design Patterns: Patterns using encapsulation
* Modern C++: Smart pointers, RAII

