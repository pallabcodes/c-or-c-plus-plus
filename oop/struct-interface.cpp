/*
 * Object-Oriented Programming: Structs and Interfaces
 *
 * Demonstrates struct usage and interface implementation using abstract classes.
 */
#include <iostream>
#include <string>
#include <cassert>

// Thread-safety: Not thread-safe (mutable state)
// Ownership: Owns string member
// Invariants: Name must be non-empty, Age > 0
// Failure modes: Undefined behavior if invariants violated
struct Person {
    std::string Name;
    int Age;

    // Thread-safety: Not thread-safe (may modify state via output)
    // Ownership: None
    // Invariants: Name must be non-empty, Age > 0
    // Failure modes: Undefined behavior if Name is empty or Age <= 0
    void Introduce() const {
        assert(!Name.empty() && "Name must be non-empty");
        assert(Age > 0 && "Age must be positive");
        std::cout << "Hello, my name is " << Name << " and I'm " << Age
                  << " years old." << std::endl;
    }
};

// Thread-safety: Not thread-safe (abstract interface)
// Ownership: Abstract base class, does not own derived objects
// Invariants: None
// Failure modes: None
class ISpeaker {
public:
    virtual void Speak() = 0;
    virtual ~ISpeaker() = default;
};

// Thread-safety: Not thread-safe (may modify state via output)
// Ownership: Owns no resources
// Invariants: None
// Failure modes: None
class Speaker : public ISpeaker {
public:
    // Thread-safety: Not thread-safe (may modify state via output)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void Speak() override {
        std::cout << "Speaking..." << std::endl;
    }
};

int main() {
    Person person;
    person.Name = "John Doe";
    person.Age = 30;
    person.Introduce();

    Speaker speaker;
    speaker.Speak();

    return 0;
}
