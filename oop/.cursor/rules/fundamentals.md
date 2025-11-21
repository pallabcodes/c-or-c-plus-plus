# OOP Fundamentals

## Scope
Applies to fundamental object-oriented programming concepts including classes, objects, encapsulation, abstraction, and basic class design.

## Classes and Objects

### Class Definition
* Classes encapsulate data and behavior
* Objects are instances of classes
* Classes define the blueprint for objects
* Use meaningful class names (PascalCase convention)
* Group related data and functions together

### Object Creation
* Stack allocation: `ClassName obj;`
* Heap allocation: `ClassName* obj = new ClassName();`
* Modern C++: Prefer stack allocation or smart pointers
* Initialization: Use constructors, initialization lists
* Destruction: Use destructors for cleanup

### Basic Class Structure
```cpp
class MyClass {
private:
    // Private members - internal state
    int data_;
    
public:
    // Public interface
    MyClass(int data) : data_(data) {}
    int getData() const { return data_; }
    void setData(int data) { data_ = data; }
    
    ~MyClass() = default;
};
```

## Encapsulation

### Access Control
* `private`: Accessible only within the class
* `protected`: Accessible within class and derived classes
* `public`: Accessible from anywhere
* Default access in classes is private
* Use private for implementation details

### Data Hiding
* Hide internal implementation details
* Expose only necessary interface
* Use accessors and mutators for controlled access
* Consider const correctness
* Avoid exposing raw pointers or references to internal data

### Implementation Standards
* Use underscore suffix for member variables (`data_`)
* Provide const methods for read-only access
* Validate inputs in mutators
* Document access patterns
* Consider thread safety for shared objects

## Abstraction

### Abstract Classes
* Classes with pure virtual functions
* Cannot be instantiated directly
* Define interface contracts
* Derived classes must implement pure virtual functions
* Use for defining common interfaces

### Interfaces
* C++ uses abstract classes for interfaces
* Pure virtual functions define interface
* No data members in pure interfaces
* Multiple inheritance for multiple interfaces
* Use for dependency inversion

### Code Example
```cpp
// Abstract interface
class IShape {
public:
    virtual double area() const = 0;
    virtual double perimeter() const = 0;
    virtual ~IShape() = default;
};

// Concrete implementation
class Circle : public IShape {
private:
    double radius_;
public:
    Circle(double r) : radius_(r) {}
    double area() const override {
        return 3.14159 * radius_ * radius_;
    }
    double perimeter() const override {
        return 2 * 3.14159 * radius_;
    }
};
```

## Constructors and Destructors

### Constructors
* Initialize objects
* Can have multiple constructors (overloading)
* Use initialization lists for efficiency
* Delegate constructors (C++11)
* Default constructors, copy constructors, move constructors

### Destructors
* Clean up resources
* Virtual destructors in base classes
* RAII pattern for resource management
* Default destructors when no cleanup needed
* Order of destruction (reverse of construction)

### Rule of Three/Five/Zero
* Rule of Three: Destructor, copy constructor, copy assignment
* Rule of Five: Add move constructor, move assignment (C++11)
* Rule of Zero: Use smart pointers and standard library types

## Member Functions

### Const Correctness
* Const member functions don't modify object state
* Const objects can only call const methods
* Mutable keyword for exceptions
* Use const references for parameters when possible
* Const correctness enables better optimization

### Inline Functions
* Defined in header for template-like behavior
* Compiler hint for inlining
* Use for small, frequently called functions
* Trade-off: Code size vs. performance
* Modern compilers often inline automatically

## Code Quality Standards

### Documentation
* Document class purpose and responsibilities
* Explain public API methods
* Note invariants and constraints
* Document thread safety guarantees
* Provide usage examples

### Error Handling
* Validate constructor parameters
* Use exceptions for error conditions
* Provide exception safety guarantees
* Document exception specifications
* Consider noexcept for performance

### Testing
* Test constructor initialization
* Test all public methods
* Test error conditions
* Test const correctness
* Verify resource cleanup

## Related Topics
* Inheritance: Extending classes with inheritance
* Polymorphism: Virtual functions and dynamic dispatch
* Design Patterns: Common OOP patterns
* Modern C++: Smart pointers, RAII, move semantics

