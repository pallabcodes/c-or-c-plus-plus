# TypeScript vs Modern C++: Comprehensive Comparison

## Quick Answer: Does C++ Have Generics?

**Yes!** C++ has **templates**, which are more powerful than TypeScript generics:
- **C++ Templates**: Compile-time code generation, zero runtime overhead
- **TypeScript Generics**: Type checking only, erased at runtime

## Table of Contents
1. [Generics/Templates Comparison](#genericstemplates-comparison)
2. [What TypeScript Has That C++ Doesn't](#what-typescript-has-that-c-doesnt)
3. [What C++ Has That TypeScript Doesn't](#what-c-has-that-typescript-doesnt)
4. [Feature-by-Feature Comparison](#feature-by-feature-comparison)
5. [When to Use Each](#when-to-use-each)

## Generics/Templates Comparison

### C++ Templates
```cpp
// Function template
template<typename T>
T max(const T& a, const T& b) {
    return a > b ? a : b;
}

// Class template
template<typename T>
class Vector {
    T* data_;
    size_t size_;
public:
    Vector(size_t size) : size_(size), data_(new T[size]) {}
    ~Vector() { delete[] data_; }
};

// Value template parameters (C++ only!)
template<int N>
class Array {
    int data_[N];
public:
    constexpr size_t size() const { return N; }
};

// Variadic templates
template<typename... Args>
void print(Args... args) {
    ((std::cout << args << " "), ...);
}
```

### TypeScript Generics
```typescript
// Generic function
function max<T>(a: T, b: T): T {
    return a > b ? a : b;
}

// Generic class
class Vector<T> {
    private data: T[];
    private size: number;
    
    constructor(size: number) {
        this.size = size;
        this.data = new Array(size);
    }
}

// TypeScript doesn't have value parameters
// Can't do: class Array<N extends number>

// Rest parameters (runtime, not compile-time)
function print(...args: any[]): void {
    console.log(...args);
}
```

### Key Differences

| Feature | C++ Templates | TypeScript Generics |
|---------|---------------|---------------------|
| **When evaluated** | Compile-time | Type-checking time (compile-time for TS) |
| **Runtime presence** | Generates actual code | Erased at runtime |
| **Performance** | Zero overhead | No runtime cost (but no code generation) |
| **Value parameters** | ✅ `template<int N>` | ❌ Not available |
| **Type constraints** | Concepts (C++20) | `extends` keyword |
| **Specialization** | ✅ Full and partial | ❌ No (use conditional types) |
| **Code generation** | ✅ Different code per type | ❌ Same code, type-checked |

## What TypeScript Has That C++ Doesn't

### 1. **Structural Typing (Duck Typing)**
```typescript
// TypeScript: If it quacks like a duck, it's a duck
interface Duck {
    quack(): void;
}

function makeQuack(duck: Duck) {
    duck.quack();
}

// Any object with quack() method works
makeQuack({ quack: () => console.log("Quack!") });
```

```cpp
// C++: Nominal typing - must explicitly inherit or match exactly
class Duck {
public:
    virtual void quack() = 0;
};

void makeQuack(Duck& duck) {
    duck.quack();
}

// Must inherit from Duck - can't use unrelated types
class MyDuck : public Duck {
    void quack() override { std::cout << "Quack!" << std::endl; }
};
```

### 2. **Union Types**
```typescript
// TypeScript: Union types
type StringOrNumber = string | number;

function process(value: StringOrNumber) {
    if (typeof value === 'string') {
        return value.toUpperCase();
    } else {
        return value * 2;
    }
}
```

```cpp
// C++: Must use std::variant (C++17) or tagged unions
#include <variant>

using StringOrNumber = std::variant<std::string, int>;

void process(const StringOrNumber& value) {
    if (std::holds_alternative<std::string>(value)) {
        auto str = std::get<std::string>(value);
        // Process string
    } else {
        auto num = std::get<int>(value);
        // Process number
    }
}
```

### 3. **Intersection Types**
```typescript
// TypeScript: Intersection types
type A = { a: number };
type B = { b: string };
type C = A & B;  // { a: number, b: string }
```

```cpp
// C++: Must use multiple inheritance or composition
struct A { int a; };
struct B { std::string b; };
struct C : public A, public B {};  // Multiple inheritance
```

### 4. **Mapped Types**
```typescript
// TypeScript: Transform types
type Readonly<T> = {
    readonly [P in keyof T]: T[P];
};

type Partial<T> = {
    [P in keyof T]?: T[P];
};

type Pick<T, K extends keyof T> = {
    [P in K]: T[P];
};
```

```cpp
// C++: No direct equivalent - must use templates and type traits
// Can achieve similar with template specialization but more verbose
```

### 5. **Template Literal Types**
```typescript
// TypeScript: String manipulation at type level
type EventName = "click" | "scroll" | "hover";
type HandlerName<T extends EventName> = `on${Capitalize<T>}`;
// Result: "onClick" | "onScroll" | "onHover"
```

```cpp
// C++: Not available - strings are runtime values
```

### 6. **Conditional Types**
```typescript
// TypeScript: Type-level conditionals
type IsArray<T> = T extends any[] ? true : false;
type Flatten<T> = T extends (infer U)[] ? U : T;
```

```cpp
// C++: Use template specialization
template<typename T>
struct IsArray {
    static constexpr bool value = false;
};

template<typename T>
struct IsArray<T[]> {
    static constexpr bool value = true;
};
```

### 7. **Type Guards**
```typescript
// TypeScript: Runtime type narrowing
function isString(value: unknown): value is string {
    return typeof value === 'string';
}

function process(value: unknown) {
    if (isString(value)) {
        // TypeScript knows value is string here
        console.log(value.toUpperCase());
    }
}
```

```cpp
// C++: Use type traits and if constexpr
template<typename T>
constexpr bool is_string_v = std::is_same_v<T, std::string>;

template<typename T>
void process(T value) {
    if constexpr (is_string_v<T>) {
        // Compile-time check
        std::cout << value << std::endl;
    }
}
```

### 8. **Discriminated Unions**
```typescript
// TypeScript: Tagged unions with type narrowing
type Result<T> = 
    | { success: true; data: T }
    | { success: false; error: string };

function handleResult<T>(result: Result<T>) {
    if (result.success) {
        // TypeScript knows result.data exists
        console.log(result.data);
    } else {
        // TypeScript knows result.error exists
        console.log(result.error);
    }
}
```

```cpp
// C++: Use std::variant with custom types
struct Success { int data; };
struct Error { std::string message; };
using Result = std::variant<Success, Error>;

void handleResult(const Result& result) {
    if (std::holds_alternative<Success>(result)) {
        auto success = std::get<Success>(result);
        std::cout << success.data << std::endl;
    } else {
        auto error = std::get<Error>(result);
        std::cout << error.message << std::endl;
    }
}
```

### 9. **Utility Types**
```typescript
// TypeScript: Built-in utility types
type Partial<T> = { [P in keyof T]?: T[P] };
type Required<T> = { [P in keyof T]-?: T[P] };
type Readonly<T> = { readonly [P in keyof T]: T[P] };
type Record<K extends keyof any, T> = { [P in K]: T };
type Exclude<T, U> = T extends U ? never : T;
type Extract<T, U> = T extends U ? T : never;
type Omit<T, K extends keyof any> = Pick<T, Exclude<keyof T, K>>;
```

```cpp
// C++: Must implement manually with type traits
// More verbose, less ergonomic
```

### 10. **Better Reflection**
```typescript
// TypeScript: Excellent reflection support
type Keys<T> = keyof T;
type Values<T> = T[keyof T];
type Entries<T> = { [K in keyof T]: [K, T[K]] }[keyof T][];

interface Person {
    name: string;
    age: number;
}

type PersonKeys = Keys<Person>;  // "name" | "age"
type PersonValues = Values<Person>;  // string | number
```

```cpp
// C++: Limited reflection (C++20 reflection TS is experimental)
// Must use type traits and manual work
```

## What C++ Has That TypeScript Doesn't

### 1. **Value Template Parameters**
```cpp
// C++: Can use values as template parameters
template<int N>
class Array {
    int data_[N];  // Size known at compile-time
public:
    constexpr size_t size() const { return N; }
};

Array<10> arr;  // Compile-time size
```

```typescript
// TypeScript: Can't use values as type parameters
// Can only use types, not values
```

### 2. **Template Specialization**
```cpp
// C++: Full and partial specialization
template<typename T>
class Container {
    // Generic implementation
};

template<>
class Container<int> {
    // Specialized for int
};

template<typename T>
class Container<T*> {
    // Specialized for pointers
};
```

```typescript
// TypeScript: No specialization
// Must use conditional types or function overloads
```

### 3. **Compile-Time Code Generation**
```cpp
// C++: Templates generate actual code
template<typename T>
T add(T a, T b) {
    return a + b;
}

// Generates: int add(int a, int b) { return a + b; }
// Generates: double add(double a, double b) { return a + b; }
// Different functions for each type!
```

```typescript
// TypeScript: Generics are type-checked but erased
function add<T>(a: T, b: T): T {
    return a + b;
}

// Runtime: function add(a, b) { return a + b; }
// Same function for all types
```

### 4. **constexpr (Compile-Time Computation)**
```cpp
// C++: True compile-time computation
constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

constexpr int result = factorial(5);  // Computed at compile-time!
```

```typescript
// TypeScript: const is runtime (not compile-time)
const factorial = (n: number): number => 
    n <= 1 ? 1 : n * factorial(n - 1);

const result = factorial(5);  // Computed at runtime
```

### 5. **Operator Overloading**
```cpp
// C++: Can overload operators
class Vector {
    int x_, y_;
public:
    Vector operator+(const Vector& other) const {
        return Vector(x_ + other.x_, y_ + other.y_);
    }
};

Vector a(1, 2), b(3, 4);
Vector c = a + b;  // Custom + operator
```

```typescript
// TypeScript: No operator overloading
// Must use methods: a.add(b)
```

### 6. **Multiple Inheritance**
```cpp
// C++: Multiple inheritance
class A { /* ... */ };
class B { /* ... */ };
class C : public A, public B { /* ... */ };
```

```typescript
// TypeScript: Single inheritance only
// Can use mixins but not true multiple inheritance
```

### 7. **RAII (Resource Management)**
```cpp
// C++: Automatic resource management
class File {
    FILE* file_;
public:
    File(const char* name) : file_(fopen(name, "r")) {}
    ~File() { if (file_) fclose(file_); }  // Automatic cleanup
};
```

```typescript
// TypeScript: Manual resource management or try-finally
// No deterministic destructors
```

### 8. **Move Semantics**
```cpp
// C++: Move semantics for performance
class LargeObject {
    std::vector<int> data_;
public:
    LargeObject(LargeObject&& other) noexcept 
        : data_(std::move(other.data_)) {}  // Move, don't copy
};
```

```typescript
// TypeScript: No move semantics
// Everything is reference-based (but can't explicitly move)
```

### 9. **Memory Layout Control**
```cpp
// C++: Control memory layout
struct Packed {
    char a;
    int b;
    char c;
} __attribute__((packed));  // Control padding
```

```typescript
// TypeScript: No control over memory layout
// JavaScript engine decides
```

### 10. **Zero-Cost Abstractions**
```cpp
// C++: Templates have zero runtime cost
template<typename T>
T max(const T& a, const T& b) {
    return a > b ? a : b;
}

// Compiles to: inline int max(int a, int b) { return a > b ? a : b; }
// Zero overhead!
```

```typescript
// TypeScript: Generics have no runtime cost (erased)
// But can't generate optimized code per type
```

### 11. **SFINAE (Substitution Failure Is Not An Error)**
```cpp
// C++: SFINAE for conditional compilation
template<typename T>
typename std::enable_if_t<std::is_integral_v<T>, T>
process(T value) {
    return value * 2;
}

template<typename T>
typename std::enable_if_t<std::is_floating_point_v<T>, T>
process(T value) {
    return value * 1.5;
}
```

```typescript
// TypeScript: Function overloads (less powerful)
function process(value: number): number;
function process(value: number): number {
    return typeof value === 'number' ? value * 2 : value * 1.5;
}
```

### 12. **Concepts (C++20)**
```cpp
// C++: Concepts for better type constraints
template<std::integral T>
T add(T a, T b) {
    return a + b;
}

// Much cleaner than SFINAE!
```

```typescript
// TypeScript: Generic constraints (less powerful)
function add<T extends number>(a: T, b: T): T {
    return (a + b) as T;
}
```

### 13. **Variadic Templates with Fold Expressions**
```cpp
// C++: Powerful variadic templates
template<typename... Args>
auto sum(Args... args) {
    return (args + ...);  // Fold expression
}
```

```typescript
// TypeScript: Rest parameters (runtime)
function sum(...args: number[]): number {
    return args.reduce((a, b) => a + b, 0);
}
```

### 14. **Template Metaprogramming**
```cpp
// C++: Full template metaprogramming
template<int N>
struct Factorial {
    static constexpr int value = N * Factorial<N - 1>::value;
};

template<>
struct Factorial<0> {
    static constexpr int value = 1;
};

constexpr int fact5 = Factorial<5>::value;  // Computed at compile-time!
```

```typescript
// TypeScript: Limited type-level programming
// Can't compute values at compile-time
```

## Feature-by-Feature Comparison

### Type System

| Feature | C++ | TypeScript |
|---------|-----|------------|
| **Static typing** | ✅ | ✅ |
| **Type inference** | ✅ (auto, C++11) | ✅ (Excellent) |
| **Structural typing** | ❌ (Nominal) | ✅ (Duck typing) |
| **Union types** | ✅ (std::variant, C++17) | ✅ (Native) |
| **Intersection types** | ✅ (Multiple inheritance) | ✅ (Native) |
| **Type erasure** | ✅ (std::any, void*) | ✅ (Runtime) |
| **Null safety** | ❌ (nullptr) | ✅ (Strict null checks) |

### Generics/Templates

| Feature | C++ | TypeScript |
|---------|-----|------------|
| **Generics/Templates** | ✅ (Templates) | ✅ (Generics) |
| **Type parameters** | ✅ | ✅ |
| **Value parameters** | ✅ | ❌ |
| **Constraints** | ✅ (Concepts, C++20) | ✅ (extends) |
| **Specialization** | ✅ (Full & partial) | ❌ |
| **Code generation** | ✅ (Compile-time) | ❌ (Erased) |
| **Variadic** | ✅ (Templates) | ✅ (Rest params) |

### Metaprogramming

| Feature | C++ | TypeScript |
|---------|-----|------------|
| **Compile-time computation** | ✅ (constexpr) | ❌ |
| **Type-level programming** | ✅ (TMP) | ✅ (Conditional types) |
| **Reflection** | ⚠️ (Limited, experimental) | ✅ (Excellent) |
| **Mapped types** | ❌ | ✅ |
| **Template literals** | ❌ | ✅ |
| **Type guards** | ⚠️ (if constexpr) | ✅ (Runtime) |

### Language Features

| Feature | C++ | TypeScript |
|---------|-----|------------|
| **Operator overloading** | ✅ | ❌ |
| **Multiple inheritance** | ✅ | ❌ |
| **Garbage collection** | ❌ | ✅ (JS runtime) |
| **RAII** | ✅ | ❌ |
| **Move semantics** | ✅ | ❌ |
| **Memory control** | ✅ | ❌ |
| **Async/await** | ✅ (C++20 coroutines) | ✅ |
| **Decorators** | ❌ | ✅ (Experimental) |

## When to Use Each

### Use C++ When:
- **Performance is critical** - Zero-overhead abstractions
- **Memory control needed** - RAII, manual memory management
- **System programming** - Operating systems, drivers, embedded
- **Compile-time computation** - constexpr, template metaprogramming
- **Operator overloading** - Mathematical libraries, DSLs
- **Value template parameters** - Fixed-size arrays, compile-time sizes

### Use TypeScript When:
- **Web development** - Browser APIs, Node.js
- **Rapid prototyping** - Faster development cycle
- **Type safety with flexibility** - Structural typing, unions
- **Better reflection** - Type introspection, mapped types
- **Developer experience** - Better error messages, tooling
- **Runtime flexibility** - Dynamic types, runtime checks

## Summary

### C++ Strengths:
- ✅ **Performance**: Zero-overhead abstractions, compile-time code generation
- ✅ **Control**: Memory layout, resource management, move semantics
- ✅ **Power**: Template metaprogramming, constexpr, operator overloading
- ✅ **Value parameters**: Template parameters can be values, not just types

### TypeScript Strengths:
- ✅ **Type system**: Structural typing, unions, intersections, mapped types
- ✅ **Reflection**: Excellent type introspection and manipulation
- ✅ **Developer experience**: Better error messages, tooling, ergonomics
- ✅ **Flexibility**: Runtime type checks, discriminated unions, type guards

### Key Insight:
- **C++ templates** = Code generation (different code per type)
- **TypeScript generics** = Type checking (same code, type-checked)

Both are powerful, but serve different purposes:
- **C++**: Performance and control
- **TypeScript**: Developer experience and type safety
