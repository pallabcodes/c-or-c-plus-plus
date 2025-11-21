# C++ Namespaces: Complete Guide for Bloomberg SDE-3

## Table of Contents
1. [Introduction and Why Namespaces Exist](#introduction-and-why-namespaces-exist)
2. [Basic Namespace Syntax](#basic-namespace-syntax)
3. [Using Declarations vs Using Directives](#using-declarations-vs-using-directives)
4. [Nested Namespaces](#nested-namespaces)
5. [Anonymous Namespaces](#anonymous-namespaces)
6. [Namespace Aliases](#namespace-aliases)
7. [Argument Dependent Lookup (ADL)](#argument-dependent-lookup-adl)
8. [Inline Namespaces](#inline-namespaces)
9. [Header File Organization](#header-file-organization)
10. [Best Practices for Large Codebases](#best-practices-for-large-codebases)
11. [Common Pitfalls and Gotchas](#common-pitfalls-and-gotchas)
12. [Performance Considerations](#performance-considerations)
13. [Testing with Namespaces](#testing-with-namespaces)
14. [Modern C++ Features](#modern-c-features)

## Introduction and Why Namespaces Exist

### The Problem: Name Collisions
Before namespaces, C++ suffered from the "global namespace pollution" problem. All functions, classes, and variables lived in a single global scope, leading to:

```cpp
// Without namespaces - potential conflicts
class Vector { /* ... */ };           // Math vector
class Vector { /* ... */ };           // Container vector
void sort() { /* ... */ }             // Generic sort
void sort() { /* ... */ }             // Database sort
```

### The Solution: Namespaces
Namespaces provide **logical grouping** and **prevent name collisions** by creating separate scopes:

```cpp
namespace math {
    class Vector { /* 2D/3D vector */ };
    void rotate(Vector& v) { /* ... */ }
}

namespace containers {
    class Vector { /* dynamic array */ };
    void sort(Vector& v) { /* ... */ }
}
```

### Why Bloomberg Engineers Need to Master Namespaces

1. **Large-Scale Development**: Bloomberg's codebase spans millions of lines across thousands of files
2. **Third-Party Integration**: Multiple libraries with overlapping names
3. **Team Collaboration**: Prevent naming conflicts between teams
4. **API Design**: Clean, intuitive interfaces for financial systems
5. **Maintainability**: Clear organization reduces cognitive load

## Basic Namespace Syntax

### Namespace Declaration
```cpp
// Declaration
namespace my_namespace {
    int value = 42;
    void function() { /* ... */ }
    class MyClass { /* ... */ };
}

// Can be reopened multiple times
namespace my_namespace {
    void another_function() { /* ... */ }
}
```

### Accessing Namespace Members
```cpp
// Fully qualified access
my_namespace::value = 100;
my_namespace::function();

// Using namespace directive (generally avoid in headers)
using namespace my_namespace;
value = 200;  // Now accessible directly
```

### Namespace Definition in Multiple Files
```cpp
// header.h
namespace bloomberg {
    namespace trading {
        class Order;
        void submitOrder(Order* order);
    }
}

// implementation.cpp
namespace bloomberg {
    namespace trading {
        void submitOrder(Order* order) {
            // Implementation
        }
    }
}
```

## Using Declarations vs Using Directives

### Using Declarations (`using` declaration)
Brings **specific** names into scope:

```cpp
namespace bloomberg {
    namespace analytics {
        class RiskCalculator { /* ... */ };
        void calculateVaR() { /* ... */ }
    }
}

using bloomberg::analytics::RiskCalculator;  // Only this class
using bloomberg::analytics::calculateVaR;    // Only this function

RiskCalculator calc;  // OK
calculateVaR();       // OK
// analytics::SomeOtherClass - NOT accessible
```

**Pros:**
- Precise control over what's imported
- Clear intent
- No pollution of enclosing scope

**Cons:**
- Verbose for multiple imports

### Using Directives (`using namespace`)
Brings **entire namespace** into scope:

```cpp
using namespace std;  // DANGER in global scope!

vector<int> v;       // OK
cout << "Hello";     // OK
```

**Pros:**
- Convenient for local use
- Less typing

**Cons:**
- Pollutes enclosing scope
- Can cause silent ambiguities
- Makes dependencies unclear

### Bloomberg Standard: Prefer Using Declarations
```cpp
// Preferred in Bloomberg codebase
using bloomberg::trading::Order;
using bloomberg::trading::Trade;

// Avoid this in headers
using namespace bloomberg::trading;
```

## Nested Namespaces

### C++17 Nested Namespace Definition
```cpp
// Old way (verbose)
namespace bloomberg {
    namespace trading {
        namespace execution {
            class OrderRouter { /* ... */ };
        }
    }
}

// C++17 way (clean)
namespace bloomberg::trading::execution {
    class OrderRouter { /* ... */ };
}
```

### Accessing Nested Namespaces
```cpp
// All these are equivalent
bloomberg::trading::execution::OrderRouter router1;
using namespace bloomberg::trading::execution;
OrderRouter router2;

using bloomberg::trading::execution::OrderRouter;
OrderRouter router3;
```

### Real-World Bloomberg Example
```cpp
namespace bloomberg::bdlp::service {
    // Bloomberg Data License Platform
    class EntitlementManager {
        // Manages data access permissions
    };
}

namespace bloomberg::emsx::api {
    // Execution Management System API
    class OrderManager {
        // Handles order execution
    };
}
```

## Anonymous Namespaces

### Purpose: File-Local Linkage
Anonymous namespaces make declarations **local to the translation unit**:

```cpp
// file1.cpp
namespace {
    int helper_function() { return 42; }
    class LocalClass { /* ... */ };
}

void public_function() {
    int result = helper_function();  // OK
    LocalClass obj;                  // OK
}

// file2.cpp
// helper_function and LocalClass are NOT visible here
```

### Equivalent to Static (but preferred)
```cpp
// Old way
static int helper_function() { return 42; }
static LocalClass obj;

// New way (preferred)
namespace {
    int helper_function() { return 42; }
    LocalClass obj;
}
```

### When to Use Anonymous Namespaces
1. **Implementation details** not needed in headers
2. **Helper functions** for a single file
3. **Internal classes** used only within one translation unit
4. **File-static variables** with complex initialization

## Namespace Aliases

### Creating Aliases for Convenience
```cpp
namespace bloomberg {
    namespace trading {
        namespace execution {
            class OrderProcessor { /* ... */ };
        }
    }
}

// Create alias
namespace bte = bloomberg::trading::execution;

bte::OrderProcessor processor;  // Shorter than full name
```

### Real-World Usage
```cpp
// Common in Bloomberg codebase
namespace bb = bloomberg;
namespace bbg = bloomberg;
namespace blp = bloomberg;

// Or for specific subsystems
namespace emsx = bloomberg::emsx;
namespace dapi = bloomberg::dapi;
```

### Template Specialization with Aliases
```cpp
template<typename T>
class Container { /* ... */ };

namespace mylib {
    template<typename T>
    class Vector { /* ... */ };
}

namespace mv = mylib;

// Use alias in specializations
template<>
class Container<mv::Vector<int>> {
    // Specialization for mylib::Vector<int>
};
```

## Argument Dependent Lookup (ADL)

### What is ADL?
ADL allows function calls to find functions in **associated namespaces** of arguments:

```cpp
namespace bloomberg {
    namespace trading {
        class Order { };

        void process(Order& order) {
            // Process order
        }
    }
}

bloomberg::trading::Order order;
process(order);  // ADL finds bloomberg::trading::process
```

### ADL Rules
Functions are found in namespaces associated with:
1. **Type of arguments** (including `this` for member calls)
2. **Template parameters** (for template specializations)

### ADL Example with Operators
```cpp
namespace math {
    class Complex {
        double real, imag;
    };

    Complex operator+(const Complex& a, const Complex& b) {
        return {a.real + b.real, a.imag + b.imag};
    }
}

math::Complex a{1, 2}, b{3, 4};
auto c = a + b;  // ADL finds math::operator+
```

### ADL and Templates
```cpp
namespace std {
    template<typename T>
    void swap(T& a, T& b) {
        T temp = move(a);
        a = move(b);
        b = move(temp);
    }
}

namespace bloomberg {
    class Trade {
        // Custom swap for efficiency
        friend void swap(Trade& a, Trade& b) {
            // Efficient swap implementation
        }
    };
}

bloomberg::Trade t1, t2;
swap(t1, t2);  // ADL finds bloomberg::swap, not std::swap
```

### ADL Gotchas
```cpp
namespace A { class X {}; void f(X); }
namespace B { void f(A::X); }  // Different function!

A::X obj;
f(obj);  // Which f? ADL finds both - AMBIGUOUS!
```

## Inline Namespaces

### C++11 Feature for Versioning
```cpp
namespace bloomberg {
    inline namespace v1 {
        class API { /* version 1 */ };
    }

    namespace v2 {
        class API { /* version 2 */ };
    }
}

// This uses v1::API (default)
bloomberg::API api;

// Explicit version selection
bloomberg::v2::API api2;
```

### Use Cases
1. **API Versioning**: Backward compatibility
2. **Experimental Features**: Easy enable/disable
3. **Platform-Specific Code**

### Bloomberg Example
```cpp
namespace bloomberg::bdem {
    inline namespace aggregate {
        // Current aggregate version
        class Aggregate { /* ... */ };
    }

    namespace aggregate_v2 {
        // Next version (not inline yet)
        class Aggregate { /* improved version */ };
    }
}
```

## Header File Organization

### Proper Header Structure
```cpp
// bloomberg/trading/order.h
#ifndef BLOOMBERG_TRADING_ORDER_H
#define BLOOMBERG_TRADING_ORDER_H

namespace bloomberg {
namespace trading {

// Forward declarations
class Portfolio;
class Account;

// Main class
class Order {
public:
    enum Type { MARKET, LIMIT, STOP };

    Order(Type type, double quantity, Portfolio* portfolio);

    // Methods...
private:
    Type type_;
    double quantity_;
    Portfolio* portfolio_;
};

// Free functions
Order* createMarketOrder(double quantity);
void submitOrder(Order* order);

}  // namespace trading
}  // namespace bloomberg

#endif  // BLOOMBERG_TRADING_ORDER_H
```

### Implementation File
```cpp
// bloomberg/trading/order.cpp
#include "order.h"
#include <stdexcept>

namespace bloomberg {
namespace trading {

Order::Order(Type type, double quantity, Portfolio* portfolio)
    : type_(type), quantity_(quantity), portfolio_(portfolio) {
    if (quantity <= 0) {
        throw std::invalid_argument("Quantity must be positive");
    }
}

Order* createMarketOrder(double quantity) {
    return new Order(Order::MARKET, quantity, nullptr);
}

void submitOrder(Order* order) {
    // Implementation
}

}  // namespace trading
}  // namespace bloomberg
```

## Best Practices for Large Codebases

### 1. Use Hierarchical Namespace Structure
```cpp
// Bloomberg-style hierarchy
namespace bloomberg {
    namespace bdem {        // Bloomberg Data Environment
        namespace aggregate {
            class Aggregate;
        }
    }
    namespace bsl {         // Bloomberg Standard Library
        namespace vector {
            class Vector;
        }
    }
    namespace emsx {        // Execution Management System
        namespace api {
            class OrderManager;
        }
    }
}
```

### 2. Prefer Using Declarations Over Using Directives
```cpp
// Good
using bloomberg::trading::Order;
using bloomberg::trading::Trade;

// Avoid in headers
// using namespace bloomberg::trading;
```

### 3. Use Namespace Aliases for Readability
```cpp
namespace bbg = bloomberg;
namespace emsx = bloomberg::emsx;
namespace dapi = bloomberg::dapi;
```

### 4. Never Put Using Directives in Headers
```cpp
// BAD - pollutes all including files
// header.h
using namespace std;

// GOOD - explicit about dependencies
// header.h
#include <vector>
#include <string>

// Explicit using declarations only when necessary
using std::vector;
using std::string;
```

### 5. Use Anonymous Namespaces for File-Local Code
```cpp
// file.cpp
namespace {
    class Helper {
        // Implementation details
    };

    Helper* createHelper() {
        return new Helper();
    }
}  // anonymous namespace
```

### 6. ADL-Aware Function Design
```cpp
namespace bloomberg {
    namespace math {
        class Matrix {
            // ...
        };

        // Put operators in same namespace as class
        Matrix operator+(const Matrix& a, const Matrix& b);
        Matrix operator*(const Matrix& a, const Matrix& b);
    }
}
```

## Common Pitfalls and Gotchas

### 1. Using Directive in Headers
```cpp
// bad_header.h - DON'T DO THIS
using namespace std;

// client.cpp
#include "bad_header.h"
// Now std is visible here - unexpected pollution
```

### 2. ADL Ambiguity
```cpp
namespace A { void f(int); }
namespace B { void f(int); }

using namespace A;
using namespace B;

f(42);  // ERROR: ambiguous - which f?
```

### 3. Template Specialization in Wrong Namespace
```cpp
namespace std {
    template<typename T> class vector;
}

namespace bloomberg {
    // WRONG - specialization must be in same namespace as primary template
    template<> class std::vector<MyClass>;  // ERROR
}

// CORRECT
namespace std {
    template<> class vector<bloomberg::MyClass> {
        // Specialization
    };
}
```

### 4. Friend Declarations and Namespaces
```cpp
namespace bloomberg {
    class Order {
        // WRONG - friend not in Order's namespace
        friend void external_function(Order&);  // external_function not found
    };
}

void external_function(bloomberg::Order& order) {
    // Implementation
}
```

### 5. Static Members and Qualified Names
```cpp
class bloomberg::trading::Order {
public:
    static int getCount() {
        return count_;
    }
private:
    static int count_;  // Declaration
};

// Definition must be qualified
int bloomberg::trading::Order::count_ = 0;
```

## Performance Considerations

### 1. Namespace Lookup Cost
- **Shallow namespaces**: Faster lookup
- **Deep nesting**: Slightly slower due to qualified name resolution
- **Using directives**: Can slow compilation due to increased scope

### 2. Anonymous Namespace vs Static
- **Anonymous namespace**: Zero runtime cost (same as static)
- **Preferred** over static for consistency with modern C++

### 3. ADL Performance
- ADL searches multiple namespaces
- Can be slower than explicit qualification
- Consider explicit qualification for performance-critical code

## Testing with Namespaces

### Unit Test Organization
```cpp
// test/order_test.cpp
#include <gtest/gtest.h>
#include "bloomberg/trading/order.h"

namespace {

// Test fixtures in anonymous namespace
class OrderTest : public ::testing::Test {
protected:
    bloomberg::trading::Order* order_;
    void SetUp() override {
        order_ = bloomberg::trading::createMarketOrder(100.0);
    }
    void TearDown() override {
        delete order_;
    }
};

TEST_F(OrderTest, Creation) {
    EXPECT_EQ(order_->getQuantity(), 100.0);
}

}  // anonymous namespace
```

### Mock Classes in Namespaces
```cpp
namespace bloomberg {
namespace trading {
namespace test {

class MockMarketData {
public:
    MOCK_METHOD(double, getPrice, (const std::string& symbol), ());
};

}  // namespace test
}  // namespace trading
}  // namespace bloomberg
```

## Modern C++ Features

### C++17: Nested Namespace Declarations
```cpp
// Old verbose way
namespace bloomberg {
    namespace trading {
        namespace execution {
            class Engine;
        }
    }
}

// New concise way
namespace bloomberg::trading::execution {
    class Engine;
}
```

### C++20: Likely/Unlikely Attributes (Namespace Related)
```cpp
namespace bloomberg::trading {
    bool [[unlikely]] isMarketClosed() {
        // Rare condition
        return false;
    }
}
```

### Modules (C++20) and Namespaces
```cpp
// Traditional header
// bloomberg/trading/order.h
export module bloomberg.trading.order;

namespace bloomberg::trading {
    export class Order {
        // ...
    };
}
```

This comprehensive guide covers namespaces at the depth expected of Bloomberg SDE-3 candidates. Focus on understanding the trade-offs between different approaches and applying best practices in large-scale development.
