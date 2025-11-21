/**
 * Reflection and Introspection - TypeScript Developer Edition
 *
 * Reflection allows code to inspect and manipulate its own structure at compile-time
 * or runtime. C++ has limited reflection compared to TypeScript, but C++20/23 are
 * adding more reflection capabilities.
 *
 * In TypeScript: typeof, keyof, in operator, type guards
 * In C++: typeid, type traits, limited runtime reflection
 *
 * Key concepts:
 * - Type introspection: Querying type information
 * - Member inspection: Checking for members
 * - Runtime type information (RTTI): typeid, dynamic_cast
 * - Compile-time reflection: Type traits, concepts
 */

#include <iostream>
#include <typeinfo>
#include <type_traits>
#include <string>
#include <vector>

// =============================================================================
// 1. RUNTIME TYPE INFORMATION (RTTI)
// =============================================================================
// In TypeScript: typeof value, instanceof

class Base {
public:
    virtual ~Base() = default;
    virtual void print() const { std::cout << "Base" << std::endl; }
};

class Derived : public Base {
public:
    void print() const override { std::cout << "Derived" << std::endl; }
};

void demonstrate_rtti() {
    std::cout << "\n=== Runtime Type Information ===\n";

    Base* ptr = new Derived();
    
    // Get type name (implementation-defined)
    std::cout << "typeid(*ptr).name() = " << typeid(*ptr).name() << std::endl;
    
    // Check if types are the same
    if (typeid(*ptr) == typeid(Derived)) {
        std::cout << "ptr points to Derived" << std::endl;
    }
    
    // Dynamic cast (runtime type checking)
    Derived* derived_ptr = dynamic_cast<Derived*>(ptr);
    if (derived_ptr) {
        std::cout << "Successfully cast to Derived" << std::endl;
    }
    
    delete ptr;

    // TypeScript equivalent:
    // class Base { print() { console.log("Base"); } }
    // class Derived extends Base { print() { console.log("Derived"); } }
    // const ptr: Base = new Derived();
    // console.log(typeof ptr);  // "object"
    // console.log(ptr instanceof Derived);  // true
}

// =============================================================================
// 2. COMPILE-TIME TYPE INTROSPECTION
// =============================================================================
// Using type traits for introspection
// In TypeScript: Conditional types, type guards

template<typename T>
void inspect_type() {
    std::cout << "\nType: " << typeid(T).name() << std::endl;
    std::cout << "  Is integral: " << std::is_integral_v<T> << std::endl;
    std::cout << "  Is pointer: " << std::is_pointer_v<T> << std::endl;
    std::cout << "  Is class: " << std::is_class_v<T> << std::endl;
    std::cout << "  Size: " << sizeof(T) << " bytes" << std::endl;
}

// TypeScript equivalent:
// function inspectType<T>(): void {
//     const value: unknown = {} as T;
//     console.log("Type:", typeof value);
//     console.log("Is number:", typeof value === 'number');
// }

void demonstrate_compile_time_introspection() {
    std::cout << "\n=== Compile-Time Type Introspection ===\n";

    inspect_type<int>();
    inspect_type<double>();
    inspect_type<std::string>();
}

// =============================================================================
// 3. MEMBER INSPECTION
// =============================================================================
// Check if types have specific members
// In TypeScript: keyof, in operator

template<typename T, typename = void>
struct has_size_method : std::false_type {};

template<typename T>
struct has_size_method<T, std::void_t<decltype(std::declval<T>().size())>> 
    : std::true_type {};

template<typename T>
constexpr bool has_size_method_v = has_size_method<T>::value;

// TypeScript equivalent:
// type HasSize<T> = "size" extends keyof T ? true : false;
// Or: type HasSize<T> = T extends { size(): number } ? true : false;

template<typename T>
void check_members() {
    if constexpr (has_size_method_v<T>) {
        std::cout << "Type has size() method" << std::endl;
    } else {
        std::cout << "Type does not have size() method" << std::endl;
    }
}

void demonstrate_member_inspection() {
    std::cout << "\n=== Member Inspection ===\n";

    check_members<std::vector<int>>();
    check_members<int>();
    check_members<std::string>();
}

// =============================================================================
// 4. TYPE NAME STRINGIFICATION
// =============================================================================
// Get type names as strings
// In TypeScript: typeof, type names

template<typename T>
const char* type_name() {
    return typeid(T).name();
}

// Better version using template specialization
template<typename T>
struct TypeName {
    static const char* name() {
        return typeid(T).name();
    }
};

template<>
struct TypeName<int> {
    static const char* name() { return "int"; }
};

template<>
struct TypeName<double> {
    static const char* name() { return "double"; }
};

template<>
struct TypeName<std::string> {
    static const char* name() { return "std::string"; }
};

// TypeScript equivalent:
// type TypeName<T> = 
//     T extends number ? "number" :
//     T extends string ? "string" :
//     T extends boolean ? "boolean" :
//     "unknown";

void demonstrate_type_name_stringification() {
    std::cout << "\n=== Type Name Stringification ===\n";

    std::cout << "TypeName<int>::name() = " << TypeName<int>::name() << std::endl;
    std::cout << "TypeName<double>::name() = " << TypeName<double>::name() << std::endl;
    std::cout << "TypeName<std::string>::name() = " << TypeName<std::string>::name() << std::endl;
}

// =============================================================================
// 5. PROPERTY INSPECTION
// =============================================================================
// Check for specific properties
// In TypeScript: keyof, mapped types

template<typename T>
struct has_value_type {
private:
    template<typename U>
    static auto test(int) -> decltype(std::declval<typename U::value_type>(), std::true_type{});
    
    template<typename>
    static std::false_type test(...);
    
public:
    static constexpr bool value = decltype(test<T>(0))::value;
};

template<typename T>
constexpr bool has_value_type_v = has_value_type<T>::value;

// TypeScript equivalent:
// type HasValueType<T> = "valueType" extends keyof T ? true : false;
// Or: type HasValueType<T> = T extends { valueType: infer U } ? true : false;

void demonstrate_property_inspection() {
    std::cout << "\n=== Property Inspection ===\n";

    std::cout << "has_value_type_v<std::vector<int>> = " 
              << has_value_type_v<std::vector<int>> << std::endl;
    std::cout << "has_value_type_v<int> = " 
              << has_value_type_v<int> << std::endl;
}

// =============================================================================
// 6. METHOD SIGNATURE INSPECTION
// =============================================================================
// Check method signatures
// In TypeScript: Function types, method signatures

template<typename T>
struct has_print_method {
private:
    template<typename U>
    static auto test(int) -> decltype(
        std::declval<U>().print(),
        std::true_type{}
    );
    
    template<typename>
    static std::false_type test(...);
    
public:
    static constexpr bool value = decltype(test<T>(0))::value;
};

template<typename T>
constexpr bool has_print_method_v = has_print_method<T>::value;

// TypeScript equivalent:
// type HasPrintMethod<T> = T extends { print(): void } ? true : false;

class HasPrint {
public:
    void print() const { std::cout << "Has print method" << std::endl; }
};

class NoPrint {};

void demonstrate_method_signature_inspection() {
    std::cout << "\n=== Method Signature Inspection ===\n";

    std::cout << "has_print_method_v<HasPrint> = " 
              << has_print_method_v<HasPrint> << std::endl;
    std::cout << "has_print_method_v<NoPrint> = " 
              << has_print_method_v<NoPrint> << std::endl;
}

// =============================================================================
// 7. TYPE GUARDS (C++ STYLE)
// =============================================================================
// Compile-time type guards
// In TypeScript: Type guards, type predicates

template<typename T>
constexpr bool is_numeric() {
    return std::is_arithmetic_v<T>;
}

template<typename T>
constexpr bool is_container() {
    return has_size_method_v<T> && !std::is_same_v<T, std::string>;
}

// TypeScript equivalent:
// function isNumeric(value: unknown): value is number {
//     return typeof value === 'number';
// }
// function isContainer(value: unknown): value is any[] {
//     return Array.isArray(value);
// }

template<typename T>
void process_type(T value) {
    if constexpr (is_numeric<T>()) {
        std::cout << "Processing numeric: " << value << std::endl;
    } else if constexpr (is_container<T>()) {
        std::cout << "Processing container, size: " << value.size() << std::endl;
    } else {
        std::cout << "Processing other type" << std::endl;
    }
}

void demonstrate_type_guards() {
    std::cout << "\n=== Type Guards ===\n";

    process_type(42);
    process_type(std::vector<int>{1, 2, 3});
    process_type(std::string("Hello"));
}

// =============================================================================
// 8. METADATA EXTRACTION
// =============================================================================
// Extract type metadata
// In TypeScript: Mapped types, utility types

template<typename T>
struct TypeMetadata {
    using type = T;
    using decayed = std::decay_t<T>;
    using pointer = std::add_pointer_t<T>;
    using reference = std::add_lvalue_reference_t<T>;
    
    static constexpr bool is_integral = std::is_integral_v<T>;
    static constexpr bool is_pointer = std::is_pointer_v<T>;
    static constexpr size_t size = sizeof(T);
};

// TypeScript equivalent:
// type TypeMetadata<T> = {
//     type: T;
//     isNumber: T extends number ? true : false;
//     isString: T extends string ? true : false;
// };

void demonstrate_metadata_extraction() {
    std::cout << "\n=== Metadata Extraction ===\n";

    std::cout << "TypeMetadata<int>::size = " << TypeMetadata<int>::size << std::endl;
    std::cout << "TypeMetadata<int>::is_integral = " 
              << TypeMetadata<int>::is_integral << std::endl;
}

// =============================================================================
// 9. C++20 REFLECTION (EXPERIMENTAL)
// =============================================================================
// C++20 reflection proposal (not yet standard)

// Note: This is experimental and may not be available
// The reflection TS is being worked on

// TypeScript equivalent:
// TypeScript has excellent reflection support:
// - typeof operator
// - keyof operator
// - in operator
// - Mapped types
// - Template literal types

void demonstrate_cpp20_reflection() {
    std::cout << "\n=== C++20 Reflection (Experimental) ===\n";

    std::cout << "C++20 reflection is still experimental" << std::endl;
    std::cout << "Use type traits and concepts for now" << std::endl;
    std::cout << "TypeScript has better reflection support currently" << std::endl;
}

// =============================================================================
// 10. PRACTICAL REFLECTION PATTERNS
// =============================================================================
// Real-world reflection usage

template<typename T>
void serialize_if_possible(const T& value) {
    if constexpr (has_print_method_v<T>) {
        value.print();
    } else if constexpr (std::is_arithmetic_v<T>) {
        std::cout << "Numeric value: " << value << std::endl;
    } else {
        std::cout << "Cannot serialize type" << std::endl;
    }
}

// TypeScript equivalent:
// function serializeIfPossible<T>(value: T): void {
//     if ('print' in value && typeof (value as any).print === 'function') {
//         (value as any).print();
//     } else if (typeof value === 'number') {
//         console.log("Numeric value:", value);
//     } else {
//         console.log("Cannot serialize type");
//     }
// }

void demonstrate_practical_patterns() {
    std::cout << "\n=== Practical Reflection Patterns ===\n";

    serialize_if_possible(42);
    serialize_if_possible(HasPrint{});
    serialize_if_possible(std::string("Hello"));
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "Reflection and Introspection - TypeScript Developer Edition\n";
    std::cout << "===========================================================\n";

    demonstrate_rtti();
    demonstrate_compile_time_introspection();
    demonstrate_member_inspection();
    demonstrate_type_name_stringification();
    demonstrate_property_inspection();
    demonstrate_method_signature_inspection();
    demonstrate_type_guards();
    demonstrate_metadata_extraction();
    demonstrate_cpp20_reflection();
    demonstrate_practical_patterns();

    std::cout << "\n=== Key Takeaways for TypeScript Developers ===\n";
    std::cout << "1. RTTI = typeof and instanceof in TypeScript\n";
    std::cout << "2. Type introspection = Conditional types in TypeScript\n";
    std::cout << "3. Member inspection = keyof and in operator\n";
    std::cout << "4. Type guards = Type predicates in TypeScript\n";
    std::cout << "5. C++ reflection is more limited than TypeScript\n";
    std::cout << "6. TypeScript has better runtime reflection\n";
    std::cout << "7. C++ has better compile-time reflection (type traits)\n";
    std::cout << "8. C++20 reflection is experimental\n";
    std::cout << "9. Use type traits for compile-time introspection\n";
    std::cout << "10. Use RTTI sparingly (performance cost)\n";

    return 0;
}
