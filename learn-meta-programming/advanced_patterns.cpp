/**
 * Advanced Metaprogramming Patterns - TypeScript Developer Edition
 *
 * This file demonstrates advanced metaprogramming patterns used in production C++:
 * - CRTP (Curiously Recurring Template Pattern)
 * - Expression Templates
 * - Policy-Based Design
 * - Type Erasure
 * - Tag Dispatch
 *
 * These patterns enable zero-overhead abstractions and powerful code reuse.
 * TypeScript equivalents are provided where applicable.
 */

#include <iostream>
#include <vector>
#include <memory>

// =============================================================================
// 1. CRTP (CURIOUSLY RECURRING TEMPLATE PATTERN)
// =============================================================================
// Base class knows about derived class
// In TypeScript: Abstract classes or mixins

template<typename Derived>
class Base {
public:
    void interface() {
        static_cast<Derived*>(this)->implementation();
    }
    
    void call_implementation() {
        static_cast<Derived*>(this)->implementation();
    }
};

class Derived1 : public Base<Derived1> {
public:
    void implementation() {
        std::cout << "Derived1 implementation" << std::endl;
    }
};

class Derived2 : public Base<Derived2> {
public:
    void implementation() {
        std::cout << "Derived2 implementation" << std::endl;
    }
};

// TypeScript equivalent:
// abstract class Base {
//     abstract implementation(): void;
//     interface(): void {
//         this.implementation();
//     }
// }
// class Derived1 extends Base {
//     implementation(): void {
//         console.log("Derived1 implementation");
//     }
// }

void demonstrate_crtp() {
    std::cout << "\n=== CRTP Pattern ===\n";

    Derived1 d1;
    Derived2 d2;
    
    d1.interface();
    d2.interface();
}

// =============================================================================
// 2. EXPRESSION TEMPLATES
// =============================================================================
// Lazy evaluation for mathematical expressions
// In TypeScript: Method chaining (no operator overloading)

template<typename Lhs, typename Rhs>
class AddExpr {
    const Lhs& lhs_;
    const Rhs& rhs_;
public:
    AddExpr(const Lhs& lhs, const Rhs& rhs) : lhs_(lhs), rhs_(rhs) {}
    
    auto operator[](size_t i) const {
        return lhs_[i] + rhs_[i];
    }
    
    size_t size() const {
        return lhs_.size();
    }
};

template<typename T>
class Vector {
    std::vector<T> data_;
public:
    Vector(std::initializer_list<T> init) : data_(init) {}
    
    T& operator[](size_t i) { return data_[i]; }
    const T& operator[](size_t i) const { return data_[i]; }
    size_t size() const { return data_.size(); }
    
    template<typename Expr>
    Vector& operator=(const Expr& expr) {
        for (size_t i = 0; i < expr.size(); ++i) {
            data_[i] = expr[i];
        }
        return *this;
    }
};

template<typename Lhs, typename Rhs>
auto operator+(const Lhs& lhs, const Rhs& rhs) {
    return AddExpr<Lhs, Rhs>(lhs, rhs);
}

// TypeScript equivalent:
// class Vector {
//     add(other: Vector): Vector {
//         return new Vector(this.data.map((v, i) => v + other.data[i]));
//     }
// }

void demonstrate_expression_templates() {
    std::cout << "\n=== Expression Templates ===\n";

    Vector<int> v1{1, 2, 3};
    Vector<int> v2{4, 5, 6};
    Vector<int> v3{0, 0, 0};
    
    v3 = v1 + v2;  // Lazy evaluation, computed when assigned
    std::cout << "v3[0] = " << v3[0] << std::endl;
    std::cout << "v3[1] = " << v3[1] << std::endl;
    std::cout << "v3[2] = " << v3[2] << std::endl;
}

// =============================================================================
// 3. POLICY-BASED DESIGN
// =============================================================================
// Compose behavior through template parameters
// In TypeScript: Dependency injection or mixins

template<typename AllocationPolicy>
class Container {
    AllocationPolicy allocator_;
public:
    void* allocate(size_t size) {
        return allocator_.allocate(size);
    }
    
    void deallocate(void* ptr) {
        allocator_.deallocate(ptr);
    }
};

struct MallocPolicy {
    void* allocate(size_t size) {
        return malloc(size);
    }
    
    void deallocate(void* ptr) {
        free(ptr);
    }
};

struct NewPolicy {
    void* allocate(size_t size) {
        return ::operator new(size);
    }
    
    void deallocate(void* ptr) {
        ::operator delete(ptr);
    }
};

// TypeScript equivalent:
// interface AllocationPolicy {
//     allocate(size: number): void;
//     deallocate(ptr: void): void;
// }
// class Container {
//     constructor(private allocator: AllocationPolicy) {}
//     allocate(size: number): void {
//         return this.allocator.allocate(size);
//     }
// }

void demonstrate_policy_based_design() {
    std::cout << "\n=== Policy-Based Design ===\n";

    Container<MallocPolicy> container1;
    Container<NewPolicy> container2;
    
    void* ptr1 = container1.allocate(100);
    void* ptr2 = container2.allocate(100);
    
    container1.deallocate(ptr1);
    container2.deallocate(ptr2);
    
    std::cout << "Policy-based design allows flexible composition" << std::endl;
}

// =============================================================================
// 4. TYPE ERASURE
// =============================================================================
// Hide concrete types behind interface
// In TypeScript: Interfaces and abstract classes

class TypeErasure {
    struct Concept {
        virtual ~Concept() = default;
        virtual void do_something() = 0;
    };
    
    template<typename T>
    struct Model : Concept {
        T object_;
        Model(T obj) : object_(obj) {}
        void do_something() override {
            object_.do_something();
        }
    };
    
    std::unique_ptr<Concept> object_;
    
public:
    template<typename T>
    TypeErasure(T obj) : object_(std::make_unique<Model<T>>(obj)) {}
    
    void do_something() {
        object_->do_something();
    }
};

class Implementation1 {
public:
    void do_something() {
        std::cout << "Implementation1" << std::endl;
    }
};

class Implementation2 {
public:
    void do_something() {
        std::cout << "Implementation2" << std::endl;
    }
};

// TypeScript equivalent:
// interface DoSomething {
//     doSomething(): void;
// }
// class Implementation1 implements DoSomething {
//     doSomething(): void {
//         console.log("Implementation1");
//     }
// }

void demonstrate_type_erasure() {
    std::cout << "\n=== Type Erasure ===\n";

    TypeErasure erasure1(Implementation1{});
    TypeErasure erasure2(Implementation2{});
    
    erasure1.do_something();
    erasure2.do_something();
}

// =============================================================================
// 5. TAG DISPATCH
// =============================================================================
// Use types as tags for dispatch
// In TypeScript: Discriminated unions or type guards

struct tag_fast {};
struct tag_safe {};

template<typename Tag>
void algorithm_impl(Tag) {
    if constexpr (std::is_same_v<Tag, tag_fast>) {
        std::cout << "Fast algorithm" << std::endl;
    } else {
        std::cout << "Safe algorithm" << std::endl;
    }
}

template<typename Tag = tag_safe>
void algorithm() {
    algorithm_impl(Tag{});
}

// TypeScript equivalent:
// type AlgorithmTag = "fast" | "safe";
// function algorithm(tag: AlgorithmTag = "safe"): void {
//     if (tag === "fast") {
//         console.log("Fast algorithm");
//     } else {
//         console.log("Safe algorithm");
//     }
// }

void demonstrate_tag_dispatch() {
    std::cout << "\n=== Tag Dispatch ===\n";

    algorithm<tag_fast>();
    algorithm<tag_safe>();
    algorithm();  // Default to safe
}

// =============================================================================
// 6. TRAITS-BASED SPECIALIZATION
// =============================================================================
// Use traits to select implementation
// In TypeScript: Conditional types

template<typename T>
struct numeric_traits {
    static constexpr bool is_signed = true;
};

template<>
struct numeric_traits<unsigned int> {
    static constexpr bool is_signed = false;
};

template<typename T>
void process_numeric(T value) {
    if constexpr (numeric_traits<T>::is_signed) {
        std::cout << "Processing signed: " << value << std::endl;
    } else {
        std::cout << "Processing unsigned: " << value << std::endl;
    }
}

// TypeScript equivalent:
// type NumericTraits<T> = T extends number 
//     ? { isSigned: true }
//     : never;

void demonstrate_traits_specialization() {
    std::cout << "\n=== Traits-Based Specialization ===\n";

    process_numeric(42);
    process_numeric(42u);
}

// =============================================================================
// 7. MIXIN PATTERN
// =============================================================================
// Compose functionality through inheritance
// In TypeScript: Mixins are more natural

template<typename Base>
class PrintableMixin : public Base {
public:
    void print() const {
        std::cout << "Printable object" << std::endl;
    }
};

template<typename Base>
class SerializableMixin : public Base {
public:
    std::string serialize() const {
        return "serialized";
    }
};

class BasicClass {};

using EnhancedClass = SerializableMixin<PrintableMixin<BasicClass>>;

// TypeScript equivalent:
// class PrintableMixin {
//     print(): void {
//         console.log("Printable object");
//     }
// }
// class SerializableMixin {
//     serialize(): string {
//         return "serialized";
//     }
// }
// class EnhancedClass extends SerializableMixin(PrintableMixin(BasicClass)) {}

void demonstrate_mixin_pattern() {
    std::cout << "\n=== Mixin Pattern ===\n";

    EnhancedClass obj;
    obj.print();
    std::cout << obj.serialize() << std::endl;
}

// =============================================================================
// 8. SFINAE-BASED OVERLOADING
// =============================================================================
// Select overloads based on type properties
// In TypeScript: Function overloads

template<typename T>
typename std::enable_if_t<std::is_integral_v<T>, void>
process(T value) {
    std::cout << "Processing integral: " << value << std::endl;
}

template<typename T>
typename std::enable_if_t<std::is_floating_point_v<T>, void>
process(T value) {
    std::cout << "Processing floating point: " << value << std::endl;
}

// TypeScript equivalent:
// function process(value: number): void;
// function process(value: number): void {
//     console.log("Processing:", value);
// }

void demonstrate_sfinae_overloading() {
    std::cout << "\n=== SFINAE-Based Overloading ===\n";

    process(42);
    process(3.14);
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "Advanced Metaprogramming Patterns - TypeScript Developer Edition\n";
    std::cout << "================================================================\n";

    demonstrate_crtp();
    demonstrate_expression_templates();
    demonstrate_policy_based_design();
    demonstrate_type_erasure();
    demonstrate_tag_dispatch();
    demonstrate_traits_specialization();
    demonstrate_mixin_pattern();
    demonstrate_sfinae_overloading();

    std::cout << "\n=== Key Takeaways for TypeScript Developers ===\n";
    std::cout << "1. CRTP = Abstract classes or mixins in TypeScript\n";
    std::cout << "2. Expression templates = Method chaining (no operators)\n";
    std::cout << "3. Policy-based design = Dependency injection\n";
    std::cout << "4. Type erasure = Interfaces and abstract classes\n";
    std::cout << "5. Tag dispatch = Discriminated unions\n";
    std::cout << "6. Traits specialization = Conditional types\n";
    std::cout << "7. Mixin pattern = More natural in TypeScript\n";
    std::cout << "8. SFINAE overloading = Function overloads\n";
    std::cout << "9. C++ patterns enable zero-overhead abstractions\n";
    std::cout << "10. TypeScript patterns are more ergonomic but less powerful\n";

    return 0;
}
