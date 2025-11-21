/**
 * C++20 Concepts - TypeScript Developer Edition
 *
 * Concepts provide a way to specify constraints on template parameters,
 * making templates more readable and providing better error messages.
 * Think of them as TypeScript's generic constraints, but more powerful.
 *
 * In TypeScript: function add<T extends number>(a: T, b: T): T
 * In C++: template<std::integral T> T add(T a, T b)
 *
 * Key benefits:
 * - Better error messages
 * - More readable code
 * - Easier to understand requirements
 * - Compile-time type checking
 */

#include <iostream>
#include <concepts>
#include <type_traits>
#include <vector>
#include <string>

// =============================================================================
// 1. STANDARD CONCEPTS
// =============================================================================
// C++20 provides standard concepts

template<std::integral T>
T add_integers(T a, T b) {
    return a + b;
}

template<std::floating_point T>
T add_floats(T a, T b) {
    return a + b;
}

template<std::totally_ordered T>
T max_value(const T& a, const T& b) {
    return a > b ? a : b;
}

// TypeScript equivalent:
// function addIntegers<T extends number>(a: T, b: T): T {
//     return (a + b) as T;
// }
// function maxValue<T extends number | string>(a: T, b: T): T {
//     return a > b ? a : b;
// }

void demonstrate_standard_concepts() {
    std::cout << "\n=== Standard Concepts ===\n";

    std::cout << "add_integers(5, 10) = " << add_integers(5, 10) << std::endl;
    std::cout << "add_floats(3.14, 2.71) = " << add_floats(3.14, 2.71) << std::endl;
    std::cout << "max_value(10, 20) = " << max_value(10, 20) << std::endl;
    std::cout << "max_value(\"apple\", \"banana\") = " 
              << max_value(std::string("apple"), std::string("banana")) << std::endl;
}

// =============================================================================
// 2. CUSTOM CONCEPTS
// =============================================================================
// Define your own concepts
// In TypeScript: interface constraints

template<typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::convertible_to<T>;
};

template<typename T>
concept Subtractable = requires(T a, T b) {
    { a - b } -> std::convertible_to<T>;
};

template<typename T>
concept Arithmetic = Addable<T> && Subtractable<T>;

// TypeScript equivalent:
// interface Addable {
//     add(other: this): this;
// }
// function add<T extends Addable>(a: T, b: T): T {
//     return a.add(b);
// }

template<Addable T>
T add(T a, T b) {
    return a + b;
}

void demonstrate_custom_concepts() {
    std::cout << "\n=== Custom Concepts ===\n";

    std::cout << "add(5, 10) = " << add(5, 10) << std::endl;
    std::cout << "add(3.14, 2.71) = " << add(3.14, 2.71) << std::endl;
}

// =============================================================================
// 3. REQUIRES CLAUSES
// =============================================================================
// More complex requirements

template<typename T>
concept HasSize = requires(T t) {
    { t.size() } -> std::convertible_to<size_t>;
};

template<typename T>
concept Container = HasSize<T> && requires(T t) {
    typename T::value_type;
    t.begin();
    t.end();
};

// TypeScript equivalent:
// interface HasSize {
//     size(): number;
// }
// interface Container<T> extends HasSize {
//     valueType: T;
//     begin(): Iterator<T>;
//     end(): Iterator<T>;
// }

template<Container T>
void print_container(const T& container) {
    std::cout << "Container size: " << container.size() << std::endl;
}

void demonstrate_requires_clauses() {
    std::cout << "\n=== Requires Clauses ===\n";

    std::vector<int> vec = {1, 2, 3, 4, 5};
    print_container(vec);
}

// =============================================================================
// 4. CONCEPT COMBINATIONS
// =============================================================================
// Combine multiple concepts

template<typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template<typename T>
concept Comparable = std::totally_ordered<T>;

template<typename T>
concept NumericComparable = Numeric<T> && Comparable<T>;

// TypeScript equivalent:
// type Numeric = number | bigint;
// type NumericComparable = Numeric & { compare(other: this): number };

template<NumericComparable T>
T clamp(T value, T min, T max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

void demonstrate_concept_combinations() {
    std::cout << "\n=== Concept Combinations ===\n";

    std::cout << "clamp(15, 10, 20) = " << clamp(15, 10, 20) << std::endl;
    std::cout << "clamp(5, 10, 20) = " << clamp(5, 10, 20) << std::endl;
    std::cout << "clamp(25, 10, 20) = " << clamp(25, 10, 20) << std::endl;
}

// =============================================================================
// 5. CONCEPTS WITH TYPE TRAITS
// =============================================================================
// Use type traits in concepts

template<typename T>
concept Pointer = std::is_pointer_v<T>;

template<typename T>
concept NotPointer = !std::is_pointer_v<T>;

template<NotPointer T>
void process_value(T value) {
    std::cout << "Processing non-pointer value" << std::endl;
}

// TypeScript equivalent:
// type NotPointer<T> = T extends object ? (T extends any[] ? true : false) : true;

void demonstrate_concepts_with_traits() {
    std::cout << "\n=== Concepts with Type Traits ===\n";

    process_value(42);
    process_value(3.14);
    // process_value(static_cast<int*>(nullptr));  // Would fail
}

// =============================================================================
// 6. CONCEPTS FOR ITERATORS
// =============================================================================
// Concepts for iterator requirements

template<typename It>
concept InputIterator = requires(It it) {
    *it;
    ++it;
    it++;
};

template<InputIterator It>
void iterate(It begin, It end) {
    for (auto it = begin; it != end; ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

// TypeScript equivalent:
// interface InputIterator<T> {
//     value(): T;
//     next(): this;
// }

void demonstrate_iterator_concepts() {
    std::cout << "\n=== Iterator Concepts ===\n";

    std::vector<int> vec = {1, 2, 3, 4, 5};
    iterate(vec.begin(), vec.end());
}

// =============================================================================
// 7. CONCEPTS VS SFINAE
// =============================================================================
// Concepts are cleaner than SFINAE

// Old way (SFINAE)
template<typename T>
typename std::enable_if_t<std::is_integral_v<T>, T>
old_add(T a, T b) {
    return a + b;
}

// New way (Concepts)
template<std::integral T>
T new_add(T a, T b) {
    return a + b;
}

// TypeScript equivalent:
// function add<T extends number>(a: T, b: T): T {
//     return (a + b) as T;
// }

void demonstrate_concepts_vs_sfinae() {
    std::cout << "\n=== Concepts vs SFINAE ===\n";

    std::cout << "old_add(5, 10) = " << old_add(5, 10) << std::endl;
    std::cout << "new_add(5, 10) = " << new_add(5, 10) << std::endl;
    std::cout << "Concepts are much cleaner!" << std::endl;
}

// =============================================================================
// 8. CONCEPTS WITH AUTO
// =============================================================================
// Use concepts with auto

template<std::integral T>
auto process_integral(T value) {
    return value * 2;
}

// TypeScript equivalent:
// function processIntegral<T extends number>(value: T): T {
//     return (value * 2) as T;
// }

void demonstrate_concepts_with_auto() {
    std::cout << "\n=== Concepts with auto ===\n";

    auto result = process_integral(21);
    std::cout << "process_integral(21) = " << result << std::endl;
}

// =============================================================================
// 9. NESTED REQUIREMENTS
// =============================================================================
// Complex concept requirements

template<typename T>
concept ComplexType = requires(T t) {
    requires std::is_class_v<T>;
    requires sizeof(T) > 4;
    { t.size() } -> std::convertible_to<size_t>;
};

// TypeScript equivalent:
// type ComplexType<T> = 
//     T extends object 
//         ? T extends { size(): number }
//             ? T
//             : never
//         : never;

void demonstrate_nested_requirements() {
    std::cout << "\n=== Nested Requirements ===\n";

    std::vector<int> vec;
    // ComplexType concept would work here if we implement it
    std::cout << "Nested requirements enable complex constraints" << std::endl;
}

// =============================================================================
// 10. CONCEPTS FOR BLOOMBERG-STYLE CODE
// =============================================================================
// Real-world concept usage

template<typename T>
concept BloombergType = requires {
    typename T::BloombergTag;
};

template<typename T>
concept Serializable = requires(T t) {
    { t.serialize() } -> std::convertible_to<std::string>;
    { T::deserialize(std::string{}) } -> std::same_as<T>;
};

// TypeScript equivalent:
// interface BloombergType {
//     readonly BloombergTag: unique symbol;
// }
// interface Serializable {
//     serialize(): string;
//     deserialize(data: string): this;
// }

void demonstrate_bloomberg_concepts() {
    std::cout << "\n=== Bloomberg-Style Concepts ===\n";

    std::cout << "Concepts enable clear API contracts" << std::endl;
    std::cout << "Better than SFINAE for Bloomberg code" << std::endl;
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "C++20 Concepts - TypeScript Developer Edition\n";
    std::cout << "==============================================\n";

    demonstrate_standard_concepts();
    demonstrate_custom_concepts();
    demonstrate_requires_clauses();
    demonstrate_concept_combinations();
    demonstrate_concepts_with_traits();
    demonstrate_iterator_concepts();
    demonstrate_concepts_vs_sfinae();
    demonstrate_concepts_with_auto();
    demonstrate_nested_requirements();
    demonstrate_bloomberg_concepts();

    std::cout << "\n=== Key Takeaways for TypeScript Developers ===\n";
    std::cout << "1. Concepts = Generic constraints in TypeScript\n";
    std::cout << "2. template<std::integral T> = <T extends number>\n";
    std::cout << "3. Custom concepts = Interface constraints\n";
    std::cout << "4. Requires clauses = Property/method requirements\n";
    std::cout << "5. Concept combinations = Intersection types\n";
    std::cout << "6. Much cleaner than SFINAE\n";
    std::cout << "7. Better error messages\n";
    std::cout << "8. More readable code\n";
    std::cout << "9. Compile-time type checking\n";
    std::cout << "10. Essential for modern C++ (C++20+)\n";

    return 0;
}
