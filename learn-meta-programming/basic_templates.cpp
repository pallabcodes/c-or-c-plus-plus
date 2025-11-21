/**
 * Basic Template Metaprogramming - TypeScript Developer Edition
 *
 * Templates are C++'s way of writing generic code that works with multiple types.
 * Think of them as C++'s equivalent to TypeScript generics, but more powerful
 * because they operate at compile-time and can generate different code for each type.
 *
 * In TypeScript: function identity<T>(x: T): T { return x; }
 * In C++: template<typename T> T identity(T x) { return x; }
 *
 * Key differences:
 * - C++ templates: Compile-time code generation, zero runtime overhead
 * - TypeScript generics: Type checking only, erased at runtime
 * - C++ templates: Can use values as parameters (template<int N>)
 * - TypeScript: Only type parameters (no value parameters)
 */

#include <iostream>
#include <string>
#include <vector>
#include <array>

// =============================================================================
// 1. FUNCTION TEMPLATES
// =============================================================================
// In TypeScript: function max<T>(a: T, b: T): T { return a > b ? a : b; }

template<typename T>
T max(const T& a, const T& b) {
    return a > b ? a : b;
}

// TypeScript equivalent:
// function max<T extends number | string>(a: T, b: T): T {
//     return a > b ? a : b;
// }

void demonstrate_function_templates() {
    std::cout << "\n=== Function Templates ===\n";

    // Works with int
    int int_max = max(10, 20);
    std::cout << "max(10, 20) = " << int_max << std::endl;

    // Works with double
    double double_max = max(3.14, 2.71);
    std::cout << "max(3.14, 2.71) = " << double_max << std::endl;

    // Works with string
    std::string string_max = max(std::string("apple"), std::string("banana"));
    std::cout << "max(\"apple\", \"banana\") = " << string_max << std::endl;

    // In TypeScript, you'd write:
    // const intMax = max(10, 20);
    // const doubleMax = max(3.14, 2.71);
    // const stringMax = max("apple", "banana");
}

// =============================================================================
// 2. CLASS TEMPLATES
// =============================================================================
// In TypeScript: class Vector<T> { private data: T[]; }

template<typename T>
class Vector {
private:
    T* data_;
    size_t size_;
    size_t capacity_;

public:
    explicit Vector(size_t size = 0) 
        : size_(size), capacity_(size > 0 ? size : 1) {
        data_ = new T[capacity_];
    }

    ~Vector() {
        delete[] data_;
    }

    void push_back(const T& value) {
        if (size_ >= capacity_) {
            capacity_ *= 2;
            T* new_data = new T[capacity_];
            for (size_t i = 0; i < size_; ++i) {
                new_data[i] = data_[i];
            }
            delete[] data_;
            data_ = new_data;
        }
        data_[size_++] = value;
    }

    T& operator[](size_t index) {
        return data_[index];
    }

    const T& operator[](size_t index) const {
        return data_[index];
    }

    size_t size() const { return size_; }
};

// TypeScript equivalent:
// class Vector<T> {
//     private data: T[] = [];
//     
//     pushBack(value: T): void {
//         this.data.push(value);
//     }
//     
//     get(index: number): T {
//         return this.data[index];
//     }
//     
//     get size(): number {
//         return this.data.length;
//     }
// }

void demonstrate_class_templates() {
    std::cout << "\n=== Class Templates ===\n";

    Vector<int> int_vector;
    int_vector.push_back(1);
    int_vector.push_back(2);
    int_vector.push_back(3);
    std::cout << "int_vector[0] = " << int_vector[0] << std::endl;
    std::cout << "int_vector.size() = " << int_vector.size() << std::endl;

    Vector<std::string> string_vector;
    string_vector.push_back("Hello");
    string_vector.push_back("World");
    std::cout << "string_vector[0] = " << string_vector[0] << std::endl;

    // In TypeScript, you'd write:
    // const intVector = new Vector<number>();
    // intVector.pushBack(1);
    // intVector.pushBack(2);
}

// =============================================================================
// 3. TEMPLATE PARAMETERS (VALUE PARAMETERS)
// =============================================================================
// C++ allows value parameters - TypeScript doesn't have this!

template<typename T, size_t N>
class Array {
private:
    T data_[N];

public:
    T& operator[](size_t index) {
        return data_[index];
    }

    const T& operator[](size_t index) const {
        return data_[index];
    }

    constexpr size_t size() const { return N; }
};

// TypeScript equivalent (using branded types):
// type Array<T, N extends number> = {
//     readonly length: N;
//     [index: number]: T;
// }
// But TypeScript can't enforce the size at compile time like C++

void demonstrate_value_parameters() {
    std::cout << "\n=== Value Template Parameters ===\n";

    Array<int, 5> int_array;
    int_array[0] = 10;
    int_array[1] = 20;
    std::cout << "int_array.size() = " << int_array.size() << std::endl;

    Array<double, 10> double_array;
    double_array[0] = 3.14;
    std::cout << "double_array.size() = " << double_array.size() << std::endl;

    // Different sizes are different types!
    // Array<int, 5> and Array<int, 10> are completely different types

    // In TypeScript, you'd use:
    // const intArray: Array<number, 5> = [10, 20, 0, 0, 0];
    // But the size isn't enforced at compile time
}

// =============================================================================
// 4. MULTIPLE TEMPLATE PARAMETERS
// =============================================================================
// In TypeScript: function pair<A, B>(a: A, b: B): [A, B] { return [a, b]; }

template<typename First, typename Second>
class Pair {
private:
    First first_;
    Second second_;

public:
    Pair(const First& first, const Second& second)
        : first_(first), second_(second) {}

    First& first() { return first_; }
    const First& first() const { return first_; }

    Second& second() { return second_; }
    const Second& second() const { return second_; }
};

// TypeScript equivalent:
// type Pair<A, B> = [A, B];
// Or:
// class Pair<A, B> {
//     constructor(
//         public first: A,
//         public second: B
//     ) {}
// }

void demonstrate_multiple_parameters() {
    std::cout << "\n=== Multiple Template Parameters ===\n";

    Pair<int, std::string> pair(42, "Hello");
    std::cout << "pair.first() = " << pair.first() << std::endl;
    std::cout << "pair.second() = " << pair.second() << std::endl;

    Pair<std::string, double> pair2("Price", 99.99);
    std::cout << "pair2.first() = " << pair2.first() << std::endl;
    std::cout << "pair2.second() = " << pair2.second() << std::endl;

    // In TypeScript, you'd write:
    // const pair: Pair<number, string> = [42, "Hello"];
    // Or: const pair = new Pair(42, "Hello");
}

// =============================================================================
// 5. TEMPLATE TYPE DEDUCTION (C++17)
// =============================================================================
// C++ can deduce template parameters automatically

template<typename T>
void print_type() {
    std::cout << "Type: " << typeid(T).name() << std::endl;
}

// Class template argument deduction (CTAD) - C++17
template<typename T>
class Container {
    T value_;
public:
    Container(T value) : value_(value) {}
    T get() const { return value_; }
};

// TypeScript equivalent:
// function printType<T>(): void {
//     console.log("Type:", typeof {} as T);
// }
// TypeScript always deduces types automatically

void demonstrate_type_deduction() {
    std::cout << "\n=== Template Type Deduction ===\n";

    // Function template - type deduced automatically
    auto max_val = max(10, 20);  // Deduced as int
    std::cout << "max(10, 20) = " << max_val << std::endl;

    // Class template argument deduction (C++17)
    Container container(42);  // Deduced as Container<int>
    std::cout << "container.get() = " << container.get() << std::endl;

    Container string_container(std::string("Hello"));  // Deduced as Container<std::string>
    std::cout << "string_container.get() = " << string_container.get() << std::endl;

    // In TypeScript, you'd write:
    // const maxVal = max(10, 20);  // Type inferred automatically
    // const container = new Container(42);  // Type inferred automatically
}

// =============================================================================
// 6. TEMPLATE SPECIALIZATION
// =============================================================================
// Provide special implementation for specific types
// In TypeScript: Function overloads or conditional types

template<typename T>
class TypeInfo {
public:
    static const char* name() {
        return "unknown";
    }
};

// Specialization for int
template<>
class TypeInfo<int> {
public:
    static const char* name() {
        return "int";
    }
};

// Specialization for double
template<>
class TypeInfo<double> {
public:
    static const char* name() {
        return "double";
    }
};

// TypeScript equivalent:
// type TypeName<T> = 
//     T extends number ? "number" :
//     T extends string ? "string" :
//     "unknown";
// Or function overloads:
// function typeName(value: number): "number";
// function typeName(value: string): "string";
// function typeName(value: any): "unknown";

void demonstrate_specialization() {
    std::cout << "\n=== Template Specialization ===\n";

    std::cout << "TypeInfo<int>::name() = " << TypeInfo<int>::name() << std::endl;
    std::cout << "TypeInfo<double>::name() = " << TypeInfo<double>::name() << std::endl;
    std::cout << "TypeInfo<std::string>::name() = " << TypeInfo<std::string>::name() << std::endl;

    // In TypeScript, you'd write:
    // type IntName = TypeName<number>;  // "number"
    // type StringName = TypeName<string>;  // "string"
}

// =============================================================================
// 7. DEFAULT TEMPLATE PARAMETERS
// =============================================================================
// Provide default types for template parameters
// In TypeScript: Default generic parameters

template<typename T = int, size_t N = 10>
class Buffer {
private:
    T data_[N];

public:
    T& operator[](size_t index) {
        return data_[index];
    }
};

// TypeScript equivalent:
// class Buffer<T = number, N extends number = 10> {
//     private data: T[] = new Array(N);
// }

void demonstrate_default_parameters() {
    std::cout << "\n=== Default Template Parameters ===\n";

    Buffer<> default_buffer;  // Uses int and 10
    Buffer<double> double_buffer;  // Uses double and 10
    Buffer<int, 5> custom_buffer;  // Uses int and 5

    default_buffer[0] = 42;
    double_buffer[0] = 3.14;
    custom_buffer[0] = 100;

    std::cout << "default_buffer[0] = " << default_buffer[0] << std::endl;
    std::cout << "double_buffer[0] = " << double_buffer[0] << std::endl;
    std::cout << "custom_buffer[0] = " << custom_buffer[0] << std::endl;

    // In TypeScript, you'd write:
    // const defaultBuffer = new Buffer();  // Uses number and 10
    // const doubleBuffer = new Buffer<number>();  // Uses number and 10
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "Basic Template Metaprogramming - TypeScript Developer Edition\n";
    std::cout << "=============================================================\n";

    demonstrate_function_templates();
    demonstrate_class_templates();
    demonstrate_value_parameters();
    demonstrate_multiple_parameters();
    demonstrate_type_deduction();
    demonstrate_specialization();
    demonstrate_default_parameters();

    std::cout << "\n=== Key Takeaways for TypeScript Developers ===\n";
    std::cout << "1. Templates = TypeScript generics, but compile-time code generation\n";
    std::cout << "2. Function templates = Generic functions in TypeScript\n";
    std::cout << "3. Class templates = Generic classes in TypeScript\n";
    std::cout << "4. Value parameters = C++ only (template<int N>)\n";
    std::cout << "5. Type deduction = Automatic in both (auto in C++, inference in TS)\n";
    std::cout << "6. Specialization = Function overloads or conditional types in TS\n";
    std::cout << "7. Default parameters = Same concept in both languages\n";
    std::cout << "8. C++ templates generate different code for each type\n";
    std::cout << "9. TypeScript generics are type-checked but erased at runtime\n";
    std::cout << "10. C++ templates have zero runtime overhead\n";

    return 0;
}
