/**
 * Namespace Pitfalls and Gotchas - JavaScript/TypeScript Developer Edition
 *
 * Common namespace mistakes that can break large codebases.
 * Think of these as "don't do this in your modules" rules.
 *
 * JS/TS equivalents:
 * - Using directive in header = `import * from 'library'` in every file
 * - ADL ambiguity = Two modules exporting same function name
 * - Wrong specialization = Putting template code in wrong module
 * - Missing qualification = Forgetting module prefix
 *
 * These examples show what NOT to do and why!
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

// =============================================================================
// PITFALL 1: USING DIRECTIVE IN HEADER FILES - NEVER DO THIS!
// =============================================================================

// BAD EXAMPLE (commented out to prevent actual issues):
// mylibrary.h - DON'T WRITE THIS!
// #ifndef MYLIBRARY_H
// #define MYLIBRARY_H
//
// using namespace std;  // EVIL: Pollutes EVERY file that includes this header!
//
// class MyClass {
//     vector<string> data;  // Now ALL including files have std:: visible
// };
//
// #endif

// In JS/TS terms: This is like putting `import * from 'lodash'` in a header file
// that gets included in 100 other files. Now EVERY file has lodash globals!

// CORRECT APPROACH:
namespace mylibrary {

    // Explicit using declarations are OK in headers (but rare)
    // Like importing specific things: import { vector, string } from 'std';
    using std::vector;
    using std::string;

    class MyClass {
    public:
        vector<string> data;  // Clear, explicit dependency on std::vector
    };

    void processData(const vector<string>& data) {
        // Implementation
    }

}  // namespace mylibrary

// =============================================================================
// PITFALL 2: ADL AMBIGUITY
// =============================================================================

namespace library1 {
    class Data { };

    void process(Data& d) {
        std::cout << "Library1 processing" << std::endl;
    }
}

namespace library2 {
    class Data { };

    void process(library2::Data& d) {
        std::cout << "Library2 processing" << std::endl;
    }
}

void demonstrate_adl_ambiguity() {
    std::cout << "\n=== ADL Ambiguity (Avoided) ===\n";

    library1::Data d1;
    library2::Data d2;

    // These work fine - no ambiguity
    library1::process(d1);
    library2::process(d2);

    // But this would be ambiguous if uncommented:
    // using namespace library1;
    // using namespace library2;
    // process(d1);  // ERROR: ambiguous - which process?
}

// =============================================================================
// PITFALL 3: TEMPLATE SPECIALIZATION IN WRONG NAMESPACE
// =============================================================================

namespace bloomberg {
    namespace container {

        template<typename T>
        class Vector {
        public:
            void push_back(const T& value) {
                data_.push_back(value);
            }
            std::size_t size() const { return data_.size(); }

        private:
            std::vector<T> data_;
        };

        // Template function in same namespace
        template<typename T>
        void sort(Vector<T>& v) {
            std::sort(v.data_.begin(), v.data_.end());
        }

    }  // namespace container
}  // namespace bloomberg

// CORRECT: Specialize in same namespace as primary template
namespace bloomberg {
    namespace container {

        // Specialization for const char* - correct
        template<>
        void sort(Vector<const char*>& v) {
            // Custom sorting for C-strings
            std::cout << "Custom sort for const char*" << std::endl;
        }

    }  // namespace container
}  // namespace bloomberg

// WRONG APPROACH (commented out):
// namespace wrong_namespace {
//     // ERROR: Specialization must be in same namespace as primary template
//     // template<>
//     // void bloomberg::container::sort(Vector<int>& v) { }
// }

// =============================================================================
// PITFALL 4: FRIEND DECLARATIONS AND NAMESPACES
// =============================================================================

namespace trading {

class Order {
public:
    Order(const std::string& symbol, double price)
        : symbol_(symbol), price_(price) {}

private:
    std::string symbol_;
    double price_;

    // Friend function declaration
    friend void printOrder(const Order& order);
};

// Friend function definition must be in same namespace as class
void printOrder(const Order& order) {
    std::cout << "Order: " << order.symbol_ << " @ $" << order.price_ << std::endl;
}

}  // namespace trading

// =============================================================================
// PITFALL 5: STATIC MEMBERS AND QUALIFIED NAMES
// =============================================================================

class bloomberg::container::Vector<int>;  // Forward declaration

namespace bloomberg {
    namespace container {

        // Static member declaration
        template<typename T>
        class Vector<T>::StaticHelper {
        public:
            static int instanceCount;
        };

        // Definition must be qualified
        template<typename T>
        int Vector<T>::StaticHelper::instanceCount = 0;

        // For explicit specializations
        template<>
        class Vector<int> {
        public:
            static int getGlobalCount() {
                return StaticHelper::instanceCount;
            }
        };

    }  // namespace container
}  // namespace bloomberg

// =============================================================================
// PITFALL 6: INLINED NAMESPACE VERSIONING PROBLEMS
// =============================================================================

namespace bloomberg {
    inline namespace v1 {

        class API {
        public:
            void doSomething() {
                std::cout << "API v1 implementation" << std::endl;
            }
        };

    }  // namespace v1

    namespace v2 {

        class API {
        public:
            void doSomething() {
                std::cout << "API v2 implementation" << std::endl;
            }

            void newFeature() {
                std::cout << "API v2 new feature" << std::endl;
            }
        };

    }  // namespace v2

}  // namespace bloomberg

void demonstrate_inline_namespace_gotcha() {
    std::cout << "\n=== Inline Namespace Gotchas ===\n";

    bloomberg::API api;        // Uses v1 (inline)
    bloomberg::v1::API api1;   // Explicit v1
    bloomberg::v2::API api2;   // Explicit v2

    api.doSomething();         // Calls v1
    api1.doSomething();        // Calls v1
    api2.doSomething();        // Calls v2
    api2.newFeature();         // v2 only

    // PITFALL: When you make v2 inline, all existing code switches to v2
    // This can break backward compatibility silently
}

// =============================================================================
// PITFALL 7: ANONYMOUS NAMESPACE LINKAGE ISSUES
// =============================================================================

namespace {

// This has internal linkage
int globalCounter = 0;

// This function is file-local
void incrementCounter() {
    ++globalCounter;
}

class FileLocalClass {
public:
    static int getCounter() { return globalCounter; }
};

}  // anonymous namespace

// PITFALL: Anonymous namespace variables are not shared across TUs
// Each translation unit gets its own copy

void demonstrate_anonymous_namespace_gotcha() {
    std::cout << "\n=== Anonymous Namespace Linkage ===\n";

    incrementCounter();
    std::cout << "Counter in this TU: " << FileLocalClass::getCounter() << std::endl;

    // If this were in a different .cpp file, it would have its own counter
    // Changes here don't affect other translation units
}

// =============================================================================
// PITFALL 8: NAMESPACE LOOKUP ORDER ISSUES
// =============================================================================

namespace outer {
    int value = 10;

    namespace inner {
        int value = 20;

        void func() {
            std::cout << "inner::value = " << value << std::endl;        // 20
            std::cout << "outer::value = " << outer::value << std::endl; // 10
        }
    }

    void func() {
        std::cout << "outer::value = " << value << std::endl;  // 10
        // inner::value is not accessible here
    }
}

void demonstrate_lookup_order() {
    std::cout << "\n=== Namespace Lookup Order ===\n";

    outer::inner::func();
    outer::func();
}

// =============================================================================
// PITFALL 9: ADL WITH OVERLOADED FUNCTIONS
// =============================================================================

namespace math {

class Vector {
public:
    Vector(double x, double y) : x_(x), y_(y) {}
    double x() const { return x_; }
    double y() const { return y_; }

private:
    double x_, y_;
};

// Multiple overloads for ADL
Vector operator+(const Vector& a, const Vector& b) {
    return Vector(a.x() + b.x(), a.y() + b.y());
}

Vector operator*(const Vector& v, double scalar) {
    return Vector(v.x() * scalar, v.y() * scalar);
}

Vector operator*(double scalar, const Vector& v) {
    return v * scalar;  // Reuse the other overload
}

}  // namespace math

void demonstrate_adl_overload_gotcha() {
    std::cout << "\n=== ADL Overload Gotchas ===\n";

    math::Vector a(1.0, 2.0);
    math::Vector b(3.0, 4.0);

    math::Vector sum = a + b;        // ADL finds math::operator+
    math::Vector scaled = a * 2.0;   // ADL finds math::operator*
    math::Vector scaled2 = 2.0 * a;  // ADL finds math::operator*

    std::cout << "Sum: (" << sum.x() << ", " << sum.y() << ")" << std::endl;
    std::cout << "Scaled: (" << scaled.x() << ", " << scaled.y() << ")" << std::endl;
}

// =============================================================================
// PITFALL 10: NAMESPACE POLLUTION IN MACROS
// =============================================================================

// PROBLEMATIC MACRO (don't do this):
#define DECLARE_LOGGER(name) \
    namespace { \
        class Logger_ ## name { \
        public: \
            static void log(const std::string& msg) { \
                std::cout << #name << ": " << msg << std::endl; \
            } \
        }; \
    }

// Better approach: Use proper namespaces
namespace logging {

class Logger {
public:
    static void info(const std::string& component, const std::string& msg) {
        std::cout << "[" << component << "] INFO: " << msg << std::endl;
    }

    static void error(const std::string& component, const std::string& msg) {
        std::cout << "[" << component << "] ERROR: " << msg << std::endl;
    }
};

}  // namespace logging

void demonstrate_macro_namespace_gotcha() {
    std::cout << "\n=== Macro Namespace Issues ===\n";

    // Using proper namespace approach
    logging::Logger::info("TRADE_ENGINE", "Order submitted");
    logging::Logger::error("RISK_ENGINE", "Position limit exceeded");
}

// =============================================================================
// DEMONSTRATION FUNCTIONS
// =============================================================================

void demonstrate_pitfalls() {
    demonstrate_adl_ambiguity();
    demonstrate_inline_namespace_gotcha();
    demonstrate_anonymous_namespace_gotcha();
    demonstrate_lookup_order();
    demonstrate_adl_overload_gotcha();
    demonstrate_macro_namespace_gotcha();
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "Namespace Pitfalls and Gotchas\n";
    std::cout << "==============================\n";

    demonstrate_pitfalls();

    std::cout << "\n=== Critical Namespace Pitfalls to Avoid (JS/TS Edition) ===\n";
    std::cout << "1. NEVER 'using namespace' in headers (like import * in shared modules)\n";
    std::cout << "2. ADL ambiguity: Multiple modules exporting same function name\n";
    std::cout << "3. Template specializations: Must be in same 'module' as primary template\n";
    std::cout << "4. Friend functions: Define in same namespace as declaring class\n";
    std::cout << "5. Static members: Need full qualification (like module.class.property)\n";
    std::cout << "6. Inline namespaces: Can silently change behavior across versions\n";
    std::cout << "7. Anonymous namespaces: File-private, not shared between modules\n";
    std::cout << "8. Namespace lookup: Order matters (local → namespace → global)\n";
    std::cout << "9. ADL overloads: Can find wrong function if not careful\n";
    std::cout << "10. Macros: Can create unexpected namespace pollution\n";

    std::cout << "\n=== Best Practices for Large Codebases ===\n";
    std::cout << "• Fully qualified names in headers (clear dependencies)\n";
    std::cout << "• Using declarations locally (limited scope pollution)\n";
    std::cout << "• Namespace aliases for deep paths (like import aliases)\n";
    std::cout << "• Operators in same namespace as operands (ADL-friendly)\n";
    std::cout << "• Anonymous namespaces for file-local code (non-exported)\n";
    std::cout << "• Document namespace ownership and purposes\n";
    std::cout << "• Test thoroughly - namespace issues appear at link time\n";

    return 0;
}
