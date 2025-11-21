# Smart Pointers Standards

## Overview
Smart pointers provide automatic memory management through RAII (Resource Acquisition Is Initialization). This document defines standards for implementing and using production grade smart pointers in C++.

## RAII Principle

### Resource Acquisition Is Initialization
* **Acquisition**: Acquire resource in constructor
* **Initialization**: Resource is ready after construction
* **Destruction**: Release resource in destructor
* **Exception safety**: Guaranteed cleanup on exceptions
* **Rationale**: RAII ensures proper resource management

## Smart Pointer Types

### unique_ptr
* **Ownership**: Exclusive ownership
* **Move semantics**: Movable but not copyable
* **Performance**: Zero overhead over raw pointers
* **Applications**: Single owner scenarios
* **Rationale**: Efficient exclusive ownership

### shared_ptr
* **Ownership**: Shared ownership with reference counting
* **Copy semantics**: Copyable, reference counted
* **Performance**: Small overhead for reference counting
* **Applications**: Shared ownership scenarios
* **Rationale**: Safe shared ownership

### weak_ptr
* **Ownership**: Non owning reference
* **Use case**: Break circular references
* **Performance**: Small overhead
* **Applications**: Observer pattern, caches
* **Rationale**: Break cycles, avoid dangling pointers

## unique_ptr Usage

### Basic Usage
```cpp
// Ownership: unique_ptr owns the object
// Thread safety: Not thread safe (unique_ptr itself)
// Lifetime: Object lifetime tied to unique_ptr lifetime
// Complexity: O(1) time, O(1) space
void process_data() {
    std::unique_ptr<int> ptr = std::make_unique<int>(42);
    // Use ptr
    // Automatically deleted when ptr goes out of scope
}
```

### Custom Deleter
```cpp
// Ownership: unique_ptr owns the object with custom deleter
// Thread safety: Not thread safe
// Lifetime: Object lifetime tied to unique_ptr lifetime
struct FileDeleter {
    void operator()(FILE* f) {
        if (f) {
            fclose(f);
        }
    }
};

void process_file() {
    std::unique_ptr<FILE, FileDeleter> file(fopen("data.txt", "r"));
    // Use file
    // Automatically closed when file goes out of scope
}
```

## shared_ptr Usage

### Basic Usage
```cpp
// Ownership: shared_ptr shares ownership
// Thread safety: Thread safe reference counting
// Lifetime: Object lifetime tied to last shared_ptr
// Complexity: O(1) time, O(1) space (with reference counting overhead)
void process_shared_data() {
    std::shared_ptr<int> ptr1 = std::make_shared<int>(42);
    {
        std::shared_ptr<int> ptr2 = ptr1;  // Reference count = 2
        // Use ptr2
    }  // ptr2 destroyed, reference count = 1
    // Use ptr1
}  // ptr1 destroyed, reference count = 0, object deleted
```

### Circular Reference Prevention
```cpp
// Ownership: Use weak_ptr to break cycles
// Thread safety: Thread safe
struct Node {
    std::shared_ptr<Node> next;
    std::weak_ptr<Node> prev;  // Use weak_ptr to break cycle
};

void create_list() {
    auto node1 = std::make_shared<Node>();
    auto node2 = std::make_shared<Node>();
    
    node1->next = node2;
    node2->prev = node1;  // weak_ptr breaks cycle
}
```

## make_unique and make_shared

### make_unique
* **Advantages**: Exception safe, no new/delete
* **Performance**: Same as new
* **Usage**: Prefer make_unique over new
* **Rationale**: Exception safety and clarity

### make_shared
* **Advantages**: Exception safe, single allocation
* **Performance**: More efficient than shared_ptr(new T)
* **Usage**: Prefer make_shared over shared_ptr(new T)
* **Rationale**: Exception safety and efficiency

### Example
```cpp
// Good: Use make_unique/make_shared
auto ptr1 = std::make_unique<int>(42);
auto ptr2 = std::make_shared<int>(42);

// Bad: Avoid new/delete
int* ptr3 = new int(42);  // BAD: Manual management
delete ptr3;
```

## Implementation Standards

### Correctness
* **Ownership**: Document ownership semantics
* **Lifetime**: Document object lifetime
* **Thread safety**: Document thread safety guarantees
* **Rationale**: Correctness is critical

### Performance
* **Overhead**: Minimize smart pointer overhead
* **Allocation**: Use make_shared for efficiency
* **Benchmarking**: Benchmark smart pointer performance
* **Rationale**: Performance is critical

## Testing Requirements

### Unit Tests
* **Operations**: Test smart pointer operations
* **Ownership**: Test ownership transfer
* **Lifetime**: Test object lifetime
* **Edge cases**: Test boundary conditions
* **Rationale**: Comprehensive testing ensures correctness

## Research Papers and References

### Smart Pointers
* "The C++ Programming Language" (Stroustrup) - Smart pointers
* "Effective Modern C++" (Meyers) - Smart pointer best practices
* Standard C++ Library smart pointer implementation

## Implementation Checklist

- [ ] Understand RAII principle
- [ ] Use unique_ptr for exclusive ownership
- [ ] Use shared_ptr for shared ownership
- [ ] Use weak_ptr to break cycles
- [ ] Prefer make_unique/make_shared
- [ ] Document ownership semantics
- [ ] Write comprehensive unit tests
- [ ] Benchmark performance

