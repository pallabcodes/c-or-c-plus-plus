# Modern C++ OOP Features

## Scope
Applies to modern C++ features (C++11/14/17/20) that enhance object-oriented programming, including smart pointers, RAII, move semantics, lambdas, and modern class features.

## Smart Pointers

### unique_ptr
* Exclusive ownership semantics
* Automatic memory management
* Move-only type (no copy)
* Zero overhead abstraction
* Prefer over raw pointers for ownership

### shared_ptr
* Shared ownership semantics
* Reference counting
* Thread-safe reference counting
* Use when shared ownership is needed
* Performance overhead (reference counting)

### weak_ptr
* Non-owning reference to shared_ptr
* Break circular references
* Check if object still exists
* Use with shared_ptr
* No ownership overhead

### Code Example
```cpp
class Resource {
public:
    Resource() { std::cout << "Resource created" << std::endl; }
    ~Resource() { std::cout << "Resource destroyed" << std::endl; }
};

void useResource() {
    std::unique_ptr<Resource> ptr = std::make_unique<Resource>();
    // Automatic cleanup when ptr goes out of scope
}
```

## RAII (Resource Acquisition Is Initialization)

### Principles
* Acquire resources in constructor
* Release resources in destructor
* Automatic cleanup on scope exit
* Exception safety
* Foundation of modern C++

### Benefits
* Automatic resource management
* Exception safety
* No manual cleanup needed
* Prevents resource leaks
* Clear ownership semantics

### Implementation
* Constructor acquires resource
* Destructor releases resource
* Copy/move semantics as needed
* Use smart pointers for memory
* Use RAII wrappers for other resources

## Move Semantics

### Rvalue References
* Bind to temporary objects
* Enable move semantics
* Performance optimization
* Avoid unnecessary copies
* C++11 feature

### Move Constructor
* Transfer ownership of resources
* Steal resources from source
* Leave source in valid but unspecified state
* More efficient than copying
* Use for expensive-to-copy types

### Move Assignment
* Transfer ownership via assignment
* Release old resources
* Steal new resources
* Self-assignment safe
* Return *this by reference

### Code Example
```cpp
class Movable {
private:
    int* data_;
    size_t size_;
public:
    // Move constructor
    Movable(Movable&& other) noexcept
        : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;
        other.size_ = 0;
    }
    
    // Move assignment
    Movable& operator=(Movable&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            other.data_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }
};
```

## Lambda Expressions

### Basic Syntax
* Anonymous function objects
* Capture variables from scope
* Can be stored and passed around
* Type inference with auto
* C++11 feature

### Capture Modes
* `[=]` capture by value
* `[&]` capture by reference
* `[var]` capture specific variable
* `[=, &var]` mixed capture
* `[this]` capture this pointer

### Use Cases
* STL algorithms
* Callbacks
* Event handlers
* Functional programming
* Temporary function objects

### Code Example
```cpp
std::vector<int> vec = {1, 2, 3, 4, 5};
std::for_each(vec.begin(), vec.end(), 
    [](int x) { std::cout << x << " "; });

auto lambda = [](int a, int b) { return a + b; };
int result = lambda(5, 3);
```

## Modern Class Features

### Default and Delete
* `= default` for compiler-generated functions
* `= delete` to prevent function generation
* Explicit control over special functions
* Clear intent
* C++11 feature

### Override and Final
* `override` keyword for clarity and safety
* `final` keyword to prevent overriding
* Catch errors at compile time
* Document intent clearly
* C++11 feature

### Delegating Constructors
* One constructor calls another
* Reduce code duplication
* Initialize in one place
* C++11 feature
* Useful for constructor overloading

### Code Example
```cpp
class ModernClass {
private:
    int value_;
public:
    ModernClass() : ModernClass(0) {}  // Delegating constructor
    
    explicit ModernClass(int v) : value_(v) {}
    
    ModernClass(const ModernClass&) = default;
    ModernClass& operator=(const ModernClass&) = default;
    ModernClass(ModernClass&&) = default;
    ModernClass& operator=(ModernClass&&) = default;
    ~ModernClass() = default;
    
    virtual void method() override final {}  // Override and final
};
```

## Constexpr and Consteval

### Constexpr Functions
* Evaluated at compile time when possible
* Can be used in constant expressions
* Performance optimization
* Type safety
* C++11/14/17 feature

### Constexpr Constructors
* Construct objects at compile time
* Constexpr objects
* Useful for constants
* Performance benefits
* C++11 feature

## Concepts and Constraints (C++20)

### Concepts
* Named sets of requirements
* Type constraints for templates
* Better error messages
* Explicit requirements
* C++20 feature

### Requires Clauses
* Specify template requirements
* Type traits and expressions
* Compile-time validation
* Clearer template code
* C++20 feature

## Code Quality Standards

### Documentation
* Document modern C++ feature usage
* Explain performance benefits
* Note C++ standard version requirements
* Document move semantics
* Explain smart pointer choices

### Error Handling
* Exception safety with RAII
* noexcept specifications
* Handle move semantics correctly
* Smart pointer exception safety
* Document exception guarantees

### Testing
* Test move semantics
* Test smart pointer behavior
* Test lambda captures
* Test constexpr evaluation
* Verify exception safety

## Related Topics
* Fundamentals: Basic OOP concepts
* Memory Management: RAII and smart pointers
* Design Patterns: Modern pattern implementations
* Performance: Move semantics optimization

