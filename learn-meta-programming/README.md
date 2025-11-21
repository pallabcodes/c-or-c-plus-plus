# C++ Metaprogramming and Reflection: Complete Guide for Bloomberg SDE-3

## Table of Contents
1. [Introduction: What is Metaprogramming?](#introduction-what-is-metaprogramming)
2. [Why Metaprogramming Matters](#why-metaprogramming-matters)
3. [Template Metaprogramming Basics](#template-metaprogramming-basics)
4. [Type Traits](#type-traits)
5. [SFINAE (Substitution Failure Is Not An Error)](#sfinae-substitution-failure-is-not-an-error)
6. [Variadic Templates](#variadic-templates)
7. [Template Specialization](#template-specialization)
8. [constexpr Metaprogramming](#constexpr-metaprogramming)
9. [C++20 Concepts](#c20-concepts)
10. [Reflection and Introspection](#reflection-and-introspection)
11. [Advanced Patterns](#advanced-patterns)
12. [Bloomberg-Style Metaprogramming](#bloomberg-style-metaprogramming)
13. [When to Use Metaprogramming](#when-to-use-metaprogramming)
14. [Best Practices](#best-practices)

## Introduction: What is Metaprogramming?

### Definition
**Metaprogramming** is writing code that generates or manipulates code at compile time. In C++, this primarily happens through:
- **Template metaprogramming**: Using templates to generate code
- **constexpr functions**: Compile-time computation
- **Type traits**: Inspecting and manipulating types
- **Reflection**: Introspecting code structure

### Key Characteristics
- **Compile-time execution**: Code runs during compilation, not runtime
- **Zero runtime overhead**: No performance cost at runtime
- **Type-safe**: Compiler enforces type correctness
- **Powerful but complex**: Can be difficult to understand and debug

### TypeScript Equivalents
TypeScript has powerful metaprogramming capabilities:
- **Type-level programming**: Conditional types, mapped types, template literal types
- **Generic constraints**: Type narrowing and type guards
- **Utility types**: `Partial<T>`, `Pick<T, K>`, `Omit<T, K>`
- **Decorators**: Metadata and code generation (experimental)

## Why Metaprogramming Matters

### Performance Benefits
- **Zero runtime cost**: Compile-time computation eliminates runtime overhead
- **Optimization opportunities**: Compiler can optimize better with compile-time information
- **Code generation**: Reduce boilerplate automatically

### Type Safety
- **Compile-time type checking**: Catch errors before runtime
- **Type constraints**: Enforce requirements on template parameters
- **Type transformations**: Create new types from existing ones

### Code Reusability
- **Generic algorithms**: Write once, use with many types
- **Policy-based design**: Compose behavior through templates
- **Library development**: Create flexible, reusable components

### Why Bloomberg Engineers Need Metaprogramming
- **Performance-critical systems**: Zero-overhead abstractions
- **Type-safe APIs**: Prevent errors in financial systems
- **Code generation**: Reduce boilerplate in large codebases
- **Library development**: Bloomberg Standard Library uses extensive metaprogramming

## Template Metaprogramming Basics

### Function Templates
```cpp
template<typename T>
T max(const T& a, const T& b) {
    return a > b ? a : b;
}
```

**TypeScript Equivalent:**
```typescript
function max<T>(a: T, b: T): T {
    return a > b ? a : b;
}
```

### Class Templates
```cpp
template<typename T>
class Vector {
    T* data_;
    size_t size_;
public:
    Vector(size_t size) : size_(size), data_(new T[size]) {}
    ~Vector() { delete[] data_; }
};
```

**TypeScript Equivalent:**
```typescript
class Vector<T> {
    private data: T[];
    private size: number;
    
    constructor(size: number) {
        this.size = size;
        this.data = new Array(size);
    }
}
```

### Template Parameters
```cpp
template<typename T, int N>
class Array {
    T data_[N];
public:
    T& operator[](size_t i) { return data_[i]; }
};
```

**TypeScript Equivalent:**
```typescript
// TypeScript doesn't have value template parameters
// But you can use const assertions and branded types
type Array<T, N extends number> = {
    readonly length: N;
    [index: number]: T;
}
```

## Type Traits

### Standard Type Traits
```cpp
#include <type_traits>

// Type queries
std::is_integral_v<int>;           // true
std::is_floating_point_v<double>; // true
std::is_same_v<int, int>;         // true
std::is_same_v<int, double>;      // false

// Type transformations
std::remove_const_t<const int>;   // int
std::add_pointer_t<int>;           // int*
std::decay_t<int[]>;              // int*
```

**TypeScript Equivalent:**
```typescript
// Type queries
type IsNumber<T> = T extends number ? true : false;
type IsSame<T, U> = T extends U ? (U extends T ? true : false) : false;

// Type transformations
type RemoveReadonly<T> = {
    -readonly [P in keyof T]: T[P];
};
type AddReadonly<T> = {
    readonly [P in keyof T]: T[P];
};
```

### Custom Type Traits
```cpp
template<typename T>
struct is_pointer {
    static constexpr bool value = false;
};

template<typename T>
struct is_pointer<T*> {
    static constexpr bool value = true;
};

template<typename T>
constexpr bool is_pointer_v = is_pointer<T>::value;
```

**TypeScript Equivalent:**
```typescript
type IsPointer<T> = T extends any[] ? false : T extends object ? true : false;
// More accurate:
type IsPointer<T> = T extends infer U | null | undefined 
    ? U extends object ? true : false 
    : false;
```

## SFINAE (Substitution Failure Is Not An Error)

### Basic SFINAE
```cpp
#include <type_traits>

template<typename T>
typename std::enable_if_t<std::is_integral_v<T>, T>
add_one(T value) {
    return value + 1;
}

template<typename T>
typename std::enable_if_t<std::is_floating_point_v<T>, T>
add_one(T value) {
    return value + 1.0;
}
```

**TypeScript Equivalent:**
```typescript
function addOne<T extends number>(value: T): T {
    return (value + 1) as T;
}

// Or with overloads:
function addOne(value: number): number;
function addOne(value: number): number {
    return value + 1;
}
```

### Expression SFINAE
```cpp
template<typename T>
auto get_size(const T& container) -> decltype(container.size()) {
    return container.size();
}

template<typename T>
auto get_size(const T& array) -> decltype(sizeof(array) / sizeof(array[0])) {
    return sizeof(array) / sizeof(array[0]);
}
```

**TypeScript Equivalent:**
```typescript
function getSize<T extends { length: number }>(container: T): number {
    return container.length;
}

function getSize<T extends any[]>(array: T): number {
    return array.length;
}
```

## Variadic Templates

### Basic Variadic Templates
```cpp
template<typename... Args>
void print(Args... args) {
    ((std::cout << args << " "), ...);
    std::cout << std::endl;
}
```

**TypeScript Equivalent:**
```typescript
function print(...args: any[]): void {
    console.log(...args);
}
```

### Fold Expressions (C++17)
```cpp
template<typename... Args>
auto sum(Args... args) {
    return (args + ...);
}

template<typename... Args>
bool all_true(Args... args) {
    return (args && ...);
}
```

**TypeScript Equivalent:**
```typescript
function sum(...args: number[]): number {
    return args.reduce((a, b) => a + b, 0);
}

function allTrue(...args: boolean[]): boolean {
    return args.every(x => x === true);
}
```

### Parameter Pack Expansion
```cpp
template<typename... Types>
class Tuple {
    std::tuple<Types...> data_;
public:
    template<size_t I>
    auto get() -> std::tuple_element_t<I, std::tuple<Types...>> {
        return std::get<I>(data_);
    }
};
```

**TypeScript Equivalent:**
```typescript
type Tuple<T extends readonly any[]> = {
    [K in keyof T]: T[K];
};

function get<T extends readonly any[], K extends keyof T>(
    tuple: T, 
    index: K
): T[K] {
    return tuple[index];
}
```

## Template Specialization

### Full Specialization
```cpp
template<typename T>
class TypeInfo {
public:
    static const char* name() { return "unknown"; }
};

template<>
class TypeInfo<int> {
public:
    static const char* name() { return "int"; }
};

template<>
class TypeInfo<double> {
public:
    static const char* name() { return "double"; }
};
```

**TypeScript Equivalent:**
```typescript
// TypeScript uses conditional types
type TypeName<T> = 
    T extends number ? "number" :
    T extends string ? "string" :
    T extends boolean ? "boolean" :
    "unknown";

// Or with function overloads
function typeName(value: number): "number";
function typeName(value: string): "string";
function typeName(value: boolean): "boolean";
function typeName(value: any): "unknown";
function typeName(value: any): string {
    return typeof value;
}
```

### Partial Specialization
```cpp
template<typename T>
class Container {
    // Generic implementation
};

template<typename T>
class Container<T*> {
    // Specialization for pointers
};

template<typename T, size_t N>
class Container<T[N]> {
    // Specialization for arrays
};
```

**TypeScript Equivalent:**
```typescript
// TypeScript uses conditional types
type Container<T> = T extends any[] 
    ? ArrayContainer<T>
    : T extends object
    ? ObjectContainer<T>
    : GenericContainer<T>;
```

## constexpr Metaprogramming

### constexpr Functions
```cpp
constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

constexpr int result = factorial(5);  // Computed at compile time
```

**TypeScript Equivalent:**
```typescript
// TypeScript doesn't have constexpr, but you can use const
const factorial = (n: number): number => 
    n <= 1 ? 1 : n * factorial(n - 1);

const result = factorial(5);  // Computed at runtime
```

### constexpr if (C++17)
```cpp
template<typename T>
constexpr auto get_value() {
    if constexpr (std::is_integral_v<T>) {
        return T{42};
    } else if constexpr (std::is_floating_point_v<T>) {
        return T{3.14};
    } else {
        return T{};
    }
}
```

**TypeScript Equivalent:**
```typescript
// TypeScript uses conditional types
type GetValue<T> = 
    T extends number 
        ? T extends infer U 
            ? U extends number 
                ? 42 
                : never 
            : never 
        : never;

// Or with function overloads
function getValue(): 42;
function getValue<T extends number>(): T extends number ? 42 : never;
function getValue(): any {
    return 42;
}
```

## C++20 Concepts

### Basic Concepts
```cpp
#include <concepts>

template<typename T>
concept Integral = std::integral<T>;

template<Integral T>
T add(T a, T b) {
    return a + b;
}
```

**TypeScript Equivalent:**
```typescript
// TypeScript uses generic constraints
function add<T extends number>(a: T, b: T): T {
    return (a + b) as T;
}
```

### Custom Concepts
```cpp
template<typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::convertible_to<T>;
};

template<Addable T>
T add(T a, T b) {
    return a + b;
}
```

**TypeScript Equivalent:**
```typescript
interface Addable {
    add(other: this): this;
}

function add<T extends Addable>(a: T, b: T): T {
    return a.add(b);
}
```

### Requires Clauses
```cpp
template<typename T>
requires std::totally_ordered<T>
T max(const T& a, const T& b) {
    return a > b ? a : b;
}
```

**TypeScript Equivalent:**
```typescript
function max<T extends number | string | bigint>(
    a: T, 
    b: T
): T {
    return a > b ? a : b;
}
```

## Reflection and Introspection

### C++20 Reflection (Experimental)
```cpp
#include <experimental/reflect>

template<typename T>
void print_type_info() {
    using type_info = std::experimental::reflect::get_typedef_v<T>;
    std::cout << type_info::name() << std::endl;
}
```

**TypeScript Equivalent:**
```typescript
// TypeScript has better reflection support
function printTypeInfo<T>(): void {
    console.log(typeof {} as T);
}

// Or with type guards
function isString(value: unknown): value is string {
    return typeof value === 'string';
}

function isNumber(value: unknown): value is number {
    return typeof value === 'number';
}
```

### Type Introspection
```cpp
template<typename T>
void inspect_type() {
    std::cout << "Is integral: " << std::is_integral_v<T> << std::endl;
    std::cout << "Is pointer: " << std::is_pointer_v<T> << std::endl;
    std::cout << "Size: " << sizeof(T) << std::endl;
}
```

**TypeScript Equivalent:**
```typescript
function inspectType<T>(): void {
    // TypeScript runtime type checking
    const value: unknown = {} as T;
    console.log("Type:", typeof value);
    console.log("Is string:", typeof value === 'string');
    console.log("Is number:", typeof value === 'number');
}
```

## Advanced Patterns

### CRTP (Curiously Recurring Template Pattern)
```cpp
template<typename Derived>
class Base {
public:
    void interface() {
        static_cast<Derived*>(this)->implementation();
    }
};

class Derived : public Base<Derived> {
public:
    void implementation() {
        // Implementation
    }
};
```

**TypeScript Equivalent:**
```typescript
// TypeScript uses mixins or abstract classes
abstract class Base {
    abstract implementation(): void;
    
    interface(): void {
        this.implementation();
    }
}

class Derived extends Base {
    implementation(): void {
        // Implementation
    }
}
```

### Expression Templates
```cpp
template<typename Lhs, typename Rhs>
class AddExpr {
    const Lhs& lhs_;
    const Rhs& rhs_;
public:
    AddExpr(const Lhs& lhs, const Rhs& rhs) 
        : lhs_(lhs), rhs_(rhs) {}
    
    auto operator[](size_t i) const {
        return lhs_[i] + rhs_[i];
    }
};
```

**TypeScript Equivalent:**
```typescript
// TypeScript doesn't have operator overloading
// But you can use method chaining
class AddExpr<T> {
    constructor(
        private lhs: T[],
        private rhs: T[]
    ) {}
    
    get(index: number): T {
        return (this.lhs[index] as any) + (this.rhs[index] as any);
    }
}
```

### Policy-Based Design
```cpp
template<typename AllocationPolicy>
class Container {
    AllocationPolicy allocator_;
public:
    void* allocate(size_t size) {
        return allocator_.allocate(size);
    }
};

struct MallocPolicy {
    void* allocate(size_t size) { return malloc(size); }
};

using MyContainer = Container<MallocPolicy>;
```

**TypeScript Equivalent:**
```typescript
// TypeScript uses interfaces and dependency injection
interface AllocationPolicy {
    allocate(size: number): void;
}

class Container {
    constructor(private allocator: AllocationPolicy) {}
    
    allocate(size: number): void {
        return this.allocator.allocate(size);
    }
}

class MallocPolicy implements AllocationPolicy {
    allocate(size: number): void {
        // Implementation
    }
}

const container = new Container(new MallocPolicy());
```

## Bloomberg-Style Metaprogramming

### Bloomberg Type Traits
```cpp
namespace Bloomberg {
    namespace bslmf {
        // Bloomberg-specific type traits
        template<typename T>
        using IsIntegral = std::is_integral<T>;
        
        template<typename T>
        using RemoveCvRef = std::remove_cvref_t<T>;
    }
}
```

### Bloomberg Concepts
```cpp
namespace Bloomberg {
    namespace bsls {
        template<typename T>
        concept BloombergType = requires {
            typename T::BloombergTag;
        };
    }
}
```

### Bloomberg Metaprogramming Patterns
- **Type erasure**: For performance-critical code
- **Policy-based design**: For flexible component composition
- **SFINAE**: For conditional compilation
- **constexpr**: For compile-time computation

## When to Use Metaprogramming

### Appropriate Uses
1. **Type-safe generic algorithms**: When you need type safety
2. **Compile-time computation**: When you can compute at compile time
3. **Code generation**: When you need to reduce boilerplate
4. **Library development**: When creating reusable components
5. **Performance optimization**: When you need zero-overhead abstractions

### When NOT to Use
1. **Simple cases**: When a regular function/class is sufficient
2. **Runtime polymorphism**: When you need runtime behavior
3. **Complex logic**: When metaprogramming makes code unreadable
4. **Debugging difficulty**: When you need easy debugging

## Best Practices

### 1. Prefer Concepts Over SFINAE (C++20)
```cpp
// Old way (SFINAE)
template<typename T>
typename std::enable_if_t<std::is_integral_v<T>, T>
add(T a, T b) { return a + b; }

// New way (Concepts)
template<std::integral T>
T add(T a, T b) { return a + b; }
```

### 2. Use constexpr When Possible
```cpp
// Compile-time computation
constexpr int result = factorial(5);
```

### 3. Document Complex Metaprogramming
```cpp
/// Computes factorial at compile time
/// @tparam N The number to compute factorial for
/// @return N! computed at compile time
template<int N>
constexpr int factorial() {
    return N * factorial<N - 1>();
}
```

### 4. Test Metaprogramming Code
```cpp
static_assert(factorial<5>() == 120);
static_assert(std::is_same_v<int, std::remove_const_t<const int>>);
```

### 5. Use Type Aliases for Readability
```cpp
template<typename T>
using RemoveCvRef = std::remove_cvref_t<T>;
```

## Summary

### Key Takeaways
1. **Metaprogramming = Code that generates code** at compile time
2. **Templates** are the primary mechanism for metaprogramming
3. **Type traits** inspect and manipulate types
4. **SFINAE** enables conditional compilation
5. **Concepts** provide better type constraints (C++20)
6. **constexpr** enables compile-time computation
7. **Use sparingly** - prefer simple solutions when possible

### Bloomberg-Specific
- Understand Bloomberg Standard Library metaprogramming patterns
- Know when to use metaprogramming vs. runtime solutions
- Follow Bloomberg coding standards for templates
- Test metaprogramming code thoroughly

This guide provides comprehensive coverage of metaprogramming at Bloomberg SDE-3 level. Focus on understanding when metaprogramming is appropriate and how to use it effectively.
