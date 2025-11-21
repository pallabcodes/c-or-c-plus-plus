/**
 * Type Traits and SFINAE - TypeScript Developer Edition
 *
 * Type traits allow you to inspect and manipulate types at compile time.
 * SFINAE (Substitution Failure Is Not An Error) enables conditional compilation
 * based on type properties.
 *
 * In TypeScript: Conditional types, mapped types, type guards
 * In C++: Type traits, std::enable_if, SFINAE
 *
 * Key concepts:
 * - Type traits: Query type properties (is_integral, is_pointer, etc.)
 * - Type transformations: Create new types (remove_const, add_pointer, etc.)
 * - SFINAE: Enable/disable function overloads based on type properties
 */

#include <iostream>
#include <type_traits>
#include <string>
#include <vector>

// =============================================================================
// 1. STANDARD TYPE TRAITS
// =============================================================================
// In TypeScript: type IsNumber<T> = T extends number ? true : false;

void demonstrate_standard_type_traits() {
    std::cout << "\n=== Standard Type Traits ===\n";

    // Type queries
    std::cout << "std::is_integral_v<int> = " << std::is_integral_v<int> << std::endl;
    std::cout << "std::is_integral_v<double> = " << std::is_integral_v<double> << std::endl;
    std::cout << "std::is_floating_point_v<double> = " << std::is_floating_point_v<double> << std::endl;
    std::cout << "std::is_pointer_v<int*> = " << std::is_pointer_v<int*> << std::endl;
    std::cout << "std::is_same_v<int, int> = " << std::is_same_v<int, int> << std::endl;
    std::cout << "std::is_same_v<int, double> = " << std::is_same_v<int, double> << std::endl;

    // TypeScript equivalent:
    // type IsNumber<T> = T extends number ? true : false;
    // type IsSame<T, U> = T extends U ? (U extends T ? true : false) : false;
    // const isNumber: IsNumber<number> = true;
    // const isSame: IsSame<number, number> = true;
}

// =============================================================================
// 2. TYPE TRANSFORMATIONS
// =============================================================================
// In TypeScript: Mapped types, conditional types

void demonstrate_type_transformations() {
    std::cout << "\n=== Type Transformations ===\n";

    // Remove const
    using NonConstInt = std::remove_const_t<const int>;
    std::cout << "std::remove_const_t<const int> is int: " 
              << std::is_same_v<NonConstInt, int> << std::endl;

    // Add pointer
    using IntPointer = std::add_pointer_t<int>;
    std::cout << "std::add_pointer_t<int> is int*: " 
              << std::is_same_v<IntPointer, int*> << std::endl;

    // Remove reference
    using NonRefInt = std::remove_reference_t<int&>;
    std::cout << "std::remove_reference_t<int&> is int: " 
              << std::is_same_v<NonRefInt, int> << std::endl;

    // Decay (array to pointer, function to function pointer)
    using DecayedArray = std::decay_t<int[]>;
    std::cout << "std::decay_t<int[]> is int*: " 
              << std::is_same_v<DecayedArray, int*> << std::endl;

    // TypeScript equivalent:
    // type RemoveReadonly<T> = {
    //     -readonly [P in keyof T]: T[P];
    // };
    // type AddReadonly<T> = {
    //     readonly [P in keyof T]: T[P];
    // };
}

// =============================================================================
// 3. CUSTOM TYPE TRAITS
// =============================================================================
// Create your own type traits
// In TypeScript: Custom conditional types

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

// TypeScript equivalent:
// type IsPointer<T> = T extends any[] ? false : T extends object ? true : false;
// Or more accurate:
// type IsPointer<T> = T extends infer U | null | undefined 
//     ? U extends object ? true : false 
//     : false;

void demonstrate_custom_type_traits() {
    std::cout << "\n=== Custom Type Traits ===\n";

    std::cout << "is_pointer_v<int> = " << is_pointer_v<int> << std::endl;
    std::cout << "is_pointer_v<int*> = " << is_pointer_v<int*> << std::endl;
    std::cout << "is_pointer_v<double*> = " << is_pointer_v<double*> << std::endl;
}

// =============================================================================
// 4. SFINAE BASICS
// =============================================================================
// Substitution Failure Is Not An Error
// In TypeScript: Function overloads or conditional types

// Enable function only for integral types
template<typename T>
typename std::enable_if_t<std::is_integral_v<T>, T>
add_one(T value) {
    return value + 1;
}

// Enable function only for floating point types
template<typename T>
typename std::enable_if_t<std::is_floating_point_v<T>, T>
add_one(T value) {
    return value + 1.0;
}

// TypeScript equivalent:
// function addOne(value: number): number;
// function addOne(value: number): number {
//     return value + 1;
// }
// Or with overloads:
// function addOne<T extends number>(value: T): T {
//     return (value + 1) as T;
// }

void demonstrate_sfinae_basics() {
    std::cout << "\n=== SFINAE Basics ===\n";

    int int_result = add_one(5);
    std::cout << "add_one(5) = " << int_result << std::endl;

    double double_result = add_one(3.14);
    std::cout << "add_one(3.14) = " << double_result << std::endl;

    // This would fail to compile:
    // add_one(std::string("hello"));  // No matching function
}

// =============================================================================
// 5. EXPRESSION SFINAE
// =============================================================================
// SFINAE based on expressions
// In TypeScript: Type guards, conditional types with extends

// Check if type has size() method
template<typename T>
auto get_size(const T& container) -> decltype(container.size()) {
    return container.size();
}

// Fallback for arrays
template<typename T, size_t N>
size_t get_size(const T (&array)[N]) {
    return N;
}

// TypeScript equivalent:
// function getSize<T extends { length: number }>(container: T): number {
//     return container.length;
// }
// function getSize<T extends any[]>(array: T): number {
//     return array.length;
// }

void demonstrate_expression_sfinae() {
    std::cout << "\n=== Expression SFINAE ===\n";

    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::cout << "get_size(vec) = " << get_size(vec) << std::endl;

    int arr[] = {1, 2, 3, 4, 5};
    std::cout << "get_size(arr) = " << get_size(arr) << std::endl;
}

// =============================================================================
// 6. SFINAE WITH VOID_T (C++17)
// =============================================================================
// Modern way to check for member existence

template<typename...>
using void_t = void;

// Check if type has begin() and end() methods
template<typename T, typename = void>
struct is_iterable : std::false_type {};

template<typename T>
struct is_iterable<T, void_t<
    decltype(std::declval<T>().begin()),
    decltype(std::declval<T>().end())
>> : std::true_type {};

template<typename T>
constexpr bool is_iterable_v = is_iterable<T>::value;

// TypeScript equivalent:
// type HasLength<T> = T extends { length: number } ? true : false;
// type IsIterable<T> = T extends { [Symbol.iterator](): any } ? true : false;

void demonstrate_void_t() {
    std::cout << "\n=== SFINAE with void_t ===\n";

    std::cout << "is_iterable_v<std::vector<int>> = " 
              << is_iterable_v<std::vector<int>> << std::endl;
    std::cout << "is_iterable_v<int> = " 
              << is_iterable_v<int> << std::endl;
    std::cout << "is_iterable_v<std::string> = " 
              << is_iterable_v<std::string> << std::endl;
}

// =============================================================================
// 7. CONDITIONAL FUNCTION OVERLOADS
// =============================================================================
// Enable different implementations based on type properties

// For arithmetic types
template<typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>
square(T value) {
    return value * value;
}

// For non-arithmetic types, provide a different implementation
template<typename T>
typename std::enable_if_t<!std::is_arithmetic_v<T>, void>
square(const T& value) {
    std::cout << "Cannot square non-arithmetic type" << std::endl;
}

// TypeScript equivalent:
// function square<T extends number>(value: T): T {
//     return (value * value) as T;
// }
// function square(value: any): void {
//     console.log("Cannot square non-numeric type");
// }

void demonstrate_conditional_overloads() {
    std::cout << "\n=== Conditional Function Overloads ===\n";

    int int_result = square(5);
    std::cout << "square(5) = " << int_result << std::endl;

    double double_result = square(3.14);
    std::cout << "square(3.14) = " << double_result << std::endl;

    square(std::string("hello"));  // Calls the non-arithmetic version
}

// =============================================================================
// 8. TYPE TRAIT HELPERS
// =============================================================================
// Convenience functions using type traits

template<typename T>
constexpr bool is_numeric_v = std::is_arithmetic_v<T>;

template<typename T>
constexpr bool is_container_v = is_iterable_v<T> && !std::is_same_v<T, std::string>;

// TypeScript equivalent:
// type IsNumeric<T> = T extends number ? true : false;
// type IsContainer<T> = T extends any[] ? true : false;

void demonstrate_type_trait_helpers() {
    std::cout << "\n=== Type Trait Helpers ===\n";

    std::cout << "is_numeric_v<int> = " << is_numeric_v<int> << std::endl;
    std::cout << "is_numeric_v<std::string> = " << is_numeric_v<std::string> << std::endl;
    std::cout << "is_container_v<std::vector<int>> = " 
              << is_container_v<std::vector<int>> << std::endl;
    std::cout << "is_container_v<std::string> = " 
              << is_container_v<std::string> << std::endl;
}

// =============================================================================
// 9. COMPILE-TIME TYPE CHECKING
// =============================================================================
// Use static_assert with type traits

template<typename T>
void process_numeric(T value) {
    static_assert(std::is_arithmetic_v<T>, 
                  "T must be an arithmetic type");
    // Process the value
    std::cout << "Processing numeric value: " << value << std::endl;
}

// TypeScript equivalent:
// function processNumeric<T extends number>(value: T): void {
//     // TypeScript compiler enforces T extends number
//     console.log("Processing numeric value:", value);
// }

void demonstrate_compile_time_checking() {
    std::cout << "\n=== Compile-Time Type Checking ===\n";

    process_numeric(42);      // OK
    process_numeric(3.14);   // OK
    // process_numeric(std::string("hello"));  // Compile error!

    std::cout << "Type checking works at compile time!" << std::endl;
}

// =============================================================================
// 10. ADVANCED TYPE MANIPULATION
// =============================================================================
// Complex type transformations

// Remove all qualifiers (const, volatile, reference)
template<typename T>
using remove_all_qualifiers_t = std::remove_cvref_t<T>;

// Add const and reference
template<typename T>
using add_const_ref_t = const T&;

// TypeScript equivalent:
// type RemoveAllQualifiers<T> = {
//     -readonly [P in keyof T]: T[P];
// };
// type AddConstRef<T> = Readonly<T> & { readonly [P in keyof T]: T[P] };

void demonstrate_advanced_manipulation() {
    std::cout << "\n=== Advanced Type Manipulation ===\n";

    using CleanInt = remove_all_qualifiers_t<const volatile int&>;
    std::cout << "remove_all_qualifiers_t<const volatile int&> is int: " 
              << std::is_same_v<CleanInt, int> << std::endl;

    using ConstRefInt = add_const_ref_t<int>;
    std::cout << "add_const_ref_t<int> is const int&: " 
              << std::is_same_v<ConstRefInt, const int&> << std::endl;
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "Type Traits and SFINAE - TypeScript Developer Edition\n";
    std::cout << "=====================================================\n";

    demonstrate_standard_type_traits();
    demonstrate_type_transformations();
    demonstrate_custom_type_traits();
    demonstrate_sfinae_basics();
    demonstrate_expression_sfinae();
    demonstrate_void_t();
    demonstrate_conditional_overloads();
    demonstrate_type_trait_helpers();
    demonstrate_compile_time_checking();
    demonstrate_advanced_manipulation();

    std::cout << "\n=== Key Takeaways for TypeScript Developers ===\n";
    std::cout << "1. Type traits = Conditional types in TypeScript\n";
    std::cout << "2. std::is_integral_v<T> = T extends number in TypeScript\n";
    std::cout << "3. std::enable_if_t = Function overloads in TypeScript\n";
    std::cout << "4. SFINAE = Type guards and conditional types\n";
    std::cout << "5. Expression SFINAE = Type guards with property checks\n";
    std::cout << "6. void_t = Checking for member existence\n";
    std::cout << "7. static_assert = Compile-time type checking\n";
    std::cout << "8. Type transformations = Mapped types in TypeScript\n";
    std::cout << "9. C++ type traits are more powerful (can check expressions)\n";
    std::cout << "10. TypeScript type system is more ergonomic but less powerful\n";

    return 0;
}
