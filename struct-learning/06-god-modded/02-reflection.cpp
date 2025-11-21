/*
 * =============================================================================
 * God Modded: Advanced Reflection - Runtime Struct Introspection
 * Production-Grade Reflection for Top-Tier Companies
 * =============================================================================
 *
 * This file demonstrates advanced reflection techniques including:
 * - Compile-time field introspection using pointer-to-member
 * - Type-safe field access with offsetof tricks
 * - Automatic field discovery via macros
 * - Zero-overhead reflection for hot paths
 * - Google-style reflection registry with caching
 *
 * Author: System Engineering Team
 * Version: 2.0
 * Last Modified: 2024-01-15
 *
 * =============================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include <type_traits>
#include <cstddef>
#include <cstring>
#include <memory>

// =============================================================================
// ADVANCED TYPE TRAITS FOR REFLECTION
// =============================================================================

template<typename T>
struct is_reflectable : std::false_type {};

// Enable reflection for specific types
#define ENABLE_REFLECTION(Type) \
    template<> \
    struct is_reflectable<Type> : std::true_type {}

// =============================================================================
// POINTER-TO-MEMBER BASED REFLECTION (ZERO OVERHEAD)
// =============================================================================

template<typename StructType, typename FieldType>
struct FieldDescriptor {
    const char* name;
    FieldType StructType::*member_ptr;
    size_t offset;
    const char* type_name;
    
    FieldDescriptor(const char* n, FieldType StructType::*ptr, const char* tn)
        : name(n), member_ptr(ptr), offset(reinterpret_cast<size_t>(&(static_cast<StructType*>(nullptr)->*ptr))), type_name(tn) {}
    
    // Zero-overhead field access
    const FieldType& get(const StructType& obj) const {
        return obj.*member_ptr;
    }
    
    FieldType& get(StructType& obj) const {
        return obj.*member_ptr;
    }
    
    // Type-safe setter
    void set(StructType& obj, const FieldType& value) const {
        obj.*member_ptr = value;
    }
};

// =============================================================================
// COMPILE-TIME FIELD REGISTRY
// =============================================================================

template<typename T>
class ReflectionRegistry {
private:
    struct FieldInfo {
        const char* name;
        size_t offset;
        size_t size;
        const char* type_name;
        std::function<std::string(const void*)> serializer;
        std::function<void(void*, const std::string&)> deserializer;
    };
    
    std::vector<FieldInfo> fields_;
    std::unordered_map<std::string, size_t> name_to_index_;
    static inline ReflectionRegistry<T>* instance_ = nullptr;
    
    ReflectionRegistry() = default;
    
public:
    static ReflectionRegistry<T>& instance() {
        if (!instance_) {
            instance_ = new ReflectionRegistry<T>();
        }
        return *instance_;
    }
    
    template<typename FieldType>
    void register_field(const char* name, FieldType T::*member_ptr, const char* type_name) {
        FieldInfo info;
        info.name = name;
        info.offset = reinterpret_cast<size_t>(&(static_cast<T*>(nullptr)->*member_ptr));
        info.size = sizeof(FieldType);
        info.type_name = type_name;
        
        // Type-specific serialization
        info.serializer = [member_ptr](const void* obj_ptr) -> std::string {
            const T* obj = static_cast<const T*>(obj_ptr);
            const FieldType& field = obj->*member_ptr;
            if constexpr (std::is_integral_v<FieldType>) {
                return std::to_string(field);
            } else if constexpr (std::is_floating_point_v<FieldType>) {
                return std::to_string(field);
            } else if constexpr (std::is_same_v<FieldType, std::string>) {
                return field;
            } else {
                return "[complex type]";
            }
        };
        
        fields_.push_back(info);
        name_to_index_[name] = fields_.size() - 1;
    }
    
    size_t field_count() const { return fields_.size(); }
    
    const FieldInfo& get_field(size_t index) const {
        return fields_[index];
    }
    
    const FieldInfo* find_field(const char* name) const {
        auto it = name_to_index_.find(name);
        if (it != name_to_index_.end()) {
            return &fields_[it->second];
        }
        return nullptr;
    }
    
    // Serialize entire struct to JSON
    std::string to_json(const T& obj) const {
        std::string result = "{";
        for (size_t i = 0; i < fields_.size(); ++i) {
            if (i > 0) result += ",";
            result += "\"" + std::string(fields_[i].name) + "\":";
            result += "\"" + fields_[i].serializer(&obj) + "\"";
        }
        result += "}";
        return result;
    }
    
    // Get field value by name (type-safe)
    template<typename FieldType>
    bool get_field_value(const T& obj, const char* name, FieldType& out) const {
        const FieldInfo* info = find_field(name);
        if (!info) return false;
        
        const void* field_ptr = reinterpret_cast<const char*>(&obj) + info->offset;
        if constexpr (std::is_same_v<FieldType, std::string>) {
            out = *reinterpret_cast<const std::string*>(field_ptr);
        } else {
            out = *reinterpret_cast<const FieldType*>(field_ptr);
        }
        return true;
    }
};

// =============================================================================
// MACRO-BASED FIELD REGISTRATION (HACKY BUT POWERFUL)
// =============================================================================

#define REGISTER_FIELD(Registry, StructType, FieldName) \
    Registry.register_field(#FieldName, &StructType::FieldName, typeid(StructType::FieldName).name())

// =============================================================================
// ADVANCED STRUCT WITH REFLECTION
// =============================================================================

struct UserRecord {
    uint64_t id;
    std::string name;
    int age;
    double balance;
    bool is_active;
    
    // Reflection support
    static void register_reflection() {
        auto& reg = ReflectionRegistry<UserRecord>::instance();
        REGISTER_FIELD(reg, UserRecord, id);
        REGISTER_FIELD(reg, UserRecord, name);
        REGISTER_FIELD(reg, UserRecord, age);
        REGISTER_FIELD(reg, UserRecord, balance);
        REGISTER_FIELD(reg, UserRecord, is_active);
    }
};

ENABLE_REFLECTION(UserRecord);

// =============================================================================
// POINTER-TO-MEMBER DESCRIPTOR APPROACH (UBER-STYLE)
// =============================================================================

struct UserRecordDescriptors {
    static inline FieldDescriptor<UserRecord, uint64_t> id_desc{"id", &UserRecord::id, "uint64_t"};
    static inline FieldDescriptor<UserRecord, std::string> name_desc{"name", &UserRecord::name, "std::string"};
    static inline FieldDescriptor<UserRecord, int> age_desc{"age", &UserRecord::age, "int"};
    static inline FieldDescriptor<UserRecord, double> balance_desc{"balance", &UserRecord::balance, "double"};
    static inline FieldDescriptor<UserRecord, bool> is_active_desc{"is_active", &UserRecord::is_active, "bool"};
    
    static void print_all(const UserRecord& u) {
        std::cout << "id: " << id_desc.get(u) << std::endl;
        std::cout << "name: " << name_desc.get(u) << std::endl;
        std::cout << "age: " << age_desc.get(u) << std::endl;
        std::cout << "balance: " << balance_desc.get(u) << std::endl;
        std::cout << "is_active: " << is_active_desc.get(u) << std::endl;
    }
};

// =============================================================================
// COMPILE-TIME FIELD ITERATION (BLOOMBERG-STYLE)
// =============================================================================

template<typename T, typename Visitor>
void visit_fields(const T& obj, Visitor&& visitor) {
    if constexpr (std::is_same_v<T, UserRecord>) {
        visitor("id", obj.id);
        visitor("name", obj.name);
        visitor("age", obj.age);
        visitor("balance", obj.balance);
        visitor("is_active", obj.is_active);
    }
}

// =============================================================================
// DEMONSTRATION FUNCTIONS
// =============================================================================

void demonstrate_basic_reflection() {
    std::cout << "\n=== BASIC REFLECTION ===" << std::endl;
    UserRecord u{999, "Ada", 37, 1234.56, true};
    
    UserRecordDescriptors::print_all(u);
}

void demonstrate_registry_reflection() {
    std::cout << "\n=== REGISTRY-BASED REFLECTION ===" << std::endl;
    
    // Register fields
    UserRecord::register_reflection();
    
    UserRecord u{12345, "Bob", 42, 5678.90, false};
    auto& reg = ReflectionRegistry<UserRecord>::instance();
    
    std::cout << "Field count: " << reg.field_count() << std::endl;
    std::cout << "JSON: " << reg.to_json(u) << std::endl;
    
    // Access field by name
    std::string name;
    if (reg.get_field_value(u, "name", name)) {
        std::cout << "Retrieved name: " << name << std::endl;
    }
    
    int age;
    if (reg.get_field_value(u, "age", age)) {
        std::cout << "Retrieved age: " << age << std::endl;
    }
}

void demonstrate_compile_time_iteration() {
    std::cout << "\n=== COMPILE-TIME FIELD ITERATION ===" << std::endl;
    UserRecord u{98765, "Charlie", 28, 999.99, true};
    
    visit_fields(u, [](const char* name, const auto& value) {
        std::cout << name << " = ";
        if constexpr (std::is_same_v<std::decay_t<decltype(value)>, std::string>) {
            std::cout << value;
        } else {
            std::cout << value;
        }
        std::cout << std::endl;
    });
}

void demonstrate_zero_overhead_access() {
    std::cout << "\n=== ZERO-OVERHEAD FIELD ACCESS ===" << std::endl;
    UserRecord u{11111, "David", 35, 2222.22, true};
    
    // Direct pointer-to-member access (zero overhead)
    auto& id = UserRecordDescriptors::id_desc.get(u);
    auto& name = UserRecordDescriptors::name_desc.get(u);
    
    std::cout << "Direct access - id: " << id << ", name: " << name << std::endl;
    
    // Modify via descriptor
    UserRecordDescriptors::age_desc.set(u, 50);
    std::cout << "Modified age: " << u.age << std::endl;
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "=== GOD-MODDED ADVANCED REFLECTION ===" << std::endl;
    std::cout << "Demonstrating production-grade reflection techniques" << std::endl;
    
    try {
        demonstrate_basic_reflection();
        demonstrate_registry_reflection();
        demonstrate_compile_time_iteration();
        demonstrate_zero_overhead_access();
        
        std::cout << "\n=== REFLECTION COMPLETED SUCCESSFULLY ===" << std::endl;
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
 *   g++ -std=c++17 -O2 -Wall -Wextra -o reflection 02-reflection.cpp
 *   clang++ -std=c++17 -O2 -Wall -Wextra -o reflection 02-reflection.cpp
 *
 * Advanced reflection techniques:
 *   - Pointer-to-member for zero-overhead access
 *   - Compile-time field iteration
 *   - Runtime registry with caching
 *   - Type-safe field access
 *   - Automatic JSON serialization
 */
