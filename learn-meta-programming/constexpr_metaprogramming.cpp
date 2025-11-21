/**
 * constexpr Metaprogramming - TypeScript Developer Edition
 *
 * constexpr allows computation at compile-time, enabling powerful metaprogramming
 * without template metaprogramming complexity. Think of it as compile-time functions
 * that can be evaluated during compilation.
 *
 * In TypeScript: const values are computed at runtime (no true constexpr)
 * In C++: constexpr functions are evaluated at compile-time when possible
 *
 * Key concepts:
 * - constexpr functions: Can be evaluated at compile-time
 * - constexpr variables: Computed at compile-time
 * - if constexpr: Compile-time conditionals (C++17)
 * - consteval: Always evaluated at compile-time (C++20)
 */

#include <iostream>
#include <array>
#include <type_traits>

// =============================================================================
// 1. CONSTEXPR FUNCTIONS
// =============================================================================
// In TypeScript: const factorial = (n: number): number => ...

constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

constexpr int power(int base, int exponent) {
    int result = 1;
    for (int i = 0; i < exponent; ++i) {
        result *= base;
    }
    return result;
}

// TypeScript equivalent (runtime):
// const factorial = (n: number): number => 
//     n <= 1 ? 1 : n * factorial(n - 1);
// const result = factorial(5);  // Computed at runtime

void demonstrate_constexpr_functions() {
    std::cout << "\n=== constexpr Functions ===\n";

    // Computed at compile-time
    constexpr int fact5 = factorial(5);
    std::cout << "factorial(5) = " << fact5 << std::endl;

    // Can also be called at runtime
    int runtime_result = factorial(5);
    std::cout << "Runtime factorial(5) = " << runtime_result << std::endl;

    constexpr int pow2_10 = power(2, 10);
    std::cout << "power(2, 10) = " << pow2_10 << std::endl;
}

// =============================================================================
// 2. CONSTEXPR VARIABLES
// =============================================================================
// In TypeScript: const PI = 3.14159; (but computed at runtime)

constexpr double PI = 3.141592653589793;
constexpr int MAX_SIZE = 1024;
constexpr int FIBONACCI_10 = 55;

// Computed at compile-time
constexpr int computed_value = factorial(5) * 2;

// TypeScript equivalent:
// const PI = 3.141592653589793;  // Computed at runtime
// const MAX_SIZE = 1024;
// const computedValue = factorial(5) * 2;  // Runtime computation

void demonstrate_constexpr_variables() {
    std::cout << "\n=== constexpr Variables ===\n";

    std::cout << "PI = " << PI << std::endl;
    std::cout << "MAX_SIZE = " << MAX_SIZE << std::endl;
    std::cout << "computed_value = " << computed_value << std::endl;
}

// =============================================================================
// 3. IF CONSTEXPR (C++17)
// =============================================================================
// Compile-time conditionals
// In TypeScript: Conditional types or type guards

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

// TypeScript equivalent:
// type GetValue<T> = 
//     T extends number ? 42 :
//     T extends string ? "default" :
//     never;
// Or with function overloads

void demonstrate_if_constexpr() {
    std::cout << "\n=== if constexpr ===\n";

    constexpr int int_val = get_value<int>();
    constexpr double double_val = get_value<double>();

    std::cout << "get_value<int>() = " << int_val << std::endl;
    std::cout << "get_value<double>() = " << double_val << std::endl;
}

// =============================================================================
// 4. CONSTEXPR TEMPLATES
// =============================================================================
// Combine constexpr with templates

template<typename T>
constexpr T max_constexpr(const T& a, const T& b) {
    return a > b ? a : b;
}

template<int N>
constexpr int array_size() {
    return N;
}

// TypeScript equivalent:
// function maxConstexpr<T extends number>(a: T, b: T): T {
//     return a > b ? a : b;
// }
// const result = maxConstexpr(10, 20);  // Runtime

void demonstrate_constexpr_templates() {
    std::cout << "\n=== constexpr Templates ===\n";

    constexpr int max_val = max_constexpr(10, 20);
    std::cout << "max_constexpr(10, 20) = " << max_val << std::endl;

    constexpr int size = array_size<5>();
    std::cout << "array_size<5>() = " << size << std::endl;
}

// =============================================================================
// 5. CONSTEXPR ARRAYS AND STRUCTURES
// =============================================================================
// Create compile-time data structures

constexpr std::array<int, 5> compile_time_array = {1, 2, 3, 4, 5};

constexpr int get_array_element(size_t index) {
    return compile_time_array[index];
}

// TypeScript equivalent:
// const compileTimeArray = [1, 2, 3, 4, 5] as const;
// const getArrayElement = (index: number): number => 
//     compileTimeArray[index];  // Runtime

void demonstrate_constexpr_structures() {
    std::cout << "\n=== constexpr Arrays and Structures ===\n";

    constexpr int element = get_array_element(2);
    std::cout << "get_array_element(2) = " << element << std::endl;

    for (size_t i = 0; i < compile_time_array.size(); ++i) {
        std::cout << compile_time_array[i] << " ";
    }
    std::cout << std::endl;
}

// =============================================================================
// 6. CONSTEXPR LOOPS AND ALGORITHMS
// =============================================================================
// Compile-time algorithms

constexpr int sum_array(const int* arr, size_t size) {
    int sum = 0;
    for (size_t i = 0; i < size; ++i) {
        sum += arr[i];
    }
    return sum;
}

constexpr int find_max(const int* arr, size_t size) {
    int max_val = arr[0];
    for (size_t i = 1; i < size; ++i) {
        if (arr[i] > max_val) {
            max_val = arr[i];
        }
    }
    return max_val;
}

// TypeScript equivalent:
// function sumArray(arr: readonly number[]): number {
//     return arr.reduce((a, b) => a + b, 0);  // Runtime
// }

void demonstrate_constexpr_algorithms() {
    std::cout << "\n=== constexpr Algorithms ===\n";

    constexpr int arr[] = {5, 2, 8, 1, 9};
    constexpr int sum = sum_array(arr, 5);
    constexpr int max = find_max(arr, 5);

    std::cout << "sum_array(arr, 5) = " << sum << std::endl;
    std::cout << "find_max(arr, 5) = " << max << std::endl;
}

// =============================================================================
// 7. CONSTEXPR STRING OPERATIONS
// =============================================================================
// Compile-time string manipulation (C++20)

constexpr size_t string_length(const char* str) {
    size_t len = 0;
    while (str[len] != '\0') {
        ++len;
    }
    return len;
}

constexpr bool strings_equal(const char* a, const char* b) {
    size_t i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) return false;
        ++i;
    }
    return a[i] == b[i];
}

// TypeScript equivalent:
// const stringLength = (str: string): number => str.length;  // Runtime
// const stringsEqual = (a: string, b: string): boolean => a === b;  // Runtime

void demonstrate_constexpr_strings() {
    std::cout << "\n=== constexpr String Operations ===\n";

    constexpr size_t len = string_length("Hello");
    std::cout << "string_length(\"Hello\") = " << len << std::endl;

    constexpr bool equal = strings_equal("Hello", "Hello");
    std::cout << "strings_equal(\"Hello\", \"Hello\") = " << equal << std::endl;
}

// =============================================================================
// 8. CONSTEVAL (C++20)
// =============================================================================
// Functions that MUST be evaluated at compile-time

consteval int must_be_compile_time(int n) {
    return n * 2;
}

// TypeScript equivalent: Not available
// TypeScript doesn't have compile-time function evaluation

void demonstrate_consteval() {
    std::cout << "\n=== consteval (C++20) ===\n";

    constexpr int result = must_be_compile_time(21);
    std::cout << "must_be_compile_time(21) = " << result << std::endl;

    // This would fail:
    // int runtime_result = must_be_compile_time(21);  // Error!
}

// =============================================================================
// 9. CONSTEXPR WITH TYPE TRAITS
// =============================================================================
// Combine constexpr with type traits

template<typename T>
constexpr bool is_numeric() {
    return std::is_arithmetic_v<T>;
}

template<typename T>
constexpr size_t type_size() {
    return sizeof(T);
}

// TypeScript equivalent:
// type IsNumeric<T> = T extends number ? true : false;
// type TypeSize<T> = T extends number ? 8 : never;  // Limited

void demonstrate_constexpr_type_traits() {
    std::cout << "\n=== constexpr with Type Traits ===\n";

    constexpr bool is_int_numeric = is_numeric<int>();
    constexpr bool is_string_numeric = is_numeric<std::string>();

    std::cout << "is_numeric<int>() = " << is_int_numeric << std::endl;
    std::cout << "is_numeric<std::string>() = " << is_string_numeric << std::endl;

    constexpr size_t int_size = type_size<int>();
    std::cout << "type_size<int>() = " << int_size << std::endl;
}

// =============================================================================
// 10. COMPILE-TIME VALIDATION
// =============================================================================
// Use constexpr for compile-time validation

template<int N>
constexpr void validate_positive() {
    static_assert(N > 0, "N must be positive");
}

template<typename T, size_t N>
constexpr void validate_array_size() {
    static_assert(N > 0, "Array size must be positive");
    static_assert(N <= 1000, "Array size too large");
}

// TypeScript equivalent:
// type ValidatePositive<N extends number> = 
//     N extends number ? (N extends 0 ? never : N) : never;

void demonstrate_compile_time_validation() {
    std::cout << "\n=== Compile-Time Validation ===\n";

    validate_positive<5>();  // OK
    // validate_positive<-1>();  // Compile error!

    validate_array_size<int, 10>();  // OK
    // validate_array_size<int, 2000>();  // Compile error!

    std::cout << "Compile-time validation works!" << std::endl;
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "constexpr Metaprogramming - TypeScript Developer Edition\n";
    std::cout << "========================================================\n";

    demonstrate_constexpr_functions();
    demonstrate_constexpr_variables();
    demonstrate_if_constexpr();
    demonstrate_constexpr_templates();
    demonstrate_constexpr_structures();
    demonstrate_constexpr_algorithms();
    demonstrate_constexpr_strings();
    demonstrate_consteval();
    demonstrate_constexpr_type_traits();
    demonstrate_compile_time_validation();

    std::cout << "\n=== Key Takeaways for TypeScript Developers ===\n";
    std::cout << "1. constexpr = Compile-time computation (TypeScript doesn't have this)\n";
    std::cout << "2. constexpr functions = Can be evaluated at compile-time\n";
    std::cout << "3. constexpr variables = Computed at compile-time\n";
    std::cout << "4. if constexpr = Compile-time conditionals (like conditional types)\n";
    std::cout << "5. consteval = Must be compile-time (C++20, no TS equivalent)\n";
    std::cout << "6. Zero runtime overhead for constexpr computations\n";
    std::cout << "7. TypeScript const is runtime (not compile-time)\n";
    std::cout << "8. C++ constexpr enables powerful compile-time programming\n";
    std::cout << "9. Can validate and compute at compile-time\n";
    std::cout << "10. Essential for zero-overhead abstractions\n";

    return 0;
}
