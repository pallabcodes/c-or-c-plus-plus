# Complete C++ Metaprogramming and Reflection Guide - Bloomberg SDE-3 Level

## Overview

This comprehensive guide covers **everything** about C++ metaprogramming and reflection at the level expected of Bloomberg SDE-3 candidates. The content includes practical examples with TypeScript equivalents to help developers coming from TypeScript.

## Files Created

### 📚 Core Documentation
- **`README.md`** - Complete theoretical foundation and concepts
- **`SUMMARY.md`** - This quick reference guide

### 💡 Practical Examples
- **`basic_templates.cpp`** - Fundamental template metaprogramming
- **`type_traits_sfinae.cpp`** - Type traits and SFINAE patterns
- **`variadic_templates.cpp`** - Variadic templates and parameter packs
- **`constexpr_metaprogramming.cpp`** - Compile-time computation
- **`concepts_cpp20.cpp`** - C++20 concepts and constraints
- **`reflection_introspection.cpp`** - Reflection and type introspection
- **`advanced_patterns.cpp`** - CRTP, expression templates, policy-based design
- **`bloomberg_patterns.cpp`** - Bloomberg-specific metaprogramming patterns

## Key Concepts by Category

### 🔍 **What is Metaprogramming?**
- **Code that generates code** at compile-time
- **Zero runtime overhead** - computations happen during compilation
- **Type-safe** - compiler enforces correctness
- **Powerful but complex** - requires deep understanding

### 📝 **Template Metaprogramming**
```cpp
// Function templates
template<typename T>
T max(const T& a, const T& b) { return a > b ? a : b; }

// Class templates
template<typename T>
class Vector { /* ... */ };

// Value parameters (C++ only)
template<int N>
class Array { /* ... */ };
```

### 🏗️ **Type Traits and SFINAE**
```cpp
// Type queries
std::is_integral_v<int>;           // true
std::is_same_v<int, int>;         // true

// Type transformations
std::remove_const_t<const int>;   // int
std::add_pointer_t<int>;           // int*

// SFINAE
template<typename T>
typename std::enable_if_t<std::is_integral_v<T>, T>
add_one(T value) { return value + 1; }
```

### 🔎 **Variadic Templates**
```cpp
// Variadic functions
template<typename... Args>
void print(Args... args) { ((std::cout << args << " "), ...); }

// Fold expressions (C++17)
template<typename... Args>
auto sum(Args... args) { return (args + ...); }
```

### 🏛️ **C++20 Concepts**
```cpp
// Standard concepts
template<std::integral T>
T add(T a, T b) { return a + b; }

// Custom concepts
template<typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::convertible_to<T>;
};
```

### 💻 **constexpr Metaprogramming**
```cpp
// constexpr functions
constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

// if constexpr (C++17)
template<typename T>
constexpr auto get_value() {
    if constexpr (std::is_integral_v<T>) {
        return T{42};
    }
}
```

## Critical Best Practices

### ✅ **DOs**
- **Use concepts over SFINAE** (C++20)
- **Prefer constexpr** for compile-time computation
- **Document complex metaprogramming** thoroughly
- **Test with static_assert** for compile-time validation
- **Follow Bloomberg naming conventions** (BSL_, BSLS_)

### ❌ **DON'Ts**
- **Don't overuse metaprogramming** - prefer simple solutions
- **Don't use SFINAE** when concepts are available
- **Don't ignore readability** - complex metaprogramming is hard to maintain
- **Don't skip testing** - metaprogramming bugs are compile-time errors

## Common Patterns

### 1. **CRTP (Curiously Recurring Template Pattern)**
```cpp
template<typename Derived>
class Base {
    void interface() {
        static_cast<Derived*>(this)->implementation();
    }
};
```

### 2. **Expression Templates**
```cpp
template<typename Lhs, typename Rhs>
class AddExpr {
    auto operator[](size_t i) const {
        return lhs_[i] + rhs_[i];
    }
};
```

### 3. **Policy-Based Design**
```cpp
template<typename AllocationPolicy>
class Container {
    AllocationPolicy allocator_;
};
```

### 4. **Type Erasure**
```cpp
class TypeErasure {
    struct Concept { virtual ~Concept() = default; };
    std::unique_ptr<Concept> object_;
};
```

## Bloomberg-Specific Patterns

### Naming Conventions
- `BSL_` - Bloomberg Standard Library
- `BSLS_` - Bloomberg Standard Library Support
- `BSLMF_` - Bloomberg Standard Library Metaprogramming Foundation

### Common Bloomberg Patterns
```cpp
// Bloomberg type traits
Bloomberg::bslmf::IsIntegral<T>
Bloomberg::bslmf::RemoveCvRef<T>

// Bloomberg concepts
Bloomberg::bsls::BloombergType<T>
Bloomberg::bsls::Serializable<T>
```

## TypeScript Equivalents

| C++ Metaprogramming | TypeScript Equivalent |
|---------------------|----------------------|
| `template<typename T>` | `function f<T>()` or `class C<T>` |
| `std::is_integral_v<T>` | `T extends number` |
| `std::enable_if_t` | Function overloads or conditional types |
| `template<typename... Args>` | `function f(...args: any[])` |
| `constexpr` | `const` (but runtime, not compile-time) |
| `if constexpr` | Conditional types or type guards |
| Concepts | Generic constraints `<T extends number>` |
| Type traits | Conditional types, mapped types |
| SFINAE | Function overloads |

## When to Use Metaprogramming

### Appropriate Uses
1. **Type-safe generic algorithms** - When you need type safety
2. **Compile-time computation** - When you can compute at compile-time
3. **Code generation** - When you need to reduce boilerplate
4. **Library development** - When creating reusable components
5. **Performance optimization** - When you need zero-overhead abstractions

### When NOT to Use
1. **Simple cases** - When a regular function/class is sufficient
2. **Runtime polymorphism** - When you need runtime behavior
3. **Complex logic** - When metaprogramming makes code unreadable
4. **Debugging difficulty** - When you need easy debugging

## Interview Preparation Tips

### Key Topics to Master
1. **Template metaprogramming** mechanics
2. **Type traits** and SFINAE patterns
3. **Variadic templates** and fold expressions
4. **C++20 concepts** vs SFINAE
5. **constexpr** metaprogramming
6. **Bloomberg patterns** and conventions

### Common Interview Questions
- What is template metaprogramming?
- Explain SFINAE and when to use it
- What are concepts and why are they better than SFINAE?
- How does constexpr differ from const?
- What is CRTP and when would you use it?
- How do variadic templates work?

## Quick Reference

### Template Basics
```cpp
template<typename T>           // Type parameter
template<int N>                // Value parameter (C++ only)
template<typename... Args>     // Variadic template
```

### Type Traits
```cpp
std::is_integral_v<T>          // Type query
std::remove_const_t<T>         // Type transformation
std::enable_if_t<condition, T> // SFINAE helper
```

### Concepts (C++20)
```cpp
template<std::integral T>      // Standard concept
template<Addable T>            // Custom concept
requires std::totally_ordered<T> // Requires clause
```

### constexpr
```cpp
constexpr int value = 42;      // Compile-time constant
constexpr int func(int n) { }  // Compile-time function
if constexpr (condition) { }   // Compile-time if (C++17)
```

This guide provides comprehensive coverage of metaprogramming at Bloomberg SDE-3 level. Study each example file thoroughly and practice applying these patterns in your code.
