/*
 * Object-Oriented Programming: Method Overloading
 *
 * Demonstrates method overloading - defining multiple methods with the same name
 * but different parameters.
 */
#include <iostream>
#include <cassert>

// Thread-safety: Not thread-safe (mutable state)
// Ownership: Owns no resources
// Invariants: None
// Failure modes: Undefined behavior if arr is null and size > 0
class MyClass {
public:
    // Thread-safety: Not thread-safe (calls overloaded method)
    // Ownership: Borrows arr (read-only)
    // Invariants: arr must be valid for size elements if size > 0
    // Failure modes: Undefined behavior if arr is null and size > 0
    int doSomething(int arr[], int size) {
        assert(arr != nullptr || size == 0);
        return doSomething(arr, size, true);
    }

    // Thread-safety: Not thread-safe (may modify state)
    // Ownership: Borrows arr (read-only)
    // Invariants: arr must be valid for size elements if size > 0
    // Failure modes: Undefined behavior if arr is null and size > 0
    int doSomething(int arr[], int size, bool flag) {
        assert(arr != nullptr || size == 0);
        // Implementation
        return 0;
    }
};

int main() {
    int myArray[] = {1, 2, 3};
    MyClass obj;
    int result = obj.doSomething(myArray, 3);
    std::cout << "Result: " << result << std::endl;
    return 0;
}
