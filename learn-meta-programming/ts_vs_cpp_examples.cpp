/**
 * TypeScript vs C++: Side-by-Side Code Examples
 *
 * This file demonstrates equivalent code in TypeScript and C++,
 * showing the differences and similarities between the two languages.
 */

#include <iostream>
#include <vector>
#include <string>
#include <variant>
#include <type_traits>
#include <memory>

// =============================================================================
// 1. GENERICS/TEMPLATES - Basic Example
// =============================================================================

// C++: Template function
template<typename T>
T max_value(const T& a, const T& b) {
    return a > b ? a : b;
}

// TypeScript equivalent:
// function maxValue<T>(a: T, b: T): T {
//     return a > b ? a : b;
// }

void demonstrate_generics() {
    std::cout << "\n=== Generics/Templates ===\n";
    
    int int_max = max_value(10, 20);
    double double_max = max_value(3.14, 2.71);
    
    std::cout << "max_value(10, 20) = " << int_max << std::endl;
    std::cout << "max_value(3.14, 2.71) = " << double_max << std::endl;
    
    // C++ generates: int max_value(int a, int b) { return a > b ? a : b; }
    // C++ generates: double max_value(double a, double b) { return a > b ? a : b; }
    // TypeScript: Same function, type-checked: function maxValue(a, b) { return a > b ? a : b; }
}

// =============================================================================
// 2. VALUE TEMPLATE PARAMETERS (C++ ONLY)
// =============================================================================

// C++: Value template parameters
template<int N>
class FixedArray {
    int data_[N];
public:
    constexpr size_t size() const { return N; }
    int& operator[](size_t i) { return data_[i]; }
};

// TypeScript: NOT POSSIBLE - can't use values as type parameters
// Can only do: class Array<T> { } where T is a type, not a value

void demonstrate_value_parameters() {
    std::cout << "\n=== Value Template Parameters (C++ Only) ===\n";
    
    FixedArray<10> arr;
    std::cout << "FixedArray<10>::size() = " << arr.size() << std::endl;
    
    // Different sizes = different types!
    FixedArray<5> arr2;   // Different type from FixedArray<10>
    FixedArray<20> arr3;  // Another different type
    
    std::cout << "C++ can use VALUES as template parameters!" << std::endl;
    std::cout << "TypeScript can only use TYPES as generic parameters" << std::endl;
}

// =============================================================================
// 3. UNION TYPES
// =============================================================================

// C++: std::variant (C++17)
using StringOrNumber = std::variant<std::string, int>;

void process_variant(const StringOrNumber& value) {
    if (std::holds_alternative<std::string>(value)) {
        auto str = std::get<std::string>(value);
        std::cout << "String: " << str << std::endl;
    } else {
        auto num = std::get<int>(value);
        std::cout << "Number: " << num << std::endl;
    }
}

// TypeScript equivalent:
// type StringOrNumber = string | number;
// function processVariant(value: StringOrNumber): void {
//     if (typeof value === 'string') {
//         console.log("String:", value);
//     } else {
//         console.log("Number:", value);
//     }
// }

void demonstrate_unions() {
    std::cout << "\n=== Union Types ===\n";
    
    process_variant(std::string("Hello"));
    process_variant(42);
    
    std::cout << "C++: Uses std::variant (more verbose)" << std::endl;
    std::cout << "TypeScript: Native union types (more ergonomic)" << std::endl;
}

// =============================================================================
// 4. CONSTEXPR (C++ ONLY - COMPILE-TIME COMPUTATION)
// =============================================================================

// C++: True compile-time computation
constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

constexpr int fact5 = factorial(5);  // Computed at COMPILE-TIME!

// TypeScript equivalent (but runtime):
// const factorial = (n: number): number => 
//     n <= 1 ? 1 : n * factorial(n - 1);
// const fact5 = factorial(5);  // Computed at RUNTIME

void demonstrate_constexpr() {
    std::cout << "\n=== constexpr (C++ Only) ===\n";
    
    std::cout << "factorial(5) = " << fact5 << std::endl;
    std::cout << "This was computed at COMPILE-TIME in C++!" << std::endl;
    std::cout << "TypeScript would compute this at runtime" << std::endl;
}

// =============================================================================
// 5. TEMPLATE SPECIALIZATION (C++ ONLY)
// =============================================================================

// C++: Template specialization
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

// TypeScript equivalent (using conditional types):
// type TypeName<T> = 
//     T extends number ? "number" :
//     T extends string ? "string" :
//     "unknown";

void demonstrate_specialization() {
    std::cout << "\n=== Template Specialization (C++ Only) ===\n";
    
    std::cout << "TypeInfo<int>::name() = " << TypeInfo<int>::name() << std::endl;
    std::cout << "TypeInfo<double>::name() = " << TypeInfo<double>::name() << std::endl;
    std::cout << "C++: Full and partial specialization" << std::endl;
    std::cout << "TypeScript: Use conditional types instead" << std::endl;
}

// =============================================================================
// 6. OPERATOR OVERLOADING (C++ ONLY)
// =============================================================================

// C++: Operator overloading
class Vector {
    int x_, y_;
public:
    Vector(int x, int y) : x_(x), y_(y) {}
    
    Vector operator+(const Vector& other) const {
        return Vector(x_ + other.x_, y_ + other.y_);
    }
    
    friend std::ostream& operator<<(std::ostream& os, const Vector& v) {
        os << "(" << v.x_ << ", " << v.y_ << ")";
        return os;
    }
};

// TypeScript: NO operator overloading
// Must use methods: vector1.add(vector2)

void demonstrate_operator_overloading() {
    std::cout << "\n=== Operator Overloading (C++ Only) ===\n";
    
    Vector a(1, 2);
    Vector b(3, 4);
    Vector c = a + b;  // Custom + operator
    
    std::cout << "a + b = " << c << std::endl;
    std::cout << "C++: Can overload operators" << std::endl;
    std::cout << "TypeScript: Must use methods instead" << std::endl;
}

// =============================================================================
// 7. STRUCTURAL VS NOMINAL TYPING
// =============================================================================

// C++: Nominal typing - must match exactly
class Duck {
public:
    virtual void quack() = 0;
};

class MyDuck : public Duck {
public:
    void quack() override {
        std::cout << "Quack!" << std::endl;
    }
};

void makeQuack(Duck& duck) {
    duck.quack();
}

// TypeScript: Structural typing (duck typing)
// interface Duck {
//     quack(): void;
// }
// function makeQuack(duck: Duck): void {
//     duck.quack();
// }
// // Any object with quack() method works!

void demonstrate_typing() {
    std::cout << "\n=== Structural vs Nominal Typing ===\n";
    
    MyDuck duck;
    makeQuack(duck);
    
    std::cout << "C++: Nominal typing (must inherit)" << std::endl;
    std::cout << "TypeScript: Structural typing (duck typing)" << std::endl;
}

// =============================================================================
// 8. VARIADIC TEMPLATES
// =============================================================================

// C++: Variadic templates with fold expressions (C++17)
template<typename... Args>
auto sum(Args... args) {
    return (args + ...);  // Fold expression
}

template<typename... Args>
void print_all(Args... args) {
    ((std::cout << args << " "), ...);
    std::cout << std::endl;
}

// TypeScript equivalent:
// function sum(...args: number[]): number {
//     return args.reduce((a, b) => a + b, 0);
// }
// function printAll(...args: any[]): void {
//     console.log(...args);
// }

void demonstrate_variadic() {
    std::cout << "\n=== Variadic Templates ===\n";
    
    std::cout << "sum(1, 2, 3, 4, 5) = " << sum(1, 2, 3, 4, 5) << std::endl;
    print_all(1, "Hello", 3.14, "World");
    
    std::cout << "C++: Compile-time expansion, type-safe" << std::endl;
    std::cout << "TypeScript: Runtime array, type-checked" << std::endl;
}

// =============================================================================
// 9. CONCEPTS (C++20) VS GENERIC CONSTRAINTS
// =============================================================================

// C++: Concepts (C++20)
#include <concepts>

template<std::integral T>
T add_integers(T a, T b) {
    return a + b;
}

// TypeScript equivalent:
// function addIntegers<T extends number>(a: T, b: T): T {
//     return (a + b) as T;
// }

void demonstrate_concepts() {
    std::cout << "\n=== Concepts vs Generic Constraints ===\n";
    
    std::cout << "add_integers(5, 10) = " << add_integers(5, 10) << std::endl;
    
    std::cout << "C++: Concepts (C++20) - cleaner than SFINAE" << std::endl;
    std::cout << "TypeScript: extends keyword - more ergonomic" << std::endl;
}

// =============================================================================
// 10. CODE GENERATION COMPARISON
// =============================================================================

// C++: Templates generate different code for each type
template<typename T>
class Container {
    T value_;
public:
    Container(T value) : value_(value) {}
    T get() const { return value_; }
};

// When you use:
// Container<int> c1(42);
// Container<double> c2(3.14);
// C++ generates TWO different classes!

// TypeScript: Generics are erased
// class Container<T> {
//     constructor(private value: T) {}
//     get(): T { return this.value; }
// }
// Runtime: class Container { constructor(value) { this.value = value; } }
// Same class for all types!

void demonstrate_code_generation() {
    std::cout << "\n=== Code Generation ===\n";
    
    Container<int> c1(42);
    Container<double> c2(3.14);
    
    std::cout << "c1.get() = " << c1.get() << std::endl;
    std::cout << "c2.get() = " << c2.get() << std::endl;
    
    std::cout << "C++: Generates DIFFERENT code for each type" << std::endl;
    std::cout << "TypeScript: SAME code, type-checked" << std::endl;
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "TypeScript vs C++: Side-by-Side Examples\n";
    std::cout << "=========================================\n";

    demonstrate_generics();
    demonstrate_value_parameters();
    demonstrate_unions();
    demonstrate_constexpr();
    demonstrate_specialization();
    demonstrate_operator_overloading();
    demonstrate_typing();
    demonstrate_variadic();
    demonstrate_concepts();
    demonstrate_code_generation();

    std::cout << "\n=== Key Differences Summary ===\n";
    std::cout << "1. C++ templates = Code generation (different code per type)\n";
    std::cout << "2. TypeScript generics = Type checking (same code, type-checked)\n";
    std::cout << "3. C++ has value template parameters, TypeScript doesn't\n";
    std::cout << "4. C++ has constexpr (compile-time computation), TypeScript doesn't\n";
    std::cout << "5. C++ has operator overloading, TypeScript doesn't\n";
    std::cout << "6. C++ has template specialization, TypeScript uses conditional types\n";
    std::cout << "7. TypeScript has structural typing, C++ has nominal typing\n";
    std::cout << "8. TypeScript has better reflection, C++ has better metaprogramming\n";
    std::cout << "9. C++: Performance and control\n";
    std::cout << "10. TypeScript: Developer experience and type safety\n";

    return 0;
}
