/*
 * =============================================================================
 * Advanced Struct Techniques: Struct Templates
 * Production grade generic programming with constraints and specializations
 * =============================================================================
 */

#include <iostream>
#include <type_traits>
#include <string>
#include <vector>

// Simple constrained template using enable_if

template<typename T,
         typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
struct PodBox {
    T value;
    constexpr PodBox() : value{} {}
    constexpr explicit PodBox(const T& v) : value(v) {}
};

// Specialization example for const char*

template<>
struct PodBox<const char*> {
    const char* value;
    explicit PodBox(const char* v) : value(v) {}
};

// Heterogeneous pair with deduction guides

template<typename A, typename B>
struct Pair {
    A first;
    B second;
};

template<typename A, typename B>
Pair(A, B) -> Pair<A, B>;

// Compile time size info
template<typename T>
struct SizeInfo {
    static constexpr size_t size = sizeof(T);
};

// SFINAE printer

template<typename T>
auto print_if_streamable(const T& v, int) -> decltype(std::cout << v, void()) {
    std::cout << v;
}

template<typename T>
void print_if_streamable(const T&, ...) {
    std::cout << "[not streamable]";
}

// Aggregator with partial specialization for strings

template<typename T>
struct Aggregator {
    T sum{};
    void add(const T& v) { sum += v; }
};

template<>
struct Aggregator<std::string> {
    std::string sum;
    void add(const std::string& v) { if (!sum.empty()) sum += ','; sum += v; }
};

// Demo
void demo_podbox() {
    std::cout << "\n=== STRUCT TEMPLATES: PODBOX ===" << std::endl;
    PodBox<int> a{42};
    PodBox<double> b{3.14};
    PodBox<const char*> c{"hello"};
    std::cout << "int=" << a.value << " double=" << b.value << " cstr=" << c.value << std::endl;
}

void demo_pair() {
    std::cout << "\n=== STRUCT TEMPLATES: PAIR ===" << std::endl;
    Pair p{123, std::string{"abc"}}; // CTAD
    std::cout << "first="; print_if_streamable(p.first, 0);
    std::cout << " second="; print_if_streamable(p.second, 0);
    std::cout << std::endl;
}

void demo_aggregator() {
    std::cout << "\n=== STRUCT TEMPLATES: AGGREGATOR ===" << std::endl;
    Aggregator<int> ai; ai.add(10); ai.add(20);
    Aggregator<std::string> as; as.add("x"); as.add("y");
    std::cout << "sum_int=" << ai.sum << " sum_str=" << as.sum << std::endl;
}

int main() {
    try {
        demo_podbox();
        demo_pair();
        demo_aggregator();
        std::cout << "\n=== STRUCT TEMPLATES COMPLETED SUCCESSFULLY ===" << std::endl;
    } catch (...) {
        std::cerr << "error" << std::endl;
        return 1;
    }
    return 0;
}
