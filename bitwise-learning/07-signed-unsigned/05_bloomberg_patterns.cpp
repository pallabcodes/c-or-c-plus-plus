/**
 * Bloomberg-Style Integer Handling Patterns - TypeScript Developer Edition
 *
 * Bloomberg has evolved sophisticated patterns for handling signed/unsigned integers
 * safely in large-scale financial systems. These patterns prevent bugs that could
 * cost millions in financial losses.
 *
 * Key Bloomberg principles:
 * - Type safety over performance
 * - Explicit over implicit
 * - Safe over fast (when safety matters)
 * - Domain-driven design for integer types
 */

#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <limits>
#include <cstdint>

// =============================================================================
// 1. BLOOMBERG TYPE ALIASES
// =============================================================================
// Bloomberg uses descriptive type aliases that encode domain knowledge

namespace bloomberg {
    namespace types {

        // Financial primitives
        using Price = int64_t;        // Prices in smallest currency unit (e.g., cents)
                                     // Signed: can be negative (discounts)
        using Quantity = uint64_t;    // Share quantities - always positive
        using Amount = int64_t;       // Monetary amounts - can be negative (losses)
        using Balance = int64_t;      // Account balances - can be negative
        using OrderId = uint64_t;     // Order IDs - monotonically increasing
        using Timestamp = uint64_t;   // Unix timestamps in milliseconds
        using Sequence = uint64_t;    // Sequence numbers - always increasing

        // Business constraints
        using Age = uint8_t;          // Ages 0-255 years (reasonable range)
        using Rating = uint8_t;       // Credit ratings 0-255 (AAA=0, D=255)
        using Priority = uint8_t;     // Task priorities 0-255
        using Percentage = uint8_t;   // Percentages 0-100 (but allow 0-255)

        // System types
        using ErrorCode = int32_t;    // Error codes (negative = error, 0 = success)
        using Size = size_t;          // Container sizes (unsigned, platform-dependent)
        using Index = size_t;         // Array indices (unsigned, platform-dependent)
        using Offset = int64_t;       // Offsets can be negative (time offsets, etc.)

        void demonstrate_bloomberg_type_aliases() {
            std::cout << "\n=== Bloomberg Type Aliases ===\n";

            Price stock_price = 15025;      // $150.25
            Quantity shares = 1000;         // 1000 shares
            Amount profit = -50000;         // -$500.00 loss
            OrderId order_id = 1234567890123ULL; // Large order ID
            Timestamp timestamp = 1703123456789ULL; // Unix timestamp

            std::cout << "Stock price: $" << stock_price / 100.0 << std::endl;
            std::cout << "Shares: " << shares << std::endl;
            std::cout << "Profit/Loss: $" << profit / 100.0 << std::endl;
            std::cout << "Order ID: " << order_id << std::endl;
            std::cout << "Timestamp: " << timestamp << std::endl;

            // Type safety: Compiler prevents mixing incompatible types
            // profit = shares;  // Compilation error: uint64_t vs int64_t
        }

    } // namespace types
} // namespace bloomberg

// =============================================================================
// 2. SAFE ARITHMETIC CLASSES
// =============================================================================
// Bloomberg's SafeMath classes prevent overflow in critical calculations

namespace bloomberg {
    namespace safe_math {

        // Base safe arithmetic template
        template<typename T>
        class SafeArithmetic {
        public:
            static std::optional<T> add(T a, T b) {
                if constexpr (std::is_unsigned_v<T>) {
                    T result = a + b;
                    if (result < a) return std::nullopt; // Overflow
                    return result;
                } else {
                    if (b > 0 && a > std::numeric_limits<T>::max() - b) return std::nullopt;
                    if (b < 0 && a < std::numeric_limits<T>::min() - b) return std::nullopt;
                    return a + b;
                }
            }

            static std::optional<T> subtract(T a, T b) {
                if constexpr (std::is_unsigned_v<T>) {
                    if (b > a) return std::nullopt; // Underflow
                    return a - b;
                } else {
                    if (b < 0 && a > std::numeric_limits<T>::max() + b) return std::nullopt;
                    if (b > 0 && a < std::numeric_limits<T>::min() + b) return std::nullopt;
                    return a - b;
                }
            }

            static std::optional<T> multiply(T a, T b) {
                if (a == 0 || b == 0) return T{0};
                if constexpr (std::is_unsigned_v<T>) {
                    if (a > std::numeric_limits<T>::max() / b) return std::nullopt;
                    return a * b;
                } else {
                    // Simplified signed check
                    T abs_a = a < 0 ? -a : a;
                    T abs_b = b < 0 ? -b : b;
                    if (abs_a > std::numeric_limits<T>::max() / abs_b) return std::nullopt;
                    return a * b;
                }
            }
        };

        void demonstrate_safe_arithmetic() {
            std::cout << "\n=== Bloomberg Safe Arithmetic ===\n";

            using namespace bloomberg::types;

            // Safe financial calculations
            Price price1 = 50000;   // $500.00
            Price price2 = 75000;   // $750.00

            auto total = SafeArithmetic<Price>::add(price1, price2);
            if (total) {
                std::cout << "Total price: $" << *total / 100.0 << std::endl;
            } else {
                std::cout << "Price calculation overflow!" << std::endl;
            }

            // Safe quantity operations
            Quantity qty1 = 1000;
            Quantity qty2 = 500;

            auto sum_qty = SafeArithmetic<Quantity>::add(qty1, qty2);
            if (sum_qty) {
                std::cout << "Total quantity: " << *sum_qty << std::endl;
            }
        }

    } // namespace safe_math
} // namespace bloomberg

// =============================================================================
// 3. DOMAIN-SPECIFIC WRAPPER CLASSES
// =============================================================================
// Bloomberg wraps integers in classes that enforce business rules

namespace bloomberg {
    namespace domain {

        // Price wrapper - ensures valid price ranges
        class Price {
            types::Price value_;  // cents
        public:
            explicit Price(types::Price cents) : value_(cents) {
                // Validate price range (example: $0.00 to $1,000,000.00)
                if (cents < 0 || cents > 1000000000LL) {
                    throw std::invalid_argument("Invalid price range");
                }
            }

            types::Price cents() const { return value_; }
            double dollars() const { return value_ / 100.0; }

            Price operator+(const Price& other) const {
                auto result = safe_math::SafeArithmetic<types::Price>::add(value_, other.value_);
                if (!result) throw std::overflow_error("Price addition overflow");
                return Price(*result);
            }
        };

        // Quantity wrapper - ensures non-negative quantities
        class Quantity {
            types::Quantity value_;
        public:
            explicit Quantity(types::Quantity qty) : value_(qty) {
                if (qty == 0) throw std::invalid_argument("Quantity must be positive");
            }

            types::Quantity value() const { return value_; }

            Quantity operator+(const Quantity& other) const {
                auto result = safe_math::SafeArithmetic<types::Quantity>::add(value_, other.value_);
                if (!result) throw std::overflow_error("Quantity addition overflow");
                return Quantity(*result);
            }

            Quantity operator*(const Price& price) const {
                auto result = safe_math::SafeArithmetic<types::Amount>::multiply(
                    static_cast<types::Amount>(value_), price.cents());
                if (!result) throw std::overflow_error("Quantity * Price overflow");
                return Quantity(static_cast<types::Quantity>(*result));
            }
        };

        // Order ID wrapper - ensures valid order IDs
        class OrderId {
            types::OrderId value_;
        public:
            explicit OrderId(types::OrderId id) : value_(id) {
                if (id == 0) throw std::invalid_argument("Order ID must be positive");
            }

            types::OrderId value() const { return value_; }

            bool operator<(const OrderId& other) const {
                return value_ < other.value_;
            }
        };

        void demonstrate_domain_wrappers() {
            std::cout << "\n=== Bloomberg Domain Wrappers ===\n";

            Price apple_price(15025);    // $150.25
            Quantity shares(100);        // 100 shares
            OrderId order1(12345);
            OrderId order2(67890);

            std::cout << "Apple price: $" << apple_price.dollars() << std::endl;
            std::cout << "Shares: " << shares.value() << std::endl;
            std::cout << "Order1 < Order2: " << (order1 < order2 ? "true" : "false") << std::endl;

            // Safe operations
            Price total_price = apple_price + apple_price;
            std::cout << "Total price: $" << total_price.dollars() << std::endl;

            // Operations that maintain type safety
            auto total_value = shares * apple_price;  // This would be Amount, not Quantity
            std::cout << "Total value: $" << total_value / 100.0 << std::endl;
        }

    } // namespace domain
} // namespace bloomberg

// =============================================================================
// 4. RANGE-CHECKED CONVERSIONS
// =============================================================================
// Bloomberg's approach to safe type conversions

namespace bloomberg {
    namespace conversions {

        // Safe conversion with range checking
        template<typename To, typename From>
        std::optional<To> safe_convert(From value) {
            if constexpr (std::is_same_v<To, From>) {
                return value;  // Same type, no conversion needed
            }

            // Check if value is within target range
            if (value > std::numeric_limits<To>::max() ||
                value < std::numeric_limits<To>::min()) {
                return std::nullopt;
            }

            return static_cast<To>(value);
        }

        // Domain-specific conversions
        std::optional<types::Quantity> to_quantity(types::Amount amount) {
            if (amount < 0) return std::nullopt;
            return safe_convert<types::Quantity>(amount);
        }

        std::optional<types::Price> to_price(types::Amount amount) {
            // Prices can be negative (discounts), but check reasonable bounds
            if (amount < -100000000LL || amount > 1000000000LL) return std::nullopt;
            return safe_convert<types::Price>(amount);
        }

        void demonstrate_safe_conversions() {
            std::cout << "\n=== Bloomberg Safe Conversions ===\n";

            // Safe quantity conversion
            types::Amount raw_qty = 100;
            auto safe_qty = to_quantity(raw_qty);
            if (safe_qty) {
                std::cout << "Converted to quantity: " << *safe_qty << std::endl;
            }

            // Reject negative quantities
            types::Amount negative_qty = -50;
            auto invalid_qty = to_quantity(negative_qty);
            if (!invalid_qty) {
                std::cout << "Rejected negative quantity: " << negative_qty << std::endl;
            }

            // Safe price conversion
            types::Amount raw_price = 15025;
            auto safe_price = to_price(raw_price);
            if (safe_price) {
                std::cout << "Converted to price: $" << *safe_price / 100.0 << std::endl;
            }
        }

    } // namespace conversions
} // namespace bloomberg

// =============================================================================
// 5. BLOOMBERG ERROR HANDLING PATTERNS
// =============================================================================
// How Bloomberg handles integer-related errors

namespace bloomberg {
    namespace error {

        // Bloomberg error codes (negative = error)
        enum ErrorCode {
            SUCCESS = 0,
            OVERFLOW_ERROR = -1,
            UNDERFLOW_ERROR = -2,
            INVALID_RANGE = -3,
            TYPE_MISMATCH = -4
        };

        // Error result class
        template<typename T>
        class Result {
            std::optional<T> value_;
            ErrorCode error_;
        public:
            Result(T val) : value_(val), error_(SUCCESS) {}
            Result(ErrorCode err) : error_(err) {}

            bool success() const { return error_ == SUCCESS; }
            const T& value() const { return *value_; }
            ErrorCode error() const { return error_; }
        };

        // Safe arithmetic with error codes
        class CheckedMath {
        public:
            static Result<types::Amount> add_amounts(types::Amount a, types::Amount b) {
                auto result = safe_math::SafeArithmetic<types::Amount>::add(a, b);
                if (!result) return Result<types::Amount>(OVERFLOW_ERROR);
                return Result<types::Amount>(*result);
            }

            static Result<types::Quantity> add_quantities(types::Quantity a, types::Quantity b) {
                auto result = safe_math::SafeArithmetic<types::Quantity>::add(a, b);
                if (!result) return Result<types::Quantity>(OVERFLOW_ERROR);
                return Result<types::Quantity>(*result);
            }
        };

        void demonstrate_error_handling() {
            std::cout << "\n=== Bloomberg Error Handling ===\n";

            types::Amount balance1 = 9000000000000000000LL;  // Very large
            types::Amount balance2 = 1000000000000000000LL;  // Also large

            auto result = CheckedMath::add_amounts(balance1, balance2);
            if (result.success()) {
                std::cout << "Addition successful: " << result.value() << std::endl;
            } else {
                std::cout << "Addition failed with error code: " << result.error() << std::endl;
            }
        }

    } // namespace error
} // namespace bloomberg

// =============================================================================
// 6. PERFORMANCE-CRITICAL PATTERNS
// =============================================================================
// When Bloomberg needs performance over safety

namespace bloomberg {
    namespace performance {

        // Fast operations for hot paths (use with caution!)
        class FastMath {
        public:
            // Assume no overflow for performance
            static types::Amount fast_add(types::Amount a, types::Amount b) {
                return a + b;  // No overflow check
            }

            // Use wider types for intermediate calculations
            static types::Amount safe_multiply(types::Quantity a, types::Price b) {
                // Use wider intermediate type to prevent overflow
                using WideType = __int128;  // GCC extension for very wide integers
                WideType wide_result = static_cast<WideType>(a) * static_cast<WideType>(b);
                return static_cast<types::Amount>(wide_result);
            }
        };

        void demonstrate_performance_patterns() {
            std::cout << "\n=== Bloomberg Performance Patterns ===\n";

            types::Quantity qty = 1000000;
            types::Price price = 15025;  // $150.25

            // Safe multiplication using wider intermediate type
            types::Amount total = FastMath::safe_multiply(qty, price);
            std::cout << "Total value (safe): $" << total / 100.0 << std::endl;

            // Expected: 1,000,000 * $150.25 = $150,250,000
            std::cout << "Expected: $" << (1000000LL * 15025LL) / 100.0 << std::endl;
        }

    } // namespace performance
} // namespace bloomberg

// =============================================================================
// 7. TESTING PATTERNS
// =============================================================================
// How Bloomberg tests integer operations

namespace bloomberg {
    namespace testing {

        // Test helper for boundary conditions
        class IntegerTestHelper {
        public:
            static void test_boundaries() {
                std::cout << "\n=== Boundary Testing ===\n";

                // Test Price boundaries
                types::Price min_price = 0;           // $0.00
                types::Price max_price = 1000000000LL; // $10,000,000.00

                std::cout << "Min price: $" << min_price / 100.0 << std::endl;
                std::cout << "Max price: $" << max_price / 100.0 << std::endl;

                // Test overflow conditions
                auto overflow_result = safe_math::SafeArithmetic<types::Price>::add(
                    max_price, 1);
                if (!overflow_result) {
                    std::cout << "Correctly detected price overflow" << std::endl;
                }
            }

            static void test_mixed_operations() {
                std::cout << "\n=== Mixed Operations Testing ===\n";

                types::Quantity qty = 100;
                types::Price price = 15025;  // $150.25

                // Test various combinations
                auto total1 = safe_math::SafeArithmetic<types::Amount>::multiply(
                    static_cast<types::Amount>(qty), price);
                auto total2 = safe_math::SafeArithmetic<types::Amount>::multiply(
                    price, static_cast<types::Amount>(qty));

                if (total1 && total2 && *total1 == *total2) {
                    std::cout << "Commutative property holds: $" << *total1 / 100.0 << std::endl;
                }
            }
        };

        void demonstrate_testing_patterns() {
            IntegerTestHelper::test_boundaries();
            IntegerTestHelper::test_mixed_operations();
        }

    } // namespace testing
} // namespace bloomberg

// =============================================================================
// 8. TYPESCRIPT EQUIVALENTS
// =============================================================================
// How these Bloomberg patterns translate to TypeScript

void demonstrate_typescript_equivalents() {
    std::cout << "\n=== TypeScript Equivalents ===\n";

    std::cout << "// Bloomberg TypeScript equivalents:" << std::endl;
    std::cout << "type Price = bigint;        // Use BigInt for precision" << std::endl;
    std::cout << "type Quantity = bigint;     // Always positive" << std::endl;
    std::cout << "type Amount = bigint;       // Can be negative" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "// Safe arithmetic:" << std::endl;
    std::cout << "class SafeMath {" << std::endl;
    std::cout << "  static add(a: bigint, b: bigint): bigint {" << std::endl;
    std::cout << "    // BigInt handles arbitrary precision" << std::endl;
    std::cout << "    return a + b;" << std::endl;
    std::cout << "  }" << std::endl;
    std::cout << "}" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "// Domain wrappers:" << std::endl;
    std::cout << "class Price {" << std::endl;
    std::cout << "  constructor(private cents: bigint) {" << std::endl;
    std::cout << "    if (cents < 0n || cents > 1000000000n) {" << std::endl;
    std::cout << "      throw new Error('Invalid price');" << std::endl;
    std::cout << "    }" << std::endl;
    std::cout << "  }" << std::endl;
    std::cout << "}" << std::endl;
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "Bloomberg-Style Integer Handling Patterns - TypeScript Developer Edition\n";
    std::cout << "=========================================================================\n";

    bloomberg::types::demonstrate_bloomberg_type_aliases();
    bloomberg::safe_math::demonstrate_safe_arithmetic();
    bloomberg::domain::demonstrate_domain_wrappers();
    bloomberg::conversions::demonstrate_safe_conversions();
    bloomberg::error::demonstrate_error_handling();
    bloomberg::performance::demonstrate_performance_patterns();
    bloomberg::testing::demonstrate_testing_patterns();
    demonstrate_typescript_equivalents();

    std::cout << "\n=== Bloomberg Patterns Takeaways ===\n";
    std::cout << "1. Type aliases: Price, Quantity, Amount with domain semantics\n";
    std::cout << "2. Safe arithmetic: Always check for overflow in financial calc\n";
    std::cout << "3. Domain wrappers: Classes that enforce business rules\n";
    std::cout << "4. Range checking: Validate inputs at construction/conversion\n";
    std::cout << "5. Error handling: Result classes with error codes\n";
    std::cout << "6. Performance: Use wider types for intermediate calculations\n";
    std::cout << "7. Testing: Boundary testing for all integer operations\n";
    std::cout << "8. Fixed-width types: uint64_t, int64_t over int, long\n";
    std::cout << "9. Explicit conversions: Never rely on implicit conversions\n";
    std::cout << "10. Documentation: Clear comments about signedness and ranges\n";

    return 0;
}
