/**
 * Modern C++ Alternatives to Macros - JavaScript/TypeScript Developer Edition
 *
 * Modern C++ provides better alternatives to macros:
 * - constexpr: Compile-time constants and functions
 * - Templates: Type-safe generic code
 * - Inline functions: Type-checked function calls
 * - if constexpr: Compile-time conditionals
 * - Concepts (C++20): Type constraints
 *
 * In JS/TS, these concepts map to:
 * - constexpr constants = const (compile-time known values)
 * - Templates = Generic functions/types
 * - Inline functions = Regular functions (JS engines inline automatically)
 * - if constexpr = Build-time conditionals or type guards
 */

#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

// =============================================================================
// 1. CONSTEXPR INSTEAD OF #define CONSTANTS
// =============================================================================
// In JS/TS: const PI = 3.14159; (compile-time constant)

// OLD WAY: Macros
#define PI_MACRO 3.141592653589793
#define MAX_SIZE_MACRO 1024

// NEW WAY: constexpr (C++11+)
constexpr double PI = 3.141592653589793;
constexpr int MAX_SIZE = 1024;
constexpr int VERSION_MAJOR = 1;
constexpr int VERSION_MINOR = 0;

// constexpr variables are type-safe and scoped
namespace Constants {
    constexpr double PI = 3.141592653589793;
    constexpr int MAX_SIZE = 1024;
}

void demonstrate_constexpr_constants() {
    std::cout << "\n=== constexpr Constants vs Macros ===\n";

    // Both work similarly, but constexpr is better:
    double area1 = PI_MACRO * 10.0 * 10.0;
    double area2 = PI * 10.0 * 10.0;

    std::cout << "Area (macro): " << area1 << std::endl;
    std::cout << "Area (constexpr): " << area2 << std::endl;

    // constexpr advantages:
    // 1. Type-safe (PI is double, not just text)
    // 2. Scoped (can be in namespaces)
    // 3. Can be used in templates
    // 4. Better error messages

    // In JS/TS, you'd write:
    // const PI = 3.141592653589793;
    // const MAX_SIZE = 1024;
}

// =============================================================================
// 2. CONSTEXPR FUNCTIONS INSTEAD OF FUNCTION MACROS
// =============================================================================
// In JS/TS: const square = x => x * x; (function)

// OLD WAY: Macro function
#define SQUARE_MACRO(x) ((x) * (x))

// NEW WAY: constexpr function (C++11+)
constexpr int square(int x) {
    return x * x;
}

// Can be evaluated at compile time!
constexpr int result = square(5);  // Computed at compile time

// Works with different types (templates)
template<typename T>
constexpr T square_template(T x) {
    return x * x;
}

void demonstrate_constexpr_functions() {
    std::cout << "\n=== constexpr Functions vs Macros ===\n";

    int x = 5;
    int macro_result = SQUARE_MACRO(x);
    int func_result = square(x);
    double double_result = square_template(5.5);

    std::cout << "SQUARE_MACRO(5) = " << macro_result << std::endl;
    std::cout << "square(5) = " << func_result << std::endl;
    std::cout << "square_template(5.5) = " << double_result << std::endl;
    std::cout << "Compile-time result: " << result << std::endl;

    // constexpr advantages:
    // 1. Type-safe
    // 2. Evaluated once (no multiple evaluation problem)
    // 3. Can be used in constant expressions
    // 4. Better debugging support

    // In JS/TS, you'd write:
    // const square = x => x * x;
    // const result = square(5);
}

// =============================================================================
// 3. TEMPLATE FUNCTIONS INSTEAD OF TYPE-UNSAFE MACROS
// =============================================================================
// In JS/TS: function max<T>(a: T, b: T): T { return a > b ? a : b; }

// OLD WAY: Unsafe macro
#define MAX_MACRO(a, b) ((a) > (b) ? (a) : (b))

// NEW WAY: Template function (C++98+)
template<typename T>
constexpr T max(const T& a, const T& b) {
    return a > b ? a : b;
}

// Even better: C++20 concepts for type constraints
template<typename T>
requires std::totally_ordered<T>
constexpr T max_safe(const T& a, const T& b) {
    return a > b ? a : b;
}

void demonstrate_template_functions() {
    std::cout << "\n=== Template Functions vs Macros ===\n";

    int int_result = max(10, 20);
    double double_result = max(3.14, 2.71);
    std::string string_result = max(std::string("apple"), std::string("banana"));

    std::cout << "max(10, 20) = " << int_result << std::endl;
    std::cout << "max(3.14, 2.71) = " << double_result << std::endl;
    std::cout << "max(\"apple\", \"banana\") = " << string_result << std::endl;

    // Template advantages:
    // 1. Type-safe
    // 2. No multiple evaluation
    // 3. Works with any comparable type
    // 4. Better error messages

    // In JS/TS, you'd write:
    // function max<T>(a: T, b: T): T { return a > b ? a : b; }
    // const result = max(10, 20);
}

// =============================================================================
// 4. INLINE FUNCTIONS INSTEAD OF MACROS
// =============================================================================
// In JS/TS: Functions are automatically inlined by the engine

// OLD WAY: Macro
#define MIN_MACRO(a, b) ((a) < (b) ? (a) : (b))

// NEW WAY: Inline function
inline int min(int a, int b) {
    return a < b ? a : b;
}

// Template + inline for generic code
template<typename T>
inline T min_template(const T& a, const T& b) {
    return a < b ? a : b;
}

void demonstrate_inline_functions() {
    std::cout << "\n=== Inline Functions vs Macros ===\n";

    int a = 10, b = 20;
    int macro_result = MIN_MACRO(a, b);
    int func_result = min(a, b);
    double template_result = min_template(3.14, 2.71);

    std::cout << "MIN_MACRO(10, 20) = " << macro_result << std::endl;
    std::cout << "min(10, 20) = " << func_result << std::endl;
    std::cout << "min_template(3.14, 2.71) = " << template_result << std::endl;

    // Inline function advantages:
    // 1. Type-safe
    // 2. No multiple evaluation
    // 3. Can be debugged
    // 4. Compiler can optimize better

    // In JS/TS, you'd write:
    // function min(a, b) { return a < b ? a : b; }
    // The engine will inline it if beneficial
}

// =============================================================================
// 5. IF CONSTEXPR INSTEAD OF #ifdef
// =============================================================================
// In JS/TS: Build-time conditionals or type guards

// OLD WAY: Preprocessor conditional
#ifdef DEBUG
    void debugFunction() {
        std::cout << "Debug mode" << std::endl;
    }
#else
    void debugFunction() {
        // Empty in release
    }
#endif

// NEW WAY: if constexpr (C++17+)
template<bool Debug = false>
void debugFunctionModern() {
    if constexpr (Debug) {
        std::cout << "Debug mode" << std::endl;
    }
    // Empty in release - code is removed at compile time
}

void demonstrate_if_constexpr() {
    std::cout << "\n=== if constexpr vs #ifdef ===\n";

    debugFunction();
    debugFunctionModern<true>();   // Debug version
    debugFunctionModern<false>();   // Release version (no code generated)

    // if constexpr advantages:
    // 1. Type-safe
    // 2. Can be used in templates
    // 3. Better integration with C++ code
    // 4. No separate compilation needed

    // In JS/TS, you'd use:
    // const DEBUG = process.env.NODE_ENV !== 'production';
    // if (DEBUG) { console.log("Debug mode"); }
    // Or build-time conditionals in Webpack/Vite
}

// =============================================================================
// 6. CONSTEXPR IF FOR COMPILE-TIME BRANCHING
// =============================================================================
// In JS/TS: Type guards or build-time conditionals

template<typename T>
constexpr auto getValue() {
    if constexpr (std::is_integral_v<T>) {
        return T{42};
    } else if constexpr (std::is_floating_point_v<T>) {
        return T{3.14};
    } else {
        return T{};
    }
}

void demonstrate_constexpr_if() {
    std::cout << "\n=== constexpr if for Compile-Time Branching ===\n";

    constexpr int int_val = getValue<int>();
    constexpr double double_val = getValue<double>();

    std::cout << "getValue<int>() = " << int_val << std::endl;
    std::cout << "getValue<double>() = " << double_val << std::endl;

    // In JS/TS, you'd use:
    // function getValue<T>(): T {
    //     if (typeof T === 'number') {
    //         return 42 as T;
    //     }
    //     return {} as T;
    // }
}

// =============================================================================
// 7. VARIADIC TEMPLATES INSTEAD OF VARIADIC MACROS
// =============================================================================
// In JS/TS: Rest parameters (...args)

// OLD WAY: Variadic macro
#define LOG_MACRO(...) printf(__VA_ARGS__)

// NEW WAY: Variadic template (C++11+)
template<typename... Args>
void log(Args... args) {
    ((std::cout << args << " "), ...);
    std::cout << std::endl;
}

// Type-safe variadic function
template<typename... Args>
void logFormatted(const char* format, Args... args) {
    printf(format, args...);
}

void demonstrate_variadic_templates() {
    std::cout << "\n=== Variadic Templates vs Macros ===\n";

    log(1, 2, 3, "hello", 4.5);
    logFormatted("Value: %d, Name: %s\n", 42, "Bloomberg");

    // Variadic template advantages:
    // 1. Type-safe
    // 2. Better error messages
    // 3. Can be used in templates
    // 4. More flexible

    // In JS/TS, you'd write:
    // function log(...args) { console.log(...args); }
    // log(1, 2, 3, "hello", 4.5);
}

// =============================================================================
// 8. STRING_VIEW AND CONSTEXPR STRING OPERATIONS
// =============================================================================
// In JS/TS: Template literals and string operations

// OLD WAY: Stringification macro
#define STRINGIFY_MACRO(x) #x

// NEW WAY: constexpr string operations (C++17+)
template<typename T>
constexpr std::string typeName() {
    if constexpr (std::is_same_v<T, int>) return "int";
    else if constexpr (std::is_same_v<T, double>) return "double";
    else if constexpr (std::is_same_v<T, std::string>) return "string";
    else return "unknown";
}

void demonstrate_string_operations() {
    std::cout << "\n=== String Operations vs Macros ===\n";

    std::cout << "Type name for int: " << typeName<int>() << std::endl;
    std::cout << "Type name for double: " << typeName<double>() << std::endl;

    // In JS/TS, you'd write:
    // function typeName<T>(): string {
    //     return typeof T;
    // }
    // Or use template literals: `Type: ${typeof value}`
}

// =============================================================================
// 9. CONCEPTS INSTEAD OF MACRO TYPE CHECKS (C++20)
// =============================================================================
// In JS/TS: Type guards or type constraints

// OLD WAY: Macro type check (doesn't exist, but shows the idea)
// #define IS_INTEGRAL(type) ...

// NEW WAY: Concepts (C++20)
template<typename T>
concept Integral = std::is_integral_v<T>;

template<typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::convertible_to<T>;
};

template<Integral T>
T addIntegers(T a, T b) {
    return a + b;
}

template<Addable T>
T add(T a, T b) {
    return a + b;
}

void demonstrate_concepts() {
    std::cout << "\n=== Concepts vs Macros ===\n";

    int result1 = addIntegers(5, 10);
    double result2 = add(3.14, 2.71);
    std::string result3 = add(std::string("Hello"), std::string(" World"));

    std::cout << "addIntegers(5, 10) = " << result1 << std::endl;
    std::cout << "add(3.14, 2.71) = " << result2 << std::endl;
    std::cout << "add(\"Hello\", \" World\") = " << result3 << std::endl;

    // Concepts advantages:
    // 1. Type-safe
    // 2. Better error messages
    // 3. Compile-time checking
    // 4. More expressive

    // In JS/TS, you'd use:
    // type Addable = { add: (other: this) => this };
    // function add<T extends Addable>(a: T, b: T): T { return a.add(b); }
}

// =============================================================================
// 10. WHEN TO STILL USE MACROS
// =============================================================================
// Some things still need macros

// Header guards - still need macros
#ifndef MODERN_ALTERNATIVES_H
#define MODERN_ALTERNATIVES_H
// Or use #pragma once
#endif

// Platform-specific code - still need macros
#ifdef _WIN32
    // Windows-specific code
#else
    // Unix-specific code
#endif

// Feature flags - still need macros
#ifdef ENABLE_FEATURE
    // Feature code
#endif

// Debug assertions - macros are still useful
#ifdef DEBUG
    #define ASSERT(condition) \
        if (!(condition)) { \
            std::cerr << "Assertion failed: " << #condition << std::endl; \
            std::abort(); \
        }
#else
    #define ASSERT(condition) ((void)0)
#endif

void demonstrate_when_to_use_macros() {
    std::cout << "\n=== When to Still Use Macros ===\n";

    int value = 42;
    ASSERT(value > 0);  // Only active in debug builds

    std::cout << "Macros are still needed for:" << std::endl;
    std::cout << "1. Header guards (#ifndef/#define/#endif)" << std::endl;
    std::cout << "2. Platform-specific code (#ifdef _WIN32)" << std::endl;
    std::cout << "3. Feature flags (#ifdef ENABLE_FEATURE)" << std::endl;
    std::cout << "4. Conditional compilation" << std::endl;
    std::cout << "5. Debug assertions" << std::endl;

    // In JS/TS, you'd use:
    // - Build tools for platform-specific code
    // - Environment variables for feature flags
    // - Build-time conditionals in Webpack/Vite
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "Modern C++ Alternatives to Macros - JS/TS Developer Edition\n";
    std::cout << "===========================================================\n";

    demonstrate_constexpr_constants();
    demonstrate_constexpr_functions();
    demonstrate_template_functions();
    demonstrate_inline_functions();
    demonstrate_if_constexpr();
    demonstrate_constexpr_if();
    demonstrate_variadic_templates();
    demonstrate_string_operations();
    demonstrate_concepts();
    demonstrate_when_to_use_macros();

    std::cout << "\n=== Modern Alternatives Takeaways ===\n";
    std::cout << "1. constexpr constants > #define constants (type-safe, scoped)\n";
    std::cout << "2. constexpr functions > function macros (no multiple evaluation)\n";
    std::cout << "3. Template functions > type-unsafe macros (type-safe, generic)\n";
    std::cout << "4. Inline functions > macros (debuggable, optimizable)\n";
    std::cout << "5. if constexpr > #ifdef (type-safe, template-friendly)\n";
    std::cout << "6. Variadic templates > variadic macros (type-safe)\n";
    std::cout << "7. Concepts > macro type checks (C++20, type-safe)\n";
    std::cout << "8. Still use macros for: header guards, platform code, feature flags\n";
    std::cout << "9. Prefer modern C++ when possible\n";
    std::cout << "10. Macros are a last resort in modern C++\n";

    return 0;
}
