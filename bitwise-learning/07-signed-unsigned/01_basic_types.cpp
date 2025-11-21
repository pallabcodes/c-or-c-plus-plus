/**
 * Basic Signed/Unsigned Integer Types - TypeScript Developer Edition
 *
 * This file demonstrates the fundamental differences between signed and unsigned integers.
 * In JavaScript/TypeScript, all numbers are double-precision floating point (64-bit IEEE 754).
 * C++ gives you precise control over integer representation and range.
 *
 * Key concepts:
 * - Signed integers: Can be negative, zero, or positive
 * - Unsigned integers: Can only be zero or positive
 * - Binary representation differences
 * - Range limitations and wraparound behavior
 */

#include <iostream>
#include <iomanip>
#include <limits>
#include <cstdint>
#include <string>
#include <bit>

// =============================================================================
// 1. BASIC TYPE DECLARATIONS
// =============================================================================
// In JS/TS: let x: number = 42;
// In C++: You choose the exact bit width and signedness

void demonstrate_basic_types() {
    std::cout << "\n=== Basic Type Declarations ===\n";

    // Signed integers (can be negative)
    int8_t  signed_8bit = -42;   // -128 to +127
    int16_t signed_16bit = -1000; // -32,768 to +32,767
    int32_t signed_32bit = -1000000; // -2.1B to +2.1B
    int64_t signed_64bit = -1000000000000LL; // -9.2E to +9.2E

    // Unsigned integers (cannot be negative)
    uint8_t  unsigned_8bit = 42;   // 0 to 255
    uint16_t unsigned_16bit = 1000; // 0 to 65,535
    uint32_t unsigned_32bit = 1000000; // 0 to 4.2B
    uint64_t unsigned_64bit = 1000000000000ULL; // 0 to 18.4E

    std::cout << "Signed 8-bit:  " << static_cast<int>(signed_8bit) << std::endl;
    std::cout << "Unsigned 8-bit: " << static_cast<unsigned>(unsigned_8bit) << std::endl;
    std::cout << "Signed 64-bit:  " << signed_64bit << std::endl;
    std::cout << "Unsigned 64-bit: " << unsigned_64bit << std::endl;

    // In TypeScript, you'd write:
    // let signed8: number = -42;    // But it's still a float!
    // let unsigned8: number = 42;   // Same type, no unsigned constraint
    // TypeScript can't enforce integer ranges at compile time
}

// =============================================================================
// 2. RANGE COMPARISON
// =============================================================================
// Show the actual min/max values for each type

void demonstrate_ranges() {
    std::cout << "\n=== Type Ranges ===\n";
    std::cout << std::left << std::setw(12) << "Type"
              << std::setw(20) << "Minimum"
              << std::setw(20) << "Maximum"
              << std::setw(15) << "Bits" << std::endl;
    std::cout << std::string(67, '-') << std::endl;

    // int8_t / uint8_t
    std::cout << std::left << std::setw(12) << "int8_t"
              << std::setw(20) << static_cast<int>(std::numeric_limits<int8_t>::min())
              << std::setw(20) << static_cast<int>(std::numeric_limits<int8_t>::max())
              << std::setw(15) << "8" << std::endl;

    std::cout << std::left << std::setw(12) << "uint8_t"
              << std::setw(20) << static_cast<unsigned>(std::numeric_limits<uint8_t>::min())
              << std::setw(20) << static_cast<unsigned>(std::numeric_limits<uint8_t>::max())
              << std::setw(15) << "8" << std::endl;

    // int32_t / uint32_t
    std::cout << std::left << std::setw(12) << "int32_t"
              << std::setw(20) << std::numeric_limits<int32_t>::min()
              << std::setw(20) << std::numeric_limits<int32_t>::max()
              << std::setw(15) << "32" << std::endl;

    std::cout << std::left << std::setw(12) << "uint32_t"
              << std::setw(20) << std::numeric_limits<uint32_t>::min()
              << std::setw(20) << std::numeric_limits<uint32_t>::max()
              << std::setw(15) << "32" << std::endl;

    // int64_t / uint64_t
    std::cout << std::left << std::setw(12) << "int64_t"
              << std::setw(20) << std::numeric_limits<int64_t>::min()
              << std::setw(20) << std::numeric_limits<int64_t>::max()
              << std::setw(15) << "64" << std::endl;

    std::cout << std::left << std::setw(12) << "uint64_t"
              << std::setw(20) << std::numeric_limits<uint64_t>::min()
              << std::setw(20) << std::numeric_limits<uint64_t>::max()
              << std::setw(15) << "64" << std::endl;

    // TypeScript equivalent (conceptual):
    // const INT8_MIN = -128, INT8_MAX = 127;
    // const UINT8_MAX = 255;
    // But TypeScript can't enforce these at compile time!
    // JavaScript numbers are 64-bit floats with ~53 bits of integer precision
}

// =============================================================================
// 3. BINARY REPRESENTATION
// =============================================================================
// Show how the same bit pattern represents different values

void demonstrate_binary_representation() {
    std::cout << "\n=== Binary Representation ===\n";

    // Same bit pattern: 11111111 (255 in unsigned, -1 in signed)
    uint8_t unsigned_val = 255;
    int8_t signed_val = -1;

    std::cout << "uint8_t value: " << static_cast<unsigned>(unsigned_val)
              << " (binary: " << std::bitset<8>(unsigned_val) << ")" << std::endl;

    std::cout << "int8_t value:  " << static_cast<int>(signed_val)
              << " (binary: " << std::bitset<8>(static_cast<uint8_t>(signed_val)) << ")" << std::endl;

    // Same bit pattern represents different values!
    // This is the key difference between signed and unsigned

    // In TypeScript, you can't directly manipulate binary representation:
    // let unsigned = 255;  // 11111111 in binary
    // let signed = -1;     // Also conceptually 11111111, but no direct access
    // TypeScript doesn't expose the raw bit representation
}

// =============================================================================
// 4. TYPE ALIASES (BLOOMBERG-STYLE)
// =============================================================================
// Bloomberg uses descriptive type aliases for clarity

namespace bloomberg {
    namespace types {

        // Financial types - choose signed/unsigned based on domain
        using Price = int64_t;           // Prices can be negative (discounts)
        using Quantity = uint64_t;       // Quantities are always positive
        using OrderId = uint64_t;        // IDs are always positive
        using AccountBalance = int64_t;  // Balances can be negative
        using Age = uint8_t;             // Ages are positive, small range
        using ErrorCode = int32_t;       // Error codes can be negative

        void demonstrate_bloomberg_types() {
            std::cout << "\n=== Bloomberg-Style Type Aliases ===\n";

            Price stock_price = -15025;     // $150.25 discount (negative)
            Quantity shares = 1000;         // 1000 shares (positive only)
            OrderId order_id = 123456789;   // Order ID (positive only)
            AccountBalance balance = -50000; // -$500.00 (negative balance)

            std::cout << "Stock price: $" << stock_price / 100.0 << std::endl;
            std::cout << "Shares: " << shares << std::endl;
            std::cout << "Order ID: " << order_id << std::endl;
            std::cout << "Account balance: $" << balance / 100.0 << std::endl;

            // Type safety: Can't assign negative to unsigned
            // shares = -100;  // Compilation error!
            // balance = stock_price;  // OK: both int64_t
        }

    } // namespace types
} // namespace bloomberg

// =============================================================================
// 5. PLATFORM-DEPENDENT TYPES (AVOID THESE)
// =============================================================================
// These vary by platform - don't use in Bloomberg code

void demonstrate_platform_dependent() {
    std::cout << "\n=== Platform-Dependent Types (Avoid!) ===\n";

    // These sizes can vary:
    std::cout << "sizeof(short): " << sizeof(short) << " bytes" << std::endl;
    std::cout << "sizeof(int): " << sizeof(int) << " bytes" << std::endl;
    std::cout << "sizeof(long): " << sizeof(long) << " bytes" << std::endl;
    std::cout << "sizeof(long long): " << sizeof(long long) << " bytes" << std::endl;

    // On Windows: long is 32 bits
    // On Linux: long is 64 bits
    // This causes portability issues!

    // Bloomberg standard: Always use fixed-width types
    // int32_t price;  // Always 32 bits, everywhere
    // uint64_t quantity;  // Always 64 bits, everywhere

    // In TypeScript: All numbers are 64-bit IEEE 754 floats
    // No platform differences in number representation
}

// =============================================================================
// 6. CONSTEXPR VALUES
// =============================================================================
// Compile-time constants with proper types

constexpr int8_t  MIN_TEMPERATURE = -128;    // Celsius
constexpr uint8_t MAX_AGE = 150;             // Years
constexpr uint16_t MAX_PORT = 65535;         // Network ports
constexpr uint32_t HTTP_OK = 200;            // HTTP status codes
constexpr int64_t  PLANCK_CONSTANT = 662607015; // In some unit

void demonstrate_constexpr() {
    std::cout << "\n=== constexpr Constants ===\n";

    std::cout << "Min temperature: " << static_cast<int>(MIN_TEMPERATURE) << "°C" << std::endl;
    std::cout << "Max age: " << static_cast<unsigned>(MAX_AGE) << " years" << std::endl;
    std::cout << "Max port: " << MAX_PORT << std::endl;
    std::cout << "HTTP OK: " << HTTP_OK << std::endl;
    std::cout << "Planck constant: " << PLANCK_CONSTANT << std::endl;

    // These are evaluated at compile time!
    // TypeScript equivalent:
    // const MIN_TEMPERATURE = -128;
    // But TypeScript constants are still runtime values
}

// =============================================================================
// 7. TYPE TRAITS AND PROPERTIES
// =============================================================================
// Compile-time type information

template<typename T>
void print_type_info(const std::string& name) {
    std::cout << name << ":" << std::endl;
    std::cout << "  Signed: " << std::is_signed_v<T> << std::endl;
    std::cout << "  Size: " << sizeof(T) << " bytes" << std::endl;
    std::cout << "  Min: ";

    if constexpr (std::is_signed_v<T>) {
        std::cout << std::numeric_limits<T>::min();
    } else {
        std::cout << static_cast<unsigned long long>(std::numeric_limits<T>::min());
    }

    std::cout << std::endl << "  Max: " << std::numeric_limits<T>::max() << std::endl;
}

void demonstrate_type_traits() {
    std::cout << "\n=== Type Traits ===\n";

    print_type_info<int8_t>("int8_t");
    print_type_info<uint8_t>("uint8_t");
    print_type_info<int64_t>("int64_t");
    print_type_info<uint64_t>("uint64_t");

    // In TypeScript, you'd use typeof and other runtime checks:
    // console.log(typeof 42);  // "number"
    // No compile-time type traits available
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "Signed & Unsigned Integer Types - TypeScript Developer Edition\n";
    std::cout << "=============================================================\n";

    demonstrate_basic_types();
    demonstrate_ranges();
    demonstrate_binary_representation();
    bloomberg::types::demonstrate_bloomberg_types();
    demonstrate_platform_dependent();
    demonstrate_constexpr();
    demonstrate_type_traits();

    std::cout << "\n=== Key Takeaways for TypeScript Developers ===\n";
    std::cout << "1. C++ has true integers with precise ranges and signedness\n";
    std::cout << "2. Signed: negative/zero/positive, Unsigned: zero/positive only\n";
    std::cout << "3. Same bit pattern = different values (signed vs unsigned)\n";
    std::cout << "4. Fixed-width types (int32_t) > platform types (int)\n";
    std::cout << "5. Choose signed/unsigned based on domain requirements\n";
    std::cout << "6. constexpr = compile-time constants with proper types\n";
    std::cout << "7. Type traits provide compile-time type information\n";
    std::cout << "8. Bloomberg uses descriptive aliases for clarity\n";
    std::cout << "9. Overflow behavior differs (undefined vs wraparound)\n";
    std::cout << "10. JavaScript numbers are 64-bit floats, not integers\n";

    return 0;
}
