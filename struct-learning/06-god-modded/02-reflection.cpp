/*
 * =============================================================================
 * God Modded: Reflection
 * Lightweight reflection registry for struct fields
 * =============================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <functional>

template<typename T>
struct FieldInfo {
    const char* name;
    std::function<std::string(const T&)> getter;
};

template<typename T>
struct TypeInfo {
    const char* type_name;
    std::vector<FieldInfo<T>> fields;
};

struct UserRecord {
    uint64_t id;
    std::string name;
    int age;
};

static TypeInfo<UserRecord> make_user_typeinfo() {
    TypeInfo<UserRecord> ti{ "UserRecord", {} };
    ti.fields.push_back({"id",   [](const UserRecord& u){ return std::to_string(u.id); }});
    ti.fields.push_back({"name", [](const UserRecord& u){ return u.name; }});
    ti.fields.push_back({"age",  [](const UserRecord& u){ return std::to_string(u.age); }});
    return ti;
}

void demo_reflection() {
    std::cout << "\n=== GOD MODDED: REFLECTION ===" << std::endl;
    UserRecord u{ 999, "Ada", 37 };
    auto ti = make_user_typeinfo();
    for (const auto& f : ti.fields) {
        std::cout << ti.type_name << '.' << f.name << " = " << f.getter(u) << std::endl;
    }
}

int main() {
    try { demo_reflection(); std::cout << "\n=== REFLECTION COMPLETED SUCCESSFULLY ===" << std::endl; }
    catch (...) { std::cerr << "error" << std::endl; return 1; }
    return 0;
}
