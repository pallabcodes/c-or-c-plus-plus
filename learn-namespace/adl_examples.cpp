/**
 * Argument Dependent Lookup (ADL) Examples - JavaScript/TypeScript Developer Edition
 *
 * ADL (Koenig Lookup) is a complex C++ feature with no direct JS/TS equivalent.
 * Think of it as "smart function resolution" based on argument types.
 *
 * In JS/TS, function calls are resolved by:
 * 1. Local scope
 * 2. Module scope
 * 3. Global scope
 *
 * In C++, ADL adds: "Namespaces of argument types"
 *
 * Why it matters: Enables operator overloading and generic programming
 * without explicit qualification. Like having "instance methods" that
 * work across namespaces.
 */

#include <iostream>
#include <vector>
#include <string>
#include <complex>
#include <algorithm>

// =============================================================================
// 1. BASIC ADL EXAMPLE
// =============================================================================
// In JS/TS: You would need explicit calls:
// const sum = math.add(math.Complex, math.Complex);
//
// In C++ with ADL: The + operator "knows" to look in bloomberg::math
// because its arguments are bloomberg::math::Complex objects

namespace bloomberg {
    namespace math {

        class Complex {
        public:
            Complex(double real = 0.0, double imag = 0.0)
                : real_(real), imag_(imag) {}

            double real() const { return real_; }
            double imag() const { return imag_; }

        private:
            double real_, imag_;
        };

        // Operator in same namespace as Complex class
        // ADL will find this when calling operator+ on Complex objects
        Complex operator+(const Complex& a, const Complex& b) {
            return Complex(a.real() + b.real(), a.imag() + b.imag());
        }

        // Output operator also in same namespace
        std::ostream& operator<<(std::ostream& os, const Complex& c) {
            os << "(" << c.real() << ", " << c.imag() << ")";
            return os;
        }

        // Function that takes Complex argument
        void print(const Complex& c) {
            std::cout << "Complex number: " << c << std::endl;
        }

    } // namespace math
} // namespace bloomberg

// =============================================================================
// 2. ADL WITH TEMPLATES
// =============================================================================

namespace bloomberg {
    namespace containers {

        template<typename T>
        class Vector {
        public:
            Vector(std::size_t size = 0) : data_(size) {}
            std::size_t size() const { return data_.size(); }
            T& operator[](std::size_t i) { return data_[i]; }
            const T& operator[](std::size_t i) const { return data_[i]; }

        private:
            std::vector<T> data_;
        };

        // Template function in same namespace as Vector
        // ADL will find this for Vector<T> arguments
        template<typename T>
        void swap(Vector<T>& a, Vector<T>& b) {
            std::cout << "Custom swap for bloomberg::containers::Vector" << std::endl;
            std::swap(a.data_, b.data_);
        }

        // Non-template function that will be preferred for exact matches
        void swap(Vector<int>& a, Vector<int>& b) {
            std::cout << "Specialized swap for Vector<int>" << std::endl;
            std::swap(a.data_, b.data_);
        }

    } // namespace containers
} // namespace containers

// =============================================================================
// 3. ADL WITH INHERITANCE
// =============================================================================

namespace bloomberg {
    namespace trading {

        class Instrument {
        public:
            virtual ~Instrument() = default;
            virtual std::string getSymbol() const = 0;
        };

        class Stock : public Instrument {
        public:
            Stock(const std::string& symbol) : symbol_(symbol) {}
            std::string getSymbol() const override { return symbol_; }

        private:
            std::string symbol_;
        };

        // Function in trading namespace - ADL will find this
        // for any Instrument* or derived class pointers
        void processInstrument(const Instrument* inst) {
            if (inst) {
                std::cout << "Processing instrument: " << inst->getSymbol() << std::endl;
            }
        }

        // Overload for Stock specifically
        void processInstrument(const Stock* stock) {
            if (stock) {
                std::cout << "Processing stock specifically: " << stock->getSymbol() << std::endl;
            }
        }

    } // namespace trading
} // namespace bloomberg

// =============================================================================
// 4. ADL AMBIGUITY EXAMPLES
// =============================================================================

namespace library_a {
    class Widget { };

    void manipulate(Widget& w) {
        std::cout << "Library A: manipulating widget" << std::endl;
    }
}

namespace library_b {
    class Widget { };

    void manipulate(library_b::Widget& w) {
        std::cout << "Library B: manipulating widget" << std::endl;
    }
}

// =============================================================================
// 5. ADL AND FRIEND FUNCTIONS
// =============================================================================

namespace bloomberg {
    namespace serialization {

        class Serializable {
        public:
            virtual ~Serializable() = default;

            // Friend declaration - ADL will find operator<< in this namespace
            friend std::ostream& operator<<(std::ostream& os, const Serializable& obj);
        };

        // Implementation of friend function in same namespace
        std::ostream& operator<<(std::ostream& os, const Serializable& obj) {
            os << "Serializable object";
            return os;
        }

        class Trade : public Serializable {
        public:
            Trade(const std::string& symbol, double price, int quantity)
                : symbol_(symbol), price_(price), quantity_(quantity) {}

            // Friend function in same namespace as Trade
            friend std::ostream& operator<<(std::ostream& os, const Trade& trade);

        private:
            std::string symbol_;
            double price_;
            int quantity_;
        };

        // Implementation in same namespace - ADL will find this
        std::ostream& operator<<(std::ostream& os, const Trade& trade) {
            os << "Trade{" << trade.symbol_ << ", $" << trade.price_
               << ", " << trade.quantity_ << " shares}";
            return os;
        }

    } // namespace serialization
} // namespace bloomberg

// =============================================================================
// 6. ADL LIMITATIONS AND GOTCHAS
// =============================================================================

namespace problematic {

    void func(int x) {
        std::cout << "problematic::func(int): " << x << std::endl;
    }

    class Problem {
    public:
        void method() {
            func(42);  // ADL finds problematic::func, not global func
        }
    };

} // namespace problematic

void func(int x) {
    std::cout << "global func(int): " << x << std::endl;
}

// =============================================================================
// 7. DEMONSTRATION FUNCTIONS
// =============================================================================

void demonstrate_basic_adl() {
    std::cout << "\n=== Basic ADL ===\n";

    bloomberg::math::Complex a(1.0, 2.0);
    bloomberg::math::Complex b(3.0, 4.0);

    // ADL finds bloomberg::math::operator+
    bloomberg::math::Complex sum = a + b;

    // ADL finds bloomberg::math::operator<<
    std::cout << "Sum: " << sum << std::endl;

    // ADL finds bloomberg::math::print
    bloomberg::math::print(sum);
}

void demonstrate_template_adl() {
    std::cout << "\n=== Template ADL ===\n";

    bloomberg::containers::Vector<int> v1(5);
    bloomberg::containers::Vector<int> v2(3);

    v1[0] = 1; v1[1] = 2; v1[2] = 3; v1[3] = 4; v1[4] = 5;
    v2[0] = 10; v2[1] = 20; v2[2] = 30;

    std::cout << "Before swap - v1[0]: " << v1[0] << ", v2[0]: " << v2[0] << std::endl;

    // ADL finds bloomberg::containers::swap (specialized for int)
    swap(v1, v2);

    std::cout << "After swap - v1[0]: " << v1[0] << ", v2[0]: " << v2[0] << std::endl;

    // Demonstrate with double (uses template version)
    bloomberg::containers::Vector<double> dv1(2);
    bloomberg::containers::Vector<double> dv2(2);
    dv1[0] = 1.5; dv2[0] = 2.5;

    swap(dv1, dv2);  // ADL finds template version
}

void demonstrate_inheritance_adl() {
    std::cout << "\n=== Inheritance ADL ===\n";

    bloomberg::trading::Stock stock("AAPL");

    // ADL finds bloomberg::trading::processInstrument(const Instrument*)
    bloomberg::trading::processInstrument(&stock);

    // Direct call with Stock* - finds the Stock overload
    bloomberg::trading::processInstrument(static_cast<bloomberg::trading::Stock*>(&stock));
}

void demonstrate_serialization_adl() {
    std::cout << "\n=== Serialization ADL ===\n";

    bloomberg::serialization::Trade trade("GOOGL", 2500.00, 100);

    // ADL finds bloomberg::serialization::operator<< for Trade
    std::cout << trade << std::endl;

    bloomberg::serialization::Serializable* serializable = &trade;
    // ADL finds bloomberg::serialization::operator<< for Serializable
    std::cout << *serializable << std::endl;
}

void demonstrate_adl_gotchas() {
    std::cout << "\n=== ADL Gotchas ===\n";

    problematic::Problem p;
    p.method();  // Calls problematic::func, not global func

    // Explicit qualification to call global func
    func(100);
}

void demonstrate_adl_ambiguity() {
    std::cout << "\n=== ADL Ambiguity (Commented Out) ===\n";

    // This would cause ambiguity if uncommented:
    // library_a::Widget wa;
    // library_b::Widget wb;
    // manipulate(wa);  // Ambiguous - which manipulate?
    // manipulate(wb);  // Also ambiguous

    std::cout << "ADL ambiguity examples are commented out to avoid compilation errors" << std::endl;
    std::cout << "The issue occurs when multiple namespaces contain functions with the same name" << std::endl;
    std::cout << "and ADL finds more than one viable candidate." << std::endl;
}

// =============================================================================
// 8. ADL BEST PRACTICES
// =============================================================================

namespace best_practices {

    // 1. Put operators in the same namespace as their operands
    class Matrix {
    public:
        Matrix(int rows, int cols) : rows_(rows), cols_(cols) {}
        int rows() const { return rows_; }
        int cols() const { return cols_; }

    private:
        int rows_, cols_;
    };

    // Operator in same namespace - good practice
    Matrix operator+(const Matrix& a, const Matrix& b) {
        return Matrix(a.rows(), a.cols());  // Simplified
    }

    // 2. Use ADL-friendly function names
    void transform(Matrix& m) {
        std::cout << "Transforming matrix in best_practices namespace" << std::endl;
    }

    // 3. Be aware of ADL in generic code
    template<typename T>
    void process(const T& obj) {
        // ADL will find transform in T's namespace if it exists
        transform(obj);  // May find T's transform via ADL
    }

} // namespace best_practices

void demonstrate_best_practices() {
    std::cout << "\n=== ADL Best Practices ===\n";

    best_practices::Matrix m1(3, 4), m2(3, 4);
    best_practices::Matrix sum = m1 + m2;  // ADL finds operator+

    best_practices::process(m1);  // ADL finds best_practices::transform
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "Argument Dependent Lookup (ADL) Examples\n";
    std::cout << "==========================================\n";

    demonstrate_basic_adl();
    demonstrate_template_adl();
    demonstrate_inheritance_adl();
    demonstrate_serialization_adl();
    demonstrate_adl_gotchas();
    demonstrate_adl_ambiguity();
    demonstrate_best_practices();

    std::cout << "\n=== ADL Key Takeaways for JS/TS Developers ===\n";
    std::cout << "1. ADL = 'Smart lookup' in namespaces of function arguments\n";
    std::cout << "2. Like operators having 'instance methods' across namespaces\n";
    std::cout << "3. Put operators/helpers in same namespace as their classes (ADL-friendly)\n";
    std::cout << "4. Can cause 'which function?' ambiguity - like multiple same-named exports\n";
    std::cout << "5. Works with inheritance: derived classes can use base namespace functions\n";
    std::cout << "6. Templates participate in ADL - generic code benefits automatically\n";
    std::cout << "7. Powerful for operator overloading, but requires careful design\n";

    return 0;
}
