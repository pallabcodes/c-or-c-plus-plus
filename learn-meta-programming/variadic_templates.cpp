/**
 * Variadic Templates - TypeScript Developer Edition
 *
 * Variadic templates allow functions and classes to accept a variable number
 * of template parameters. Think of them as C++'s equivalent to TypeScript's
 * rest parameters, but operating at compile-time with type safety.
 *
 * In TypeScript: function sum(...args: number[]): number
 * In C++: template<typename... Args> auto sum(Args... args)
 *
 * Key differences:
 * - C++: Type-safe, each argument can be different type
 * - TypeScript: Runtime array, all elements same type (or union)
 * - C++: Compile-time expansion, zero overhead
 * - TypeScript: Runtime array operations
 */

#include <iostream>
#include <string>
#include <tuple>

// =============================================================================
// 1. BASIC VARIADIC FUNCTIONS
// =============================================================================
// In TypeScript: function print(...args: any[]): void

template<typename... Args>
void print(Args... args) {
    ((std::cout << args << " "), ...);
    std::cout << std::endl;
}

// TypeScript equivalent:
// function print(...args: any[]): void {
//     console.log(...args);
// }

void demonstrate_basic_variadic() {
    std::cout << "\n=== Basic Variadic Functions ===\n";

    print(1, 2, 3);
    print("Hello", "World", 42, 3.14);
    print();

    // In TypeScript, you'd write:
    // print(1, 2, 3);
    // print("Hello", "World", 42, 3.14);
}

// =============================================================================
// 2. FOLD EXPRESSIONS (C++17)
// =============================================================================
// In TypeScript: args.reduce((a, b) => a + b, 0)

template<typename... Args>
auto sum(Args... args) {
    return (args + ...);  // Binary right fold
}

template<typename... Args>
auto product(Args... args) {
    return (args * ...);  // Binary right fold
}

template<typename... Args>
bool all_true(Args... args) {
    return (args && ...);  // Logical AND fold
}

template<typename... Args>
bool any_true(Args... args) {
    return (args || ...);  // Logical OR fold
}

// TypeScript equivalent:
// function sum(...args: number[]): number {
//     return args.reduce((a, b) => a + b, 0);
// }
// function allTrue(...args: boolean[]): boolean {
//     return args.every(x => x === true);
// }

void demonstrate_fold_expressions() {
    std::cout << "\n=== Fold Expressions ===\n";

    std::cout << "sum(1, 2, 3, 4, 5) = " << sum(1, 2, 3, 4, 5) << std::endl;
    std::cout << "product(2, 3, 4) = " << product(2, 3, 4) << std::endl;
    std::cout << "all_true(true, true, true) = " << all_true(true, true, true) << std::endl;
    std::cout << "all_true(true, false, true) = " << all_true(true, false, true) << std::endl;
    std::cout << "any_true(false, false, true) = " << any_true(false, false, true) << std::endl;
}

// =============================================================================
// 3. VARIADIC CLASS TEMPLATES
// =============================================================================
// In TypeScript: class Tuple<T extends readonly any[]>

template<typename... Types>
class Tuple {
private:
    std::tuple<Types...> data_;

public:
    Tuple(Types... args) : data_(args...) {}

    template<size_t I>
    auto get() -> std::tuple_element_t<I, std::tuple<Types...>> {
        return std::get<I>(data_);
    }

    template<size_t I>
    auto get() const -> const std::tuple_element_t<I, std::tuple<Types...>> {
        return std::get<I>(data_);
    }
};

// TypeScript equivalent:
// type Tuple<T extends readonly any[]> = {
//     [K in keyof T]: T[K];
// };
// function get<T extends readonly any[], K extends keyof T>(
//     tuple: T, 
//     index: K
// ): T[K] {
//     return tuple[index];
// }

void demonstrate_variadic_class() {
    std::cout << "\n=== Variadic Class Templates ===\n";

    Tuple<int, std::string, double> tuple(42, "Hello", 3.14);
    std::cout << "tuple.get<0>() = " << tuple.get<0>() << std::endl;
    std::cout << "tuple.get<1>() = " << tuple.get<1>() << std::endl;
    std::cout << "tuple.get<2>() = " << tuple.get<2>() << std::endl;
}

// =============================================================================
// 4. PARAMETER PACK EXPANSION
// =============================================================================
// Expand parameter packs in different contexts

template<typename... Args>
void print_types() {
    ((std::cout << typeid(Args).name() << " "), ...);
    std::cout << std::endl;
}

template<typename... Args>
auto make_array(Args... args) -> std::array<int, sizeof...(Args)> {
    return {args...};
}

// TypeScript equivalent:
// function printTypes<T extends readonly any[]>(...args: T): void {
//     args.forEach(arg => console.log(typeof arg));
// }

void demonstrate_parameter_pack_expansion() {
    std::cout << "\n=== Parameter Pack Expansion ===\n";

    print_types<int, double, std::string>();

    auto arr = make_array(1, 2, 3, 4, 5);
    std::cout << "Array size: " << arr.size() << std::endl;
    for (auto val : arr) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
}

// =============================================================================
// 5. RECURSIVE VARIADIC TEMPLATES
// =============================================================================
// Process arguments recursively (before C++17 fold expressions)

template<typename T>
void print_recursive(T value) {
    std::cout << value << std::endl;
}

template<typename First, typename... Rest>
void print_recursive(First first, Rest... rest) {
    std::cout << first << " ";
    print_recursive(rest...);
}

// TypeScript equivalent:
// function printRecursive(...args: any[]): void {
//     if (args.length === 0) return;
//     console.log(args[0]);
//     printRecursive(...args.slice(1));
// }

void demonstrate_recursive_variadic() {
    std::cout << "\n=== Recursive Variadic Templates ===\n";

    print_recursive(1, 2, 3, "Hello", 4.5);
}

// =============================================================================
// 6. VARIADIC TEMPLATES WITH CONSTRAINTS
// =============================================================================
// Constrain variadic templates with type traits

template<typename... Args>
requires (std::is_arithmetic_v<Args> && ...)
auto arithmetic_sum(Args... args) {
    return (args + ...);
}

// TypeScript equivalent:
// function arithmeticSum<T extends number>(...args: T[]): number {
//     return args.reduce((a, b) => a + b, 0);
// }

void demonstrate_constrained_variadic() {
    std::cout << "\n=== Variadic Templates with Constraints ===\n";

    std::cout << "arithmetic_sum(1, 2, 3, 4) = " 
              << arithmetic_sum(1, 2, 3, 4) << std::endl;
    std::cout << "arithmetic_sum(1.5, 2.5, 3.5) = " 
              << arithmetic_sum(1.5, 2.5, 3.5) << std::endl;
}

// =============================================================================
// 7. VARIADIC TEMPLATE SPECIALIZATION
// =============================================================================
// Specialize for specific numbers of arguments

template<typename... Args>
struct Count {
    static constexpr size_t value = sizeof...(Args);
};

// Specialization for empty pack
template<>
struct Count<> {
    static constexpr size_t value = 0;
};

// TypeScript equivalent:
// type Count<T extends readonly any[]> = T['length'];
// const count: Count<[1, 2, 3]> = 3;

void demonstrate_variadic_specialization() {
    std::cout << "\n=== Variadic Template Specialization ===\n";

    std::cout << "Count<int, double, std::string>::value = " 
              << Count<int, double, std::string>::value << std::endl;
    std::cout << "Count<>::value = " << Count<>::value << std::endl;
}

// =============================================================================
// 8. FORWARDING VARIADIC ARGUMENTS
// =============================================================================
// Perfect forwarding with variadic templates

template<typename... Args>
void forward_to_print(Args&&... args) {
    print(std::forward<Args>(args)...);
}

// TypeScript equivalent:
// function forwardToPrint(...args: any[]): void {
//     print(...args);
// }

void demonstrate_forwarding() {
    std::cout << "\n=== Forwarding Variadic Arguments ===\n";

    forward_to_print(1, 2, 3, "Hello");
}

// =============================================================================
// 9. VARIADIC TEMPLATES FOR DELEGATION
// =============================================================================
// Delegate to constructors or functions

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// TypeScript equivalent:
// function makeUnique<T, A extends any[]>(
//     constructor: new (...args: A) => T,
//     ...args: A
// ): T {
//     return new constructor(...args);
// }

class Example {
    int value_;
public:
    Example(int v) : value_(v) {}
    int get() const { return value_; }
};

void demonstrate_delegation() {
    std::cout << "\n=== Variadic Templates for Delegation ===\n";

    auto ptr = make_unique<Example>(42);
    std::cout << "ptr->get() = " << ptr->get() << std::endl;
}

// =============================================================================
// 10. COMPLEX VARIADIC PATTERNS
// =============================================================================
// Advanced patterns with variadic templates

template<typename... Types>
struct TypeList {};

template<typename List>
struct Size;

template<template<typename...> class List, typename... Types>
struct Size<List<Types...>> {
    static constexpr size_t value = sizeof...(Types);
};

// TypeScript equivalent:
// type TypeList<T extends readonly any[]> = T;
// type Size<T extends readonly any[]> = T['length'];

void demonstrate_complex_patterns() {
    std::cout << "\n=== Complex Variadic Patterns ===\n";

    using MyList = TypeList<int, double, std::string>;
    std::cout << "Size<MyList>::value = " << Size<MyList>::value << std::endl;
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "Variadic Templates - TypeScript Developer Edition\n";
    std::cout << "=================================================\n";

    demonstrate_basic_variadic();
    demonstrate_fold_expressions();
    demonstrate_variadic_class();
    demonstrate_parameter_pack_expansion();
    demonstrate_recursive_variadic();
    demonstrate_constrained_variadic();
    demonstrate_variadic_specialization();
    demonstrate_forwarding();
    demonstrate_delegation();
    demonstrate_complex_patterns();

    std::cout << "\n=== Key Takeaways for TypeScript Developers ===\n";
    std::cout << "1. Variadic templates = Rest parameters (...args)\n";
    std::cout << "2. Fold expressions = Array.reduce() operations\n";
    std::cout << "3. Parameter pack expansion = Spread operator (...)\n";
    std::cout << "4. Recursive templates = Recursive functions\n";
    std::cout << "5. Type-safe: Each argument can be different type\n";
    std::cout << "6. Compile-time: Zero runtime overhead\n";
    std::cout << "7. C++17 fold expressions are more concise\n";
    std::cout << "8. TypeScript rest params are runtime arrays\n";
    std::cout << "9. C++ variadic templates are more powerful\n";
    std::cout << "10. Perfect forwarding preserves value categories\n";

    return 0;
}
