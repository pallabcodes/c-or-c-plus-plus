# Inheritance and Polymorphism

## Scope
Applies to inheritance hierarchies, polymorphism, virtual functions, and advanced inheritance patterns in C++.

## Inheritance Fundamentals

### Basic Inheritance
* Derived classes inherit from base classes
* Public inheritance for "is-a" relationships
* Private/protected inheritance for implementation inheritance
* Access control affects inheritance visibility
* Constructor initialization order (base to derived)

### Inheritance Syntax
```cpp
class Base {
public:
    Base(int value) : value_(value) {}
    virtual ~Base() = default;
protected:
    int value_;
};

class Derived : public Base {
public:
    Derived(int value, int extra) : Base(value), extra_(extra) {}
private:
    int extra_;
};
```

### Access Specifiers in Inheritance
* `public`: Public members remain public, protected remain protected
* `protected`: Public and protected become protected
* `private`: All become private (implementation inheritance)

## Virtual Functions

### Virtual Function Mechanism
* Virtual functions enable runtime polymorphism
* Virtual function table (vtable) for dispatch
* Override keyword for clarity (C++11)
* Final keyword to prevent further overriding
* Virtual destructors required in base classes

### Virtual Function Overhead
* Vtable pointer per object (one pointer overhead)
* Indirect function call (one indirection)
* Cannot inline virtual calls (usually)
* Consider performance implications
* Use final for leaf classes when appropriate

### Pure Virtual Functions
* Abstract base classes with pure virtual functions
* Cannot instantiate abstract classes
* Derived classes must implement pure virtuals
* Can have data members and non-pure virtuals
* Use for interface definitions

## Polymorphism

### Runtime Polymorphism
* Virtual function dispatch
* Base class pointers/references to derived objects
* Dynamic binding at runtime
* Enables polymorphic behavior
* Foundation of many design patterns

### Compile-Time Polymorphism
* Templates for generic programming
* Function overloading
* Operator overloading
* CRTP (Curiously Recurring Template Pattern)
* No runtime overhead

### Code Example
```cpp
class Animal {
public:
    virtual void makeSound() = 0;
    virtual ~Animal() = default;
};

class Dog : public Animal {
public:
    void makeSound() override {
        std::cout << "Woof!" << std::endl;
    }
};

class Cat : public Animal {
public:
    void makeSound() override {
        std::cout << "Meow!" << std::endl;
    }
};

// Runtime polymorphism
void makeAnimalSound(Animal* animal) {
    animal->makeSound(); // Calls appropriate derived implementation
}
```

## Multiple Inheritance

### Diamond Problem
* Multiple inheritance can create ambiguity
* Diamond inheritance: Base -> Derived1, Derived2 -> MostDerived
* Virtual inheritance solves diamond problem
* Adds complexity and overhead
* Prefer composition or single inheritance when possible

### Virtual Inheritance
* `virtual` keyword in inheritance
* Shared base class instance
* Resolves diamond problem
* Additional overhead (vtable for virtual base)
* Use sparingly and document clearly

### Interface Inheritance
* Multiple inheritance for interfaces
* Pure abstract classes as interfaces
* No data members in interfaces
* Common pattern in C++
* Enables interface segregation

## Overriding and Hiding

### Function Overriding
* Derived class redefines base class virtual function
* Must have same signature (except covariant return types)
* Use override keyword to catch errors
* Virtual keyword in base class required
* Final keyword prevents further overriding

### Function Hiding
* Non-virtual functions hide base class functions
* Name hiding based on name, not signature
* Use `using` declaration to expose base functions
* Can cause confusion and bugs
* Prefer virtual functions for polymorphism

## VTable and Virtual Dispatch

### VTable Structure
* One vtable per class with virtual functions
* Array of function pointers
* Vtable pointer in each object
* Shared among objects of same class
* Compiler-generated

### Virtual Dispatch Mechanism
* Object contains vtable pointer
* Call through vtable to correct function
* Runtime resolution of function address
* Enables polymorphic behavior
* Overhead: one pointer + one indirection

### Performance Considerations
* Virtual call overhead is small but measurable
* Cannot inline virtual calls (usually)
* Consider final for leaf classes
* Profile before optimizing
* Templates for zero-cost abstractions

## Code Quality Standards

### Documentation
* Document inheritance relationships clearly
* Explain why inheritance is used
* Note virtual function purposes
* Document override intentions
* Explain multiple inheritance rationale

### Error Handling
* Virtual destructors in base classes
* Exception safety in virtual functions
* Handle inheritance-related errors
* Document exception specifications
* Consider noexcept for performance

### Testing
* Test polymorphic behavior
* Test all derived class implementations
* Test virtual function dispatch
* Test inheritance hierarchies
* Verify proper cleanup

## Best Practices

### When to Use Inheritance
* True "is-a" relationships
* Need for polymorphism
* Code reuse with proper design
* Interface implementation
* Framework extension points

### When to Avoid Inheritance
* "has-a" relationships (use composition)
* Implementation inheritance (prefer composition)
* Deep hierarchies (max 3-4 levels)
* When templates would work better
* Performance-critical code (consider alternatives)

## Related Topics
* Fundamentals: Basic class design
* Encapsulation: Access control in inheritance
* Design Patterns: Patterns using inheritance
* Modern C++: Override, final, default, delete keywords

