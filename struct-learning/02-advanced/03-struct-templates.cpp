/*
 * =============================================================================
 * Advanced Struct Techniques: Struct Templates - Advanced Generic Programming
 * Production-Grade Template Metaprogramming for Top-Tier Companies
 * =============================================================================
 *
 * This file demonstrates advanced template techniques including:
 * - CRTP (Curiously Recurring Template Pattern)
 * - Variadic templates
 * - Concept-based templates (C++20 style with SFINAE)
 * - Perfect forwarding
 * - Type traits and metaprogramming
 * - Template specialization strategies
 *
 * Author: System Engineering Team
 * Version: 2.0
 * Last Modified: 2024-01-15
 *
 * =============================================================================
 */

#include <iostream>
#include <type_traits>
#include <string>
#include <vector>
#include <utility>
#include <tuple>
#include <array>

// =============================================================================
// CRTP PATTERN (GOOGLE-STYLE)
// =============================================================================

template<typename Derived>
struct Comparable {
    bool operator==(const Derived& other) const {
        return static_cast<const Derived&>(*this).compare(other);
    }
    
    bool operator!=(const Derived& other) const {
        return !(*this == other);
    }
};

struct Point : Comparable<Point> {
    int x, y;
    
    Point(int x, int y) : x(x), y(y) {}
    
    bool compare(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

// =============================================================================
// VARIADIC TEMPLATES (UBER-STYLE)
// =============================================================================

template<typename... Types>
struct Tuple {
    std::tuple<Types...> data;
    
    template<size_t Index>
    auto& get() {
        return std::get<Index>(data);
    }
    
    template<size_t Index>
    const auto& get() const {
        return std::get<Index>(data);
    }
    
    static constexpr size_t size() {
        return sizeof...(Types);
    }
};

// Variadic struct builder
template<typename... Fields>
struct StructBuilder {
    std::tuple<Fields...> fields;
    
    template<size_t Index>
    auto& field() {
        return std::get<Index>(fields);
    }
    
    template<typename T>
    auto& field_by_type() {
        return std::get<T>(fields);
    }
};

// =============================================================================
// CONCEPT-BASED TEMPLATES (BLOOMBERG-STYLE)
// =============================================================================

// SFINAE-based concepts (C++17 style, C++20 concepts would use 'concept' keyword)
template<typename T>
using enable_if_numeric = std::enable_if_t<std::is_arithmetic_v<T>>;

template<typename T, typename = enable_if_numeric<T>>
struct NumericBox {
    T value;
    
    NumericBox(T v) : value(v) {}
    
    template<typename U, typename = enable_if_numeric<U>>
    auto operator+(const NumericBox<U>& other) const {
        return NumericBox<decltype(value + other.value)>(value + other.value);
    }
};

// Type trait for streamable types
template<typename T>
using is_streamable = decltype(std::declval<std::ostream&>() << std::declval<T>());

template<typename T>
auto print_if_streamable(const T& v, int) -> decltype(std::cout << v, void()) {
    std::cout << v;
}

template<typename T>
void print_if_streamable(const T&, ...) {
    std::cout << "[not streamable]";
}

// =============================================================================
// PERFECT FORWARDING (AMAZON-STYLE)
// =============================================================================

template<typename T>
struct ForwardingWrapper {
    T value;
    
    template<typename U>
    ForwardingWrapper(U&& u) : value(std::forward<U>(u)) {}
    
    template<typename U>
    ForwardingWrapper& operator=(U&& u) {
        value = std::forward<U>(u);
        return *this;
    }
};

// Perfect forwarding factory
template<typename T, typename... Args>
T make_forwarded(Args&&... args) {
    return T(std::forward<Args>(args)...);
}

// =============================================================================
// ADVANCED TYPE TRAITS (PAYPAL-STYLE)
// =============================================================================

// Check if type has specific member
template<typename T>
struct has_size_method {
private:
    template<typename U>
    static auto test(int) -> decltype(std::declval<U>().size(), std::true_type{});
    
    template<typename U>
    static std::false_type test(...);
    
public:
    static constexpr bool value = decltype(test<T>(0))::value;
};

template<typename T>
constexpr bool has_size_method_v = has_size_method<T>::value;

// Type erasure with templates
template<typename T>
struct TypeErased {
private:
    struct Concept {
        virtual ~Concept() = default;
        virtual void* get() = 0;
        virtual std::unique_ptr<Concept> clone() const = 0;
    };
    
    template<typename U>
    struct Model : Concept {
        U value;
        
        Model(U v) : value(std::move(v)) {}
        
        void* get() override { return &value; }
        std::unique_ptr<Concept> clone() const override {
            return std::make_unique<Model>(value);
        }
    };
    
    std::unique_ptr<Concept> pimpl_;
    
public:
    template<typename U>
    TypeErased(U&& value) : pimpl_(std::make_unique<Model<std::decay_t<U>>>(std::forward<U>(value))) {}
    
    void* get() { return pimpl_ ? pimpl_->get() : nullptr; }
};

// =============================================================================
// TEMPLATE SPECIALIZATION STRATEGIES (STRIPE-STYLE)
// =============================================================================

// Primary template
template<typename T>
struct Serializer {
    static std::string serialize(const T& value) {
        return "[unknown type]";
    }
};

// Specialization for integers
template<>
struct Serializer<int> {
    static std::string serialize(int value) {
        return std::to_string(value);
    }
};

// Specialization for strings
template<>
struct Serializer<std::string> {
    static std::string serialize(const std::string& value) {
        return "\"" + value + "\"";
    }
};

// Partial specialization for pointers
template<typename T>
struct Serializer<T*> {
    static std::string serialize(T* value) {
        if (value) {
            return Serializer<T>::serialize(*value);
        }
        return "nullptr";
    }
};

// =============================================================================
// METAPROGRAMMING UTILITIES (GOD-MODDED)
// =============================================================================

// Compile-time type list
template<typename... Types>
struct TypeList {};

// Get type at index
template<size_t Index, typename... Types>
struct TypeAt;

template<size_t Index, typename First, typename... Rest>
struct TypeAt<Index, First, Rest...> {
    using type = typename TypeAt<Index - 1, Rest...>::type;
};

template<typename First, typename... Rest>
struct TypeAt<0, First, Rest...> {
    using type = First;
};

// Count types
template<typename... Types>
struct TypeCount {
    static constexpr size_t value = sizeof...(Types);
};

// =============================================================================
// DEMONSTRATION FUNCTIONS
// =============================================================================

void demonstrate_crtp() {
    std::cout << "\n=== CRTP PATTERN ===" << std::endl;
    Point p1(10, 20);
    Point p2(10, 20);
    Point p3(10, 30);
    
    std::cout << "p1 == p2: " << (p1 == p2) << std::endl;
    std::cout << "p1 == p3: " << (p1 == p3) << std::endl;
    std::cout << "p1 != p3: " << (p1 != p3) << std::endl;
}

void demonstrate_variadic_templates() {
    std::cout << "\n=== VARIADIC TEMPLATES ===" << std::endl;
    
    Tuple<int, double, std::string> t;
    t.get<0>() = 42;
    t.get<1>() = 3.14;
    t.get<2>() = "hello";
    
    std::cout << "Tuple size: " << t.size() << std::endl;
    std::cout << "Field 0: " << t.get<0>() << std::endl;
    std::cout << "Field 1: " << t.get<1>() << std::endl;
    std::cout << "Field 2: " << t.get<2>() << std::endl;
    
    StructBuilder<int, std::string, bool> builder;
    builder.field<0>() = 100;
    builder.field<1>() = "world";
    builder.field<2>() = true;
    
    std::cout << "Builder field 0: " << builder.field<0>() << std::endl;
}

void demonstrate_concept_based() {
    std::cout << "\n=== CONCEPT-BASED TEMPLATES ===" << std::endl;
    
    NumericBox<int> nb1(10);
    NumericBox<double> nb2(20.5);
    auto result = nb1 + nb2;
    
    std::cout << "NumericBox result: " << result.value << std::endl;
    
    int streamable = 42;
    struct NonStreamable {};
    NonStreamable ns;
    
    std::cout << "Streamable: ";
    print_if_streamable(streamable, 0);
    std::cout << std::endl;
    
    std::cout << "Non-streamable: ";
    print_if_streamable(ns, 0);
    std::cout << std::endl;
}

void demonstrate_perfect_forwarding() {
    std::cout << "\n=== PERFECT FORWARDING ===" << std::endl;
    
    std::string str = "test";
    ForwardingWrapper<std::string> wrapper1(std::move(str));
    ForwardingWrapper<std::string> wrapper2("literal");
    
    std::cout << "Wrapper1: " << wrapper1.value << std::endl;
    std::cout << "Wrapper2: " << wrapper2.value << std::endl;
    
    auto point = make_forwarded<Point>(5, 10);
    std::cout << "Forwarded Point: (" << point.x << ", " << point.y << ")" << std::endl;
}

void demonstrate_type_traits() {
    std::cout << "\n=== ADVANCED TYPE TRAITS ===" << std::endl;
    
    std::cout << "std::vector has size: " << has_size_method_v<std::vector<int>> << std::endl;
    std::cout << "int has size: " << has_size_method_v<int> << std::endl;
    
    TypeErased<int> erased(42);
    int* ptr = static_cast<int*>(erased.get());
    std::cout << "Type-erased value: " << *ptr << std::endl;
}

void demonstrate_specialization() {
    std::cout << "\n=== TEMPLATE SPECIALIZATION ===" << std::endl;
    
    int i = 42;
    std::string s = "hello";
    int* pi = &i;
    
    std::cout << "Serialize int: " << Serializer<int>::serialize(i) << std::endl;
    std::cout << "Serialize string: " << Serializer<std::string>::serialize(s) << std::endl;
    std::cout << "Serialize pointer: " << Serializer<int*>::serialize(pi) << std::endl;
}

void demonstrate_metaprogramming() {
    std::cout << "\n=== METAPROGRAMMING ===" << std::endl;
    
    using MyTypes = TypeList<int, double, std::string>;
    using FirstType = TypeAt<0, int, double, std::string>::type;
    using SecondType = TypeAt<1, int, double, std::string>::type;
    
    std::cout << "Type count: " << TypeCount<int, double, std::string>::value << std::endl;
    std::cout << "First type size: " << sizeof(FirstType) << std::endl;
    std::cout << "Second type size: " << sizeof(SecondType) << std::endl;
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "=== GOD-MODDED ADVANCED STRUCT TEMPLATES ===" << std::endl;
    std::cout << "Demonstrating production-grade template metaprogramming" << std::endl;
    
    try {
        demonstrate_crtp();
        demonstrate_variadic_templates();
        demonstrate_concept_based();
        demonstrate_perfect_forwarding();
        demonstrate_type_traits();
        demonstrate_specialization();
        demonstrate_metaprogramming();
        
        std::cout << "\n=== STRUCT TEMPLATES COMPLETED SUCCESSFULLY ===" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

// =============================================================================
// COMPILATION NOTES
// =============================================================================
/*
 * Compile with:
 *   g++ -std=c++17 -O2 -Wall -Wextra -o struct_templates 03-struct-templates.cpp
 *   clang++ -std=c++17 -O2 -Wall -Wextra -o struct_templates 03-struct-templates.cpp
 *
 * Advanced template techniques:
 *   - CRTP for compile-time polymorphism
 *   - Variadic templates for flexible structures
 *   - Concept-based templates with SFINAE
 *   - Perfect forwarding
 *   - Advanced type traits
 *   - Template specialization strategies
 *   - Metaprogramming utilities
 */
