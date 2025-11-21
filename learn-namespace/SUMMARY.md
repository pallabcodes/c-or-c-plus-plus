# Complete C++ Namespaces Guide - Bloomberg SDE-3 Level

## Overview

This comprehensive guide covers **everything** about C++ namespaces at the level expected of Bloomberg SDE-3 candidates. The content is organized into practical examples and real-world patterns used in large-scale financial systems.

## Files Created

### 📚 Core Documentation
- **`README.md`** - Complete theoretical foundation and concepts
- **`SUMMARY.md`** - This quick reference guide

### 💡 Practical Examples
- **`basic_examples.cpp`** - Fundamental namespace syntax and usage
- **`advanced_examples.cpp`** - Nested, anonymous namespaces, and aliases
- **`adl_examples.cpp`** - Argument Dependent Lookup (ADL) in depth
- **`modern_cpp_namespaces.cpp`** - C++11/17/20 namespace features

### 🏢 Bloomberg-Style Patterns
- **`bloomberg_standards.cpp`** - Real Bloomberg namespace hierarchies and patterns
- **`header_organization.h/.cpp/.hpp`** - Proper header organization with namespaces
- **`namespace_pitfalls.cpp`** - Common mistakes and how to avoid them

## Key Concepts by Category

### 🔍 **Why Namespaces Exist**
- Prevent **name collisions** in large codebases
- Provide **logical grouping** of related functionality
- Enable **API versioning** and **backward compatibility**
- Support **team collaboration** without naming conflicts

### 📝 **Basic Syntax**
```cpp
// Declaration
namespace bloomberg {
    class Security { /* ... */ };
    void processTrade() { /* ... */ }
}

// Usage
bloomberg::Security sec;
bloomberg::processTrade();

// Using declarations (preferred)
using bloomberg::Security;
Security sec;  // OK

// Using directives (avoid in headers)
using namespace std;  // OK in implementation files only
```

### 🏗️ **Advanced Features**
```cpp
// C++17 nested namespaces
namespace bloomberg::trading::execution {
    class SmartRouter { /* ... */ };
}

// Anonymous namespaces (file-local)
namespace {
    int helper_counter = 0;  // Internal linkage
}

// Namespace aliases
namespace bt = bloomberg::trading;
bt::Order order;
```

### 🔎 **Argument Dependent Lookup (ADL)**
```cpp
namespace math {
    class Complex { /* ... */ };
    Complex operator+(const Complex& a, const Complex& b);  // ADL-friendly
}

math::Complex a, b;
auto sum = a + b;  // ADL finds math::operator+
```

### 🏛️ **Bloomberg-Style Hierarchy**
```cpp
namespace bloomberg {
    namespace bsl { }    // Bloomberg Standard Library
    namespace bdem { }   // Bloomberg Data Environment
    namespace emsx { }   // Execution Management System
    namespace dapi { }   // Data API
    namespace bpipe { }  // Bloomberg Pipeline
}
```

## Critical Best Practices

### ✅ **DOs**
- Use **fully qualified names** in headers
- Prefer **using declarations** over using directives
- Place **operators in same namespace** as their operands (ADL)
- Use **anonymous namespaces** for file-local code
- Create **namespace aliases** for deep hierarchies
- **Document** namespace purposes and ownership

### ❌ **DON'Ts**
- **Never** use `using namespace` in header files
- Don't put **template specializations** in wrong namespaces
- Avoid **ADL ambiguity** by careful function naming
- Don't **pollute global namespace** with using directives
- Never use **hyphens** in namespace names

## Common Pitfalls to Avoid

1. **Header Pollution**: `using namespace std;` in headers
2. **ADL Ambiguity**: Multiple namespaces with identical function names
3. **Template Specialization**: Wrong namespace for specializations
4. **Friend Functions**: Must be in same namespace as class
5. **Inline Namespaces**: Can break backward compatibility silently
6. **Anonymous Namespace Sharing**: Variables not shared across TUs

## Modern C++ Features

### C++11
- **Inline namespaces** for API versioning
- **Scoped enums** (`enum class`) for type safety

### C++17
- **Nested namespace definitions**: `namespace A::B::C { }`
- **constexpr** in namespaces
- **Structured bindings** with namespace functions

### Modern Patterns
- **Optional<T>** for potentially missing values
- **Smart pointers** with RAII
- **Type-safe** programming practices

## Bloomberg-Specific Patterns

### Namespace Structure
```
bloomberg::
├── bsl::        (Standard Library)
├── bdem::       (Data Environment)
├── emsx::       (Execution Management)
├── dapi::       (Data API)
├── bpipe::      (Data Pipeline)
└── [business]:: (Specific business domains)
```

### Coding Standards
- **Hierarchical** namespace organization
- **Namespace aliases** for readability (`bsl`, `emsx`)
- **Interface segregation** (separate API from implementation)
- **RAII** for resource management
- **Exception safety** considerations

## Testing Namespaces

```cpp
// Unit test organization
namespace bloomberg::trading::test {
    class OrderTest : public ::testing::Test {
        // Test fixtures in test namespace
    };
}

// Anonymous namespace in test files
namespace {
    class TestHelper {
        // File-local test utilities
    };
}
```

## Performance Considerations

- **Namespace lookup**: Shallow hierarchies are faster
- **ADL overhead**: Can search multiple namespaces
- **Compilation time**: Using directives can slow compilation
- **Anonymous namespace**: Zero runtime cost (like static)

## Interview Preparation Tips

### Key Topics to Master
1. **ADL mechanics** and when it applies
2. **Inline namespaces** for versioning
3. **Template specialization** rules
4. **Header organization** principles
5. **Bloomberg coding standards**

### Common Interview Questions
- Why do namespaces exist?
- What's ADL and how does it work?
- When should you use anonymous namespaces?
- How do you organize large codebases with namespaces?
- What's wrong with `using namespace std;` in headers?

## Quick Reference

### Creating Namespaces
```cpp
// Basic
namespace mylib { /* ... */ }

// Nested (C++17)
namespace mylib::subsystem::component { /* ... */ }

// Inline (for versioning)
inline namespace v1 { /* ... */ }
```

### Using Namespaces
```cpp
// Fully qualified (preferred in headers)
mylib::MyClass obj;

// Using declaration (OK locally)
using mylib::MyClass;
MyClass obj;

// Alias (for deep hierarchies)
namespace ml = mylib;
ml::MyClass obj;
```

### ADL-Friendly Code
```cpp
namespace mylib {
    class Widget { };
    Widget operator+(const Widget& a, const Widget& b);
    void process(Widget& w);
}
```

This guide provides comprehensive coverage of namespaces at Bloomberg SDE-3 level. Study each example file thoroughly and practice applying these patterns in your code.
