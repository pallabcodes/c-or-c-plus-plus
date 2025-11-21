/**
 * Overflow, Underflow, and Wraparound - TypeScript Developer Edition
 *
 * In C++, integer overflow has well-defined (but dangerous) behavior.
 * In JavaScript/TypeScript, numbers never overflow - they just lose precision.
 *
 * Key differences:
 * - C++ signed overflow: Undefined behavior (can crash/do weird things)
 * - C++ unsigned overflow: Defined wraparound (predictable)
 * - TypeScript: No overflow, but precision loss beyond 2^53
 */

#include <iostream>
#include <limits>
#include <cstdint>
#include <string>

// =============================================================================
// 1. SIGNED INTEGER OVERFLOW (UNDEFINED BEHAVIOR)
// =============================================================================
// This is DANGEROUS and can cause crashes or unpredictable results

void demonstrate_signed_overflow() {
    std::cout << "\n=== Signed Integer Overflow (UNDEFINED BEHAVIOR) ===\n";

    int32_t max_int32 = std::numeric_limits<int32_t>::max();
    std::cout << "Max int32_t: " << max_int32 << std::endl;

    // This is UNDEFINED BEHAVIOR in C++!
    // The compiler might optimize this away or do strange things
    // int32_t overflow_result = max_int32 + 1;
    // std::cout << "After overflow: " << overflow_result << std::endl;

    std::cout << "Adding 1 to max int32_t: UNDEFINED BEHAVIOR!" << std::endl;
    std::cout << "This can crash, give wrong results, or be optimized away." << std::endl;
    std::cout << "NEVER rely on signed integer overflow!" << std::endl;

    // What might happen (implementation defined):
    std::cout << "Possible outcomes:" << std::endl;
    std::cout << "1. Program crashes" << std::endl;
    std::cout << "2. Wrong result (wraparound)" << std::endl;
    std::cout << "3. Compiler optimizes it away" << std::endl;
    std::cout << "4. Nasal demons emerge from your nose" << std::endl;

    // In TypeScript: No such danger
    // const max = Number.MAX_SAFE_INTEGER;
    // const overflow = max + 1;
    // console.log(overflow);  // Still works, just loses precision
}

// =============================================================================
// 2. UNSIGNED INTEGER OVERFLOW (DEFINED WRAPAROUND)
// =============================================================================
// This is SAFE and PREDICTABLE

void demonstrate_unsigned_overflow() {
    std::cout << "\n=== Unsigned Integer Overflow (DEFINED WRAPAROUND) ===\n";

    uint8_t max_uint8 = std::numeric_limits<uint8_t>::max();
    std::cout << "Max uint8_t: " << static_cast<unsigned>(max_uint8) << std::endl;

    uint8_t overflow_result = max_uint8 + 1;
    std::cout << "max_uint8 + 1 = " << static_cast<unsigned>(overflow_result) << std::endl;
    std::cout << "Wraparound to 0: PREDICTABLE and SAFE!" << std::endl;

    // Show the wraparound pattern
    std::cout << "\nWraparound demonstration:" << std::endl;
    uint8_t value = 250;
    for (int i = 0; i < 10; ++i) {
        std::cout << "value = " << static_cast<unsigned>(value) << std::endl;
        value += 10;  // Will wrap around
    }

    // This is useful for:
    // - Hash functions
    // - Counters that wrap
    // - Modular arithmetic
    // - Some cryptographic operations

    // In TypeScript: Need BigInt for predictable wraparound
    // const max = 255n;
    // const overflow = max + 1n;  // 256n (no wraparound)
    // For wraparound: (max + 1n) & 255n;  // Manual wraparound
}

// =============================================================================
// 3. DETECTING OVERFLOW BEFORE IT HAPPENS
// =============================================================================
// Safe arithmetic functions

template<typename T>
bool would_add_overflow(T a, T b) {
    if constexpr (std::is_unsigned_v<T>) {
        // For unsigned, check if result would be less than one operand
        return a + b < a;
    } else {
        // For signed, more complex checks
        if (b > 0 && a > std::numeric_limits<T>::max() - b) return true;
        if (b < 0 && a < std::numeric_limits<T>::min() - b) return true;
        return false;
    }
}

template<typename T>
std::optional<T> safe_add(T a, T b) {
    if (would_add_overflow(a, b)) {
        return std::nullopt;  // Overflow would occur
    }
    return a + b;
}

void demonstrate_overflow_detection() {
    std::cout << "\n=== Overflow Detection ===\n";

    // Signed overflow detection
    int32_t a = std::numeric_limits<int32_t>::max();
    int32_t b = 1;

    std::cout << "Would " << a << " + " << b << " overflow? "
              << (would_add_overflow(a, b) ? "YES" : "NO") << std::endl;

    auto result = safe_add(a, b);
    if (result) {
        std::cout << "Safe result: " << *result << std::endl;
    } else {
        std::cout << "Overflow prevented!" << std::endl;
    }

    // Unsigned overflow detection
    uint8_t x = 250;
    uint8_t y = 10;

    std::cout << "Would " << static_cast<unsigned>(x) << " + "
              << static_cast<unsigned>(y) << " overflow? "
              << (would_add_overflow(x, y) ? "YES" : "NO") << std::endl;

    auto u_result = safe_add(x, y);
    if (u_result) {
        std::cout << "Unsigned result: " << static_cast<unsigned>(*u_result) << std::endl;
    } else {
        std::cout << "Unsigned overflow prevented!" << std::endl;
    }

    // In TypeScript: No overflow, so no need for detection
    // const result = a + b;  // Always works
}

// =============================================================================
// 4. UNDERFLOW (NEGATIVE OVERFLOW)
// =============================================================================
// What happens when you go below the minimum value

void demonstrate_underflow() {
    std::cout << "\n=== Underflow (Negative Overflow) ===\n";

    // Signed underflow (undefined behavior)
    int32_t min_int32 = std::numeric_limits<int32_t>::min();
    std::cout << "Min int32_t: " << min_int32 << std::endl;

    // int32_t underflow_result = min_int32 - 1;  // UNDEFINED BEHAVIOR!
    std::cout << "Subtracting 1 from min int32_t: UNDEFINED BEHAVIOR!" << std::endl;

    // Unsigned underflow (defined wraparound to max)
    uint8_t min_uint8 = std::numeric_limits<uint8_t>::min();  // 0
    std::cout << "Min uint8_t: " << static_cast<unsigned>(min_uint8) << std::endl;

    uint8_t underflow_result = min_uint8 - 1;
    std::cout << "uint8_t(0) - 1 = " << static_cast<unsigned>(underflow_result)
              << " (wraps to 255)" << std::endl;

    // Show unsigned underflow pattern
    std::cout << "\nUnsigned underflow demonstration:" << std::endl;
    uint8_t value = 5;
    for (int i = 0; i < 10; ++i) {
        std::cout << "value = " << static_cast<unsigned>(value) << std::endl;
        value -= 10;  // Will wrap around
    }

    // In TypeScript: No underflow, just negative numbers
    // const min = Number.MIN_SAFE_INTEGER;
    // const underflow = min - 1;  // Still works, just more negative
}

// =============================================================================
// 5. MULTIPLICATION OVERFLOW
// =============================================================================
// Harder to detect than addition overflow

template<typename T>
bool would_multiply_overflow(T a, T b) {
    if (a == 0 || b == 0) return false;

    if constexpr (std::is_unsigned_v<T>) {
        return a > std::numeric_limits<T>::max() / b;
    } else {
        // Signed multiplication overflow is complex
        // Simplified check (not perfect)
        T max_val = std::numeric_limits<T>::max();
        T min_val = std::numeric_limits<T>::min();

        if ((a > 0 && b > 0 && a > max_val / b) ||
            (a < 0 && b < 0 && a < max_val / b) ||
            (a > 0 && b < 0 && b < min_val / a) ||
            (a < 0 && b > 0 && a < min_val / b)) {
            return true;
        }
        return false;
    }
}

void demonstrate_multiplication_overflow() {
    std::cout << "\n=== Multiplication Overflow ===\n";

    // Unsigned multiplication overflow
    uint16_t a = 50000;
    uint16_t b = 2;

    std::cout << "Would " << a << " * " << b << " overflow uint16_t? "
              << (would_multiply_overflow(a, b) ? "YES" : "NO") << std::endl;

    uint16_t result = a * b;  // This WILL overflow
    std::cout << a << " * " << b << " = " << result << " (OVERFLOW!)" << std::endl;

    // Safe multiplication
    uint32_t safe_result = static_cast<uint32_t>(a) * static_cast<uint32_t>(b);
    std::cout << "Safe multiplication: " << safe_result << std::endl;

    // In TypeScript: No overflow, just precision loss
    // const result = 50000 * 2;  // 100000 (exact)
    // const bigResult = 50000n * 2n;  // 100000n (exact with BigInt)
}

// =============================================================================
// 6. PRACTICAL OVERFLOW SCENARIOS
// =============================================================================
// Real-world cases where overflow matters

void demonstrate_practical_overflow() {
    std::cout << "\n=== Practical Overflow Scenarios ===\n";

    // Scenario 1: Array indexing
    std::cout << "Array indexing:" << std::endl;
    const size_t array_size = 100;
    size_t index = 50;
    size_t offset = 60;

    // This could overflow if not careful
    size_t new_index = index + offset;
    if (new_index >= array_size) {
        std::cout << "Index " << new_index << " is out of bounds (>= " << array_size << ")" << std::endl;
    } else {
        std::cout << "New index: " << new_index << std::endl;
    }

    // Scenario 2: Financial calculations
    std::cout << "\nFinancial calculations:" << std::endl;
    int64_t account_balance = 9000000000000000000LL;  // 9e18 cents ($9e16)
    int64_t transaction = 2000000000000000000LL;     // 2e18 cents ($2e16)

    auto new_balance = safe_add(account_balance, transaction);
    if (new_balance) {
        std::cout << "Transaction successful. New balance: $" << *new_balance / 100.0 << std::endl;
    } else {
        std::cout << "Transaction failed: Would overflow!" << std::endl;
    }

    // Scenario 3: Time calculations
    std::cout << "\nTime calculations:" << std::endl;
    uint32_t seconds_since_epoch = 4000000000U;  // Year 2096
    uint32_t seconds_to_add = 100000000U;        // ~3 years

    uint32_t new_time = seconds_since_epoch + seconds_to_add;
    std::cout << "Time overflow: " << seconds_since_epoch << " + " << seconds_to_add
              << " = " << new_time << std::endl;
    std::cout << "This wraps around to year 2033!" << std::endl;

    // In TypeScript: All these calculations "just work"
    // const newIndex = 50 + 60;        // 110
    // const newBalance = 9e18 + 2e18;  // 1.1e19
    // const newTime = 4e9 + 1e8;       // 4.1e9
}

// =============================================================================
// 7. BLOOMBERG-SAFE ARITHMETIC
// =============================================================================
// Bloomberg's approach to safe integer arithmetic

namespace bloomberg {
    namespace safe_math {

        // Bloomberg-style safe addition
        template<typename T>
        class SafeArithmetic {
        public:
            static std::optional<T> add(T a, T b) {
                if constexpr (std::is_unsigned_v<T>) {
                    T result = a + b;
                    if (result < a) return std::nullopt;  // Overflow occurred
                    return result;
                } else {
                    // Signed overflow detection
                    if (b > 0 && a > std::numeric_limits<T>::max() - b) return std::nullopt;
                    if (b < 0 && a < std::numeric_limits<T>::min() - b) return std::nullopt;
                    return a + b;
                }
            }

            static std::optional<T> multiply(T a, T b) {
                if (a == 0 || b == 0) return T{0};

                if constexpr (std::is_unsigned_v<T>) {
                    if (a > std::numeric_limits<T>::max() / b) return std::nullopt;
                    return a * b;
                } else {
                    // Simplified signed check
                    T max_val = std::numeric_limits<T>::max();
                    if (std::abs(a) > max_val / std::abs(b)) return std::nullopt;
                    return a * b;
                }
            }
        };

        void demonstrate_bloomberg_safe_math() {
            std::cout << "\n=== Bloomberg Safe Arithmetic ===\n";

            // Safe addition
            int64_t balance = 9000000000000000000LL;  // Large balance
            int64_t deposit = 2000000000000000000LL;   // Large deposit

            auto new_balance = SafeArithmetic<int64_t>::add(balance, deposit);
            if (new_balance) {
                std::cout << "Deposit successful. New balance: $" << *new_balance / 100.0 << std::endl;
            } else {
                std::cout << "Deposit failed: Arithmetic overflow detected!" << std::endl;
            }

            // Safe multiplication
            uint32_t quantity = 100000;
            uint32_t price = 50000;  // In cents

            auto total = SafeArithmetic<uint32_t>::multiply(quantity, price);
            if (total) {
                std::cout << "Total value: $" << *total / 100.0 << std::endl;
            } else {
                std::cout << "Calculation failed: Arithmetic overflow detected!" << std::endl;
            }
        }

    } // namespace safe_math
} // namespace bloomberg

// =============================================================================
// 8. TYPESCRIPT COMPARISON
// =============================================================================
// How TypeScript handles (or doesn't handle) overflow

void demonstrate_typescript_comparison() {
    std::cout << "\n=== TypeScript Overflow Comparison ===\n";

    std::cout << "C++ int32_t max: " << std::numeric_limits<int32_t>::max() << std::endl;
    std::cout << "JavaScript Number.MAX_SAFE_INTEGER: 9007199254740991" << std::endl;
    std::cout << "C++ uint64_t max: " << std::numeric_limits<uint64_t>::max() << std::endl;
    std::cout << "JavaScript BigInt can handle arbitrarily large integers" << std::endl;

    std::cout << "\nIn TypeScript:" << std::endl;
    std::cout << "- Numbers are 64-bit IEEE 754 floats" << std::endl;
    std::cout << "- Safe integer range: -2^53 to +2^53" << std::endl;
    std::cout << "- Beyond that: precision loss, not overflow" << std::endl;
    std::cout << "- BigInt: Arbitrary precision, no overflow" << std::endl;
    std::cout << "- No need for overflow detection" << std::endl;
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "Overflow, Underflow, and Wraparound - TypeScript Developer Edition\n";
    std::cout << "=================================================================\n";

    demonstrate_signed_overflow();
    demonstrate_unsigned_overflow();
    demonstrate_overflow_detection();
    demonstrate_underflow();
    demonstrate_multiplication_overflow();
    demonstrate_practical_overflow();
    bloomberg::safe_math::demonstrate_bloomberg_safe_math();
    demonstrate_typescript_comparison();

    std::cout << "\n=== Overflow/Underflow Takeaways for TypeScript Devs ===\n";
    std::cout << "1. Signed overflow = UNDEFINED BEHAVIOR (dangerous, avoid!)\n";
    std::cout << "2. Unsigned overflow = PREDICTABLE wraparound (safe, useful)\n";
    std::cout << "3. C++ overflow detection required before operations\n";
    std::cout << "4. TypeScript: No overflow, but precision loss beyond 2^53\n";
    std::cout << "5. Bloomberg uses safe arithmetic classes for financial calc\n";
    std::cout << "6. Always check for overflow in critical calculations\n";
    std::cout << "7. Use wider types for intermediate calculations\n";
    std::cout << "8. Consider domain constraints (quantities can't be negative)\n";
    std::cout << "9. BigInt in TypeScript = arbitrary precision (like wider C++ types)\n";
    std::cout << "10. C++ requires explicit overflow handling; TypeScript hides it\n";

    return 0;
}
