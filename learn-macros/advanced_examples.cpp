/**
 * Advanced Macro Examples - JavaScript/TypeScript Developer Edition
 *
 * This file demonstrates advanced macro features:
 * - Variadic macros (variable number of arguments)
 * - Complex stringification and concatenation
 * - Macro metaprogramming patterns
 * - Advanced debugging macros
 *
 * In JS/TS, these patterns would be implemented as:
 * - Rest parameters: function(...args)
 * - Template literals: `string ${variable}`
 * - Higher-order functions: functions that return functions
 */

#include <iostream>
#include <string>
#include <vector>
#include <cstdarg>
#include <cstdio>

// =============================================================================
// 1. VARIADIC MACROS (...)
// =============================================================================
// In JS/TS: function log(...args) { console.log(...args); }
// In C++: #define LOG(...) printf(__VA_ARGS__)

// Simple variadic macro
#define LOG(...) printf(__VA_ARGS__)
#define LOG_NEWLINE(...) printf(__VA_ARGS__); printf("\n")

// Variadic macro with format string
#define DEBUG_LOG(format, ...) \
    printf("[DEBUG] " format "\n", __VA_ARGS__)

#define ERROR_LOG(format, ...) \
    fprintf(stderr, "[ERROR] " format "\n", __VA_ARGS__)

void demonstrate_variadic_macros() {
    std::cout << "\n=== Variadic Macros ===\n";

    // Simple logging
    LOG("Value: %d\n", 42);
    LOG("Name: %s, Age: %d\n", "John", 30);

    // Debug logging with format
    DEBUG_LOG("Processing %d items", 100);
    DEBUG_LOG("User %s logged in with ID %d", "john_doe", 12345);

    // Error logging
    ERROR_LOG("Failed to connect: %s", "Connection timeout");

    // In JS/TS, you'd write:
    // function log(...args) { console.log(...args); }
    // function debugLog(format, ...args) { console.log(`[DEBUG] ${format}`, ...args); }
    // log("Value:", 42);
    // debugLog("Processing %d items", 100);
}

// =============================================================================
// 2. ADVANCED STRINGIFICATION
// =============================================================================
// Creating debug macros that print variable names and values

#define PRINT_VAR(x) \
    std::cout << #x << " = " << x << std::endl

#define PRINT_VAR_TYPE(x, type) \
    std::cout << #x << " (" << #type << ") = " << x << std::endl

#define PRINT_PAIR(a, b) \
    std::cout << #a << " = " << a << ", " << #b << " = " << b << std::endl

#define PRINT_ARRAY(arr, size) \
    do { \
        std::cout << #arr << " = ["; \
        for (int i = 0; i < (size); ++i) { \
            std::cout << arr[i]; \
            if (i < (size) - 1) std::cout << ", "; \
        } \
        std::cout << "]" << std::endl; \
    } while(0)

void demonstrate_advanced_stringification() {
    std::cout << "\n=== Advanced Stringification ===\n";

    int counter = 42;
    double price = 150.25;
    std::string name = "Bloomberg";

    PRINT_VAR(counter);
    PRINT_VAR(price);
    PRINT_VAR(name);

    PRINT_VAR_TYPE(counter, int);
    PRINT_VAR_TYPE(price, double);

    PRINT_PAIR(counter, price);

    int numbers[] = {1, 2, 3, 4, 5};
    PRINT_ARRAY(numbers, 5);

    // In JS/TS, you'd write:
    // const counter = 42;
    // console.log(`counter = ${counter}`);
    // console.log(`counter (number) = ${counter}`);
}

// =============================================================================
// 3. TOKEN CONCATENATION PATTERNS
// =============================================================================
// Creating identifiers dynamically

#define MAKE_GETTER(name, type) \
    type get_##name() const { return name##_; }

#define MAKE_SETTER(name, type) \
    void set_##name(type value) { name##_ = value; }

#define MAKE_PROPERTY(name, type) \
    MAKE_GETTER(name, type) \
    MAKE_SETTER(name, type)

// Example class using these macros
class Order {
public:
    MAKE_PROPERTY(price, double)
    MAKE_PROPERTY(quantity, int)
    MAKE_PROPERTY(symbol, std::string)

private:
    double price_;
    int quantity_;
    std::string symbol_;
};

void demonstrate_token_concatenation_patterns() {
    std::cout << "\n=== Token Concatenation Patterns ===\n";

    Order order;
    order.set_price(150.25);
    order.set_quantity(100);
    order.set_symbol("AAPL");

    std::cout << "Order: " << order.get_symbol()
              << ", Price: $" << order.get_price()
              << ", Quantity: " << order.get_quantity() << std::endl;

    // In JS/TS, you'd use:
    // class Order {
    //     get price() { return this._price; }
    //     set price(value) { this._price = value; }
    // }
    // Or use decorators or code generation tools
}

// =============================================================================
// 4. CONDITIONAL MACROS
// =============================================================================
// Macros that behave differently based on conditions

#define SAFE_DIVIDE(a, b) \
    ((b) != 0 ? ((a) / (b)) : (0))

#define CLAMP(value, min, max) \
    ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

#define BETWEEN(value, min, max) \
    ((value) >= (min) && (value) <= (max))

void demonstrate_conditional_macros() {
    std::cout << "\n=== Conditional Macros ===\n";

    int result1 = SAFE_DIVIDE(100, 5);
    std::cout << "SAFE_DIVIDE(100, 5) = " << result1 << std::endl;

    int result2 = SAFE_DIVIDE(100, 0);
    std::cout << "SAFE_DIVIDE(100, 0) = " << result2 << " (safe)" << std::endl;

    int clamped = CLAMP(150, 0, 100);
    std::cout << "CLAMP(150, 0, 100) = " << clamped << std::endl;

    bool inRange = BETWEEN(50, 0, 100);
    std::cout << "BETWEEN(50, 0, 100) = " << (inRange ? "true" : "false") << std::endl;

    // In JS/TS, you'd write:
    // const safeDivide = (a, b) => b !== 0 ? a / b : 0;
    // const clamp = (value, min, max) => value < min ? min : value > max ? max : value;
}

// =============================================================================
// 5. LOOP MACROS
// =============================================================================
// Macros that generate loop code

#define FOR_EACH_INT(i, start, end) \
    for (int i = (start); i < (end); ++i)

#define FOR_EACH_REVERSE_INT(i, start, end) \
    for (int i = (end) - 1; i >= (start); --i)

#define REPEAT(n) \
    for (int _repeat_counter = 0; _repeat_counter < (n); ++_repeat_counter)

void demonstrate_loop_macros() {
    std::cout << "\n=== Loop Macros ===\n";

    std::cout << "FOR_EACH_INT(0, 5): ";
    FOR_EACH_INT(i, 0, 5) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::cout << "FOR_EACH_REVERSE_INT(0, 5): ";
    FOR_EACH_REVERSE_INT(i, 0, 5) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::cout << "REPEAT(3): ";
    REPEAT(3) {
        std::cout << "Hello ";
    }
    std::cout << std::endl;

    // In JS/TS, you'd write:
    // for (let i = 0; i < 5; i++) { ... }
    // for (let i = 4; i >= 0; i--) { ... }
    // for (let i = 0; i < 3; i++) { console.log("Hello"); }
}

// =============================================================================
// 6. ASSERTION MACROS
// =============================================================================
// Debug assertions with file and line information

#define ASSERT(condition) \
    ((condition) ? (void)0 : \
     (std::cerr << "Assertion failed: " << #condition \
                << " in " << __FILE__ << ":" << __LINE__ << std::endl, \
      std::abort()))

#define ASSERT_MSG(condition, message) \
    ((condition) ? (void)0 : \
     (std::cerr << "Assertion failed: " << #condition \
                << "\nMessage: " << message \
                << "\nFile: " << __FILE__ << ":" << __LINE__ << std::endl, \
      std::abort()))

#define ASSERT_EQUAL(a, b) \
    ((a) == (b) ? (void)0 : \
     (std::cerr << "Assertion failed: " << #a << " == " << #b \
                << "\n  " << #a << " = " << (a) \
                << "\n  " << #b << " = " << (b) \
                << "\nFile: " << __FILE__ << ":" << __LINE__ << std::endl, \
      std::abort()))

void demonstrate_assertion_macros() {
    std::cout << "\n=== Assertion Macros ===\n";

    int value = 42;
    ASSERT(value > 0);  // This passes

    // Uncomment to see assertion failure:
    // ASSERT(value < 0);  // This would abort with error message

    ASSERT_EQUAL(value, 42);  // This passes

    // In JS/TS, you'd write:
    // function assert(condition, message) {
    //     if (!condition) {
    //         throw new Error(`Assertion failed: ${message}`);
    //     }
    // }
    // assert(value > 0, "Value must be positive");
}

// =============================================================================
// 7. PERFORMANCE MACROS
// =============================================================================
// Macros for performance-critical code

#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

#define NO_INLINE __attribute__((noinline))
#define ALWAYS_INLINE __attribute__((always_inline)) inline

void demonstrate_performance_macros() {
    std::cout << "\n=== Performance Macros ===\n";

    int value = 42;

    // Branch prediction hints (GCC/Clang specific)
    if (LIKELY(value > 0)) {
        std::cout << "Likely branch taken" << std::endl;
    }

    if (UNLIKELY(value < 0)) {
        std::cout << "Unlikely branch taken" << std::endl;
    }

    // In JS/TS, you don't have direct equivalents, but:
    // - V8 and other engines optimize based on runtime behavior
    // - You can't give explicit hints like this
}

// =============================================================================
// 8. TYPE-SAFE MACROS (Using Templates)
// =============================================================================
// Combining macros with templates for type safety

#define TYPE_SAFE_MAX(type) \
    template<> \
    type max<type>(const type& a, const type& b) { \
        return a > b ? a : b; \
    }

// In practice, you'd use templates directly, but this shows the pattern
template<typename T>
T max(const T& a, const T& b) {
    return a > b ? a : b;
}

void demonstrate_type_safe_patterns() {
    std::cout << "\n=== Type-Safe Patterns ===\n";

    int a = 10, b = 20;
    double x = 3.14, y = 2.71;

    std::cout << "max(10, 20) = " << max(a, b) << std::endl;
    std::cout << "max(3.14, 2.71) = " << max(x, y) << std::endl;

    // In JS/TS, you'd write:
    // function max<T>(a: T, b: T): T { return a > b ? a : b; }
    // const result = max(10, 20);
}

// =============================================================================
// 9. DEBUG MACROS WITH LEVELS
// =============================================================================
// Conditional debug output based on debug level

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 0
#endif

#define DEBUG_LEVEL_TRACE 1
#define DEBUG_LEVEL_DEBUG 2
#define DEBUG_LEVEL_INFO 3
#define DEBUG_LEVEL_WARN 4
#define DEBUG_LEVEL_ERROR 5

#define DEBUG_TRACE(...) \
    do { if (DEBUG_LEVEL <= DEBUG_LEVEL_TRACE) DEBUG_LOG(__VA_ARGS__); } while(0)

#define DEBUG_DEBUG(...) \
    do { if (DEBUG_LEVEL <= DEBUG_LEVEL_DEBUG) DEBUG_LOG(__VA_ARGS__); } while(0)

#define DEBUG_INFO(...) \
    do { if (DEBUG_LEVEL <= DEBUG_LEVEL_INFO) DEBUG_LOG(__VA_ARGS__); } while(0)

#define DEBUG_WARN(...) \
    do { if (DEBUG_LEVEL <= DEBUG_LEVEL_WARN) ERROR_LOG(__VA_ARGS__); } while(0)

#define DEBUG_ERROR(...) \
    do { if (DEBUG_LEVEL <= DEBUG_LEVEL_ERROR) ERROR_LOG(__VA_ARGS__); } while(0)

void demonstrate_debug_levels() {
    std::cout << "\n=== Debug Macros with Levels ===\n";

    DEBUG_TRACE("This is a trace message");
    DEBUG_DEBUG("This is a debug message");
    DEBUG_INFO("This is an info message");
    DEBUG_WARN("This is a warning message");
    DEBUG_ERROR("This is an error message");

    // In JS/TS, you'd write:
    // const DEBUG_LEVEL = 2;
    // function debugTrace(...args) {
    //     if (DEBUG_LEVEL <= 1) console.log('[TRACE]', ...args);
    // }
}

// =============================================================================
// 10. MACRO METAPROGRAMMING
// =============================================================================
// Using macros to generate code patterns

#define DECLARE_ENUM(name, ...) \
    enum class name { __VA_ARGS__ }

#define ENUM_TO_STRING(name, value) \
    ([]() -> std::string { \
        switch(value) { \
            __VA_ARGS__ \
            default: return "Unknown"; \
        } \
    }())

// Example usage (simplified)
enum class OrderType { MARKET, LIMIT, STOP };

std::string orderTypeToString(OrderType type) {
    switch(type) {
        case OrderType::MARKET: return "MARKET";
        case OrderType::LIMIT: return "LIMIT";
        case OrderType::STOP: return "STOP";
        default: return "Unknown";
    }
}

void demonstrate_metaprogramming() {
    std::cout << "\n=== Macro Metaprogramming ===\n";

    OrderType type = OrderType::LIMIT;
    std::cout << "Order type: " << orderTypeToString(type) << std::endl;

    // In JS/TS, you'd use:
    // enum OrderType { MARKET, LIMIT, STOP }
    // Or: const OrderType = { MARKET: 'MARKET', LIMIT: 'LIMIT', STOP: 'STOP' };
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "Advanced C++ Macro Examples - JS/TS Developer Edition\n";
    std::cout << "======================================================\n";

    demonstrate_variadic_macros();
    demonstrate_advanced_stringification();
    demonstrate_token_concatenation_patterns();
    demonstrate_conditional_macros();
    demonstrate_loop_macros();
    demonstrate_assertion_macros();
    demonstrate_performance_macros();
    demonstrate_type_safe_patterns();
    demonstrate_debug_levels();
    demonstrate_metaprogramming();

    std::cout << "\n=== Advanced Macro Takeaways for JS/TS Devs ===\n";
    std::cout << "1. Variadic macros = Rest parameters (...args)\n";
    std::cout << "2. Stringification = Template literals with variable names\n";
    std::cout << "3. Token concatenation = Dynamic identifier generation\n";
    std::cout << "4. Conditional macros = Ternary operators or if statements\n";
    std::cout << "5. Loop macros = for loop generators\n";
    std::cout << "6. Assertion macros = Debug assertions with context\n";
    std::cout << "7. Performance macros = Compiler hints (GCC/Clang specific)\n";
    std::cout << "8. Debug levels = Conditional logging based on level\n";
    std::cout << "9. Metaprogramming = Code generation at compile time\n";
    std::cout << "10. Prefer templates/constexpr for type safety when possible\n";

    return 0;
}
