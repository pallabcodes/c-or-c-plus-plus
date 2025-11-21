/**
 * Two's Complement Arithmetic - TypeScript Developer Edition
 *
 * Two's complement is how computers represent negative integers.
 * It's not intuitive, but it makes addition/subtraction circuits simple.
 *
 * In JavaScript/TypeScript:
 * - Numbers are stored as IEEE 754 floating point
 * - No direct access to two's complement representation
 * - Negative numbers work "magically" without thinking about bits
 *
 * In C++: You need to understand two's complement to predict overflow
 * and understand mixed signed/unsigned operations.
 */

#include <iostream>
#include <bitset>
#include <cstdint>
#include <limits>

// =============================================================================
// 1. MANUAL TWO'S COMPLEMENT CONVERSION
// =============================================================================
// Show how to convert positive to negative manually

void demonstrate_twos_complement_conversion() {
    std::cout << "\n=== Manual Two's Complement Conversion ===\n";

    // Start with positive 5: 00000101 in 8-bit binary
    int8_t positive = 5;
    std::cout << "Positive 5: " << std::bitset<8>(static_cast<uint8_t>(positive))
              << " = " << static_cast<int>(positive) << std::endl;

    // Step 1: One's complement (invert all bits): 11111010
    uint8_t ones_complement = ~static_cast<uint8_t>(positive);
    std::cout << "One's complement: " << std::bitset<8>(ones_complement)
              << " = " << static_cast<int>(ones_complement) << std::endl;

    // Step 2: Add 1: 11111011 = -5 in two's complement
    uint8_t twos_complement = ones_complement + 1;
    int8_t negative = static_cast<int8_t>(twos_complement);
    std::cout << "Two's complement: " << std::bitset<8>(twos_complement)
              << " = " << static_cast<int>(negative) << std::endl;

    // Verify: positive + negative = 0
    int8_t sum = positive + negative;
    std::cout << "5 + (-5) = " << static_cast<int>(sum) << " (should be 0)" << std::endl;

    // In TypeScript, this is hidden:
    // const positive = 5;
    // const negative = -5;
    // console.log(positive + negative); // 0
    // No need to think about bit manipulation
}

// =============================================================================
// 2. VISUALIZING TWO'S COMPLEMENT FOR DIFFERENT SIZES
// =============================================================================

template<typename T>
void show_twos_complement_range() {
    std::cout << "\n=== Two's Complement for " << sizeof(T) * 8 << "-bit "
              << (std::is_signed_v<T> ? "signed" : "unsigned") << " ===\n";

    if constexpr (!std::is_signed_v<T>) {
        // Unsigned: straightforward binary
        std::cout << "Unsigned: 0 to " << std::numeric_limits<T>::max() << std::endl;
        std::cout << "Min (0): " << std::bitset<8>(0) << std::endl;
        std::cout << "Max (" << static_cast<unsigned long long>(std::numeric_limits<T>::max())
                  << "): " << std::bitset<8>(std::numeric_limits<T>::max()) << std::endl;
        return;
    }

    // Signed: two's complement
    T min_val = std::numeric_limits<T>::min();
    T max_val = std::numeric_limits<T>::max();

    std::cout << "Range: " << static_cast<long long>(min_val) << " to "
              << static_cast<long long>(max_val) << std::endl;

    // Show key values in binary
    std::cout << "Min (" << static_cast<long long>(min_val) << "): "
              << std::bitset<8>(static_cast<uint8_t>(min_val)) << std::endl;
    std::cout << "-1: " << std::bitset<8>(static_cast<uint8_t>(-1)) << std::endl;
    std::cout << " 0: " << std::bitset<8>(static_cast<uint8_t>(0)) << std::endl;
    std::cout << "+1: " << std::bitset<8>(static_cast<uint8_t>(1)) << std::endl;
    std::cout << "Max (" << static_cast<long long>(max_val) << "): "
              << std::bitset<8>(static_cast<uint8_t>(max_val)) << std::endl;
}

void demonstrate_twos_complement_ranges() {
    show_twos_complement_range<uint8_t>();
    show_twos_complement_range<int8_t>();

    // In TypeScript:
    // console.log("Range: " + Number.MIN_SAFE_INTEGER + " to " + Number.MAX_SAFE_INTEGER);
    // But TypeScript doesn't show binary representation
}

// =============================================================================
// 3. ARITHMETIC WITH TWO'S COMPLEMENT
// =============================================================================
// Show how addition/subtraction works the same for positive/negative

void demonstrate_twos_complement_arithmetic() {
    std::cout << "\n=== Two's Complement Arithmetic ===\n";

    int8_t a = 10;   // 00001010
    int8_t b = -5;   // 11111011 (two's complement of 5)
    int8_t sum = a + b;

    std::cout << "10 + (-5) = " << static_cast<int>(sum) << std::endl;
    std::cout << "Binary: " << std::bitset<8>(static_cast<uint8_t>(a)) << " + "
              << std::bitset<8>(static_cast<uint8_t>(b)) << " = "
              << std::bitset<8>(static_cast<uint8_t>(sum)) << std::endl;

    // The CPU adds the binary representations directly!
    // No special handling needed for negative numbers

    int8_t c = -10;  // 11110110
    int8_t d = 5;    // 00000101
    int8_t diff = c - d;

    std::cout << "-10 - 5 = " << static_cast<int>(diff) << std::endl;
    std::cout << "Binary: " << std::bitset<8>(static_cast<uint8_t>(c)) << " - "
              << std::bitset<8>(static_cast<uint8_t>(d)) << " = "
              << std::bitset<8>(static_cast<uint8_t>(diff)) << std::endl;

    // In TypeScript, arithmetic is abstracted:
    // const result = 10 + (-5);  // Just works
    // No need to understand the bit representation
}

// =============================================================================
// 4. OVERFLOW IN TWO'S COMPLEMENT
// =============================================================================
// Show what happens when you exceed the range

void demonstrate_twos_complement_overflow() {
    std::cout << "\n=== Two's Complement Overflow ===\n";

    // Signed overflow (undefined behavior in C++)
    std::cout << "Signed int8_t overflow:" << std::endl;
    int8_t max_int8 = 127;  // 01111111
    std::cout << "Max int8_t: " << static_cast<int>(max_int8)
              << " (" << std::bitset<8>(static_cast<uint8_t>(max_int8)) << ")" << std::endl;

    // Adding 1 causes overflow (undefined behavior!)
    // int8_t overflow_result = max_int8 + 1;  // DANGEROUS!
    std::cout << "Adding 1 to max int8_t: UNDEFINED BEHAVIOR!" << std::endl;

    // Unsigned overflow (defined behavior - wraps around)
    std::cout << "\nUnsigned uint8_t overflow:" << std::endl;
    uint8_t max_uint8 = 255;  // 11111111
    std::cout << "Max uint8_t: " << static_cast<unsigned>(max_uint8)
              << " (" << std::bitset<8>(max_uint8) << ")" << std::endl;

    uint8_t wrap_result = max_uint8 + 1;  // Wraps to 0
    std::cout << "Adding 1 to max uint8_t: " << static_cast<unsigned>(wrap_result)
              << " (" << std::bitset<8>(wrap_result) << ") - WRAPS AROUND!" << std::endl;

    // In TypeScript: No overflow, just precision loss
    // const max = Number.MAX_SAFE_INTEGER;
    // const overflow = max + 1;
    // console.log(overflow);  // max + 1 (precision lost)
}

// =============================================================================
// 5. WHY TWO'S COMPLEMENT WORKS
// =============================================================================
// Demonstrate the mathematical properties

void demonstrate_twos_complement_properties() {
    std::cout << "\n=== Why Two's Complement Works ===\n";

    // Property 1: Negation wraps around correctly
    int8_t positive = 5;
    int8_t negative = -positive;
    int8_t sum = positive + negative;

    std::cout << positive << " + " << static_cast<int>(negative)
              << " = " << static_cast<int>(sum) << " (should be 0)" << std::endl;

    // Property 2: Range is asymmetric but correct
    int8_t min_val = std::numeric_limits<int8_t>::min();  // -128
    int8_t max_val = std::numeric_limits<int8_t>::max();  // +127

    std::cout << "Range: " << static_cast<int>(min_val) << " to "
              << static_cast<int>(max_val) << std::endl;

    // Property 3: -128 has no positive counterpart
    // This is because 10000000 is -128 in two's complement
    // The corresponding positive would be +128, which doesn't fit in 8 bits

    std::cout << "-128 in binary: " << std::bitset<8>(static_cast<uint8_t>(min_val)) << std::endl;
    std::cout << "There is no +128 in 8-bit signed integers!" << std::endl;

    // In TypeScript: No such asymmetry
    // Numbers are symmetric around zero until precision limits
}

// =============================================================================
// 6. CONVERTING BETWEEN SIGNED AND UNSIGNED
// =============================================================================
// Show reinterpret_cast and static_cast behavior

void demonstrate_signed_unsigned_conversion() {
    std::cout << "\n=== Signed/Unsigned Conversion ===\n";

    // Value preservation (static_cast)
    int8_t signed_val = -42;
    uint8_t unsigned_val = static_cast<uint8_t>(signed_val);  // -42 -> 214

    std::cout << "int8_t " << static_cast<int>(signed_val)
              << " -> uint8_t " << static_cast<unsigned>(unsigned_val) << std::endl;

    // Bit pattern preservation (reinterpret_cast)
    uint8_t bit_pattern = *reinterpret_cast<uint8_t*>(&signed_val);
    std::cout << "Bit pattern of " << static_cast<int>(signed_val)
              << ": " << std::bitset<8>(bit_pattern) << std::endl;

    // The bit pattern is the same, interpretation differs!
    int8_t back_to_signed = *reinterpret_cast<int8_t*>(&bit_pattern);
    std::cout << "Interpreting same bits as signed: " << static_cast<int>(back_to_signed) << std::endl;

    // In TypeScript: No direct bit manipulation
    // Would need to use DataView or TypedArrays:
    // const buffer = new ArrayBuffer(1);
    // const view = new DataView(buffer);
    // view.setInt8(0, -42);
    // console.log(view.getUint8(0));  // 214
}

// =============================================================================
// 7. PRACTICAL TWO'S COMPLEMENT EXAMPLES
// =============================================================================
// Real-world scenarios where two's complement matters

void demonstrate_practical_examples() {
    std::cout << "\n=== Practical Two's Complement Examples ===\n";

    // Example 1: Temperature calculations
    int8_t temperature = -10;  // -10°C
    int8_t adjustment = 5;     // +5°C
    int8_t new_temp = temperature + adjustment;

    std::cout << "Temperature: " << static_cast<int>(temperature) << "°C + "
              << static_cast<int>(adjustment) << "°C = "
              << static_cast<int>(new_temp) << "°C" << std::endl;

    // Example 2: Financial calculations (can be negative)
    int32_t profit_loss = -50000;  // -$500.00 in cents
    int32_t adjustment = 25000;    // +$250.00
    int32_t new_balance = profit_loss + adjustment;

    std::cout << "P&L: $" << profit_loss / 100.0 << " + $"
              << adjustment / 100.0 << " = $"
              << new_balance / 100.0 << std::endl;

    // Example 3: Array indexing (usually unsigned)
    uint32_t array_size = 1000;
    uint32_t index = 500;
    uint32_t offset = 100;
    uint32_t new_index = index + offset;

    std::cout << "Array index: " << index << " + offset " << offset
              << " = " << new_index << " (no overflow)" << std::endl;

    // In TypeScript, all these calculations "just work":
    // const temp = -10 + 5;        //  -5
    // const balance = -500.00 + 250.00;  // -250.00
    // const index = 500 + 100;     // 600
}

// =============================================================================
// 8. BLOOMBERG-STYLE TWO'S COMPLEMENT USAGE
// =============================================================================
// How Bloomberg uses signed/unsigned in financial contexts

namespace bloomberg {
    namespace finance {

        // Price: Can be negative (discounts)
        using Price = int64_t;

        // Quantity: Always positive
        using Quantity = uint64_t;

        // P&L: Can be negative
        using ProfitLoss = int64_t;

        void demonstrate_finance_calculations() {
            std::cout << "\n=== Bloomberg Financial Calculations ===\n";

            Price stock_price = 15025;     // $150.25
            Price discount = -5025;        // -$50.25 discount
            Price final_price = stock_price + discount;

            std::cout << "Stock price: $" << stock_price / 100.0 << std::endl;
            std::cout << "Discount: $" << discount / 100.0 << std::endl;
            std::cout << "Final price: $" << final_price / 100.0 << std::endl;

            Quantity shares = 1000;        // Always positive
            ProfitLoss pnl = -25000;       // Can be negative
            ProfitLoss adjustment = 50000; // Positive adjustment
            ProfitLoss new_pnl = pnl + adjustment;

            std::cout << "Shares: " << shares << std::endl;
            std::cout << "P&L: $" << pnl / 100.0 << std::endl;
            std::cout << "Adjustment: $" << adjustment / 100.0 << std::endl;
            std::cout << "New P&L: $" << new_pnl / 100.0 << std::endl;
        }

    } // namespace finance
} // namespace bloomberg

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "Two's Complement Arithmetic - TypeScript Developer Edition\n";
    std::cout << "=========================================================\n";

    demonstrate_twos_complement_conversion();
    demonstrate_twos_complement_ranges();
    demonstrate_twos_complement_arithmetic();
    demonstrate_twos_complement_overflow();
    demonstrate_twos_complement_properties();
    demonstrate_signed_unsigned_conversion();
    demonstrate_practical_examples();
    bloomberg::finance::demonstrate_finance_calculations();

    std::cout << "\n=== Two's Complement Takeaways for TypeScript Devs ===\n";
    std::cout << "1. Two's complement: Invert bits + 1 for negative representation\n";
    std::cout << "2. Signed range: -2^(n-1) to +2^(n-1)-1 (asymmetric)\n";
    std::cout << "3. Unsigned range: 0 to 2^n-1 (symmetric, no negative)\n";
    std::cout << "4. Same bit pattern = different values (signed vs unsigned)\n";
    std::cout << "5. Arithmetic works the same for positive/negative (CPU magic)\n";
    std::cout << "6. Signed overflow = undefined behavior (dangerous!)\n";
    std::cout << "7. Unsigned overflow = defined wraparound (predictable)\n";
    std::cout << "8. Choose signed/unsigned based on domain (finance needs signed)\n";
    std::cout << "9. TypeScript hides all this complexity (numbers 'just work')\n";
    std::cout << "10. C++ requires understanding two's complement for correctness\n";

    return 0;
}
