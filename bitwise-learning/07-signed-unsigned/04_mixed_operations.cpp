/**
 * Mixed Signed/Unsigned Operations and Pitfalls - TypeScript Developer Edition
 *
 * When signed and unsigned integers interact, unexpected things happen.
 * C++ has "usual arithmetic conversions" that can surprise you.
 *
 * In JavaScript/TypeScript:
 * - All numbers are signed 64-bit floats
 * - No unsigned types
 * - No conversion issues
 *
 * In C++: Mixed operations cause silent conversions that change meaning!
 */

#include <iostream>
#include <vector>
#include <cstdint>

// =============================================================================
// 1. USUAL ARITHMETIC CONVERSIONS
// =============================================================================
// When mixing signed and unsigned, C++ converts to a common type

void demonstrate_usual_conversions() {
    std::cout << "\n=== Usual Arithmetic Conversions ===\n";

    int32_t signed_val = -10;
    uint32_t unsigned_val = 5;

    std::cout << "signed_val: " << signed_val << std::endl;
    std::cout << "unsigned_val: " << unsigned_val << std::endl;

    // What happens in: signed_val + unsigned_val ?
    // Rule: If types differ in signedness, unsigned "wins"
    // int32_t (-10) gets converted to uint32_t (4294967286)

    auto result = signed_val + unsigned_val;
    std::cout << "signed_val + unsigned_val = " << result << std::endl;
    std::cout << "What really happened: -10 (int32_t) → 4294967286 (uint32_t)" << std::endl;
    std::cout << "Then: 4294967286 + 5 = " << (4294967286U + 5U) << std::endl;

    // In TypeScript: No such conversions
    // const result = -10 + 5;  // Just -5
}

// =============================================================================
// 2. COMPARISON PITFALLS
// =============================================================================
// Comparisons between signed and unsigned are dangerous

void demonstrate_comparison_pitfalls() {
    std::cout << "\n=== Comparison Pitfalls ===\n";

    uint32_t unsigned_val = 1;
    int32_t signed_val = -1;

    std::cout << "unsigned_val: " << unsigned_val << std::endl;
    std::cout << "signed_val: " << signed_val << std::endl;

    // DANGER: signed_val gets converted to unsigned!
    // -1 (int32_t) becomes 4294967295 (uint32_t)
    bool result = unsigned_val > signed_val;
    std::cout << "unsigned_val > signed_val: " << (result ? "true" : "false") << std::endl;
    std::cout << "Why? -1 converts to 4294967295, and 1 > 4294967295 is false" << std::endl;

    // More examples
    std::vector<int> data = {-1, 0, 1};
    size_t size = data.size();  // size_t is unsigned

    std::cout << "data.size(): " << size << std::endl;

    // This comparison might not work as expected!
    if (size >= 0) {  // Always true, but compiler might warn
        std::cout << "Size is non-negative (always true)" << std::endl;
    }

    // DANGER: This can cause infinite loops
    for (int i = 10; i >= 0; --i) {  // OK: i is signed
        std::cout << "i = " << i << std::endl;
        if (i == 5) break;  // Prevent infinite output
    }

    // In TypeScript: No such issues
    // const result = 1 > -1;  // true
    // for (let i = 10; i >= 0; i--) { ... }  // Works fine
}

// =============================================================================
// 3. LOOP VARIABLE PROBLEMS
// =============================================================================
// Using unsigned for loop variables can cause infinite loops

void demonstrate_loop_problems() {
    std::cout << "\n=== Loop Variable Problems ===\n";

    // PROBLEM: Using unsigned for countdown
    std::cout << "Using unsigned for countdown (DANGEROUS):" << std::endl;
    for (size_t i = 5; i >= 0; --i) {  // size_t is unsigned!
        std::cout << "i = " << i << std::endl;
        if (i == 0) break;  // Prevent infinite loop demonstration
    }
    std::cout << "When i reaches 0, --i makes it UINT_MAX! Infinite loop!" << std::endl;

    // SAFE: Use signed for countdown
    std::cout << "\nUsing signed for countdown (SAFE):" << std::endl;
    for (int i = 5; i >= 0; --i) {
        std::cout << "i = " << i << std::endl;
    }

    // Another pitfall: Array indexing
    std::cout << "\nArray indexing pitfall:" << std::endl;
    std::vector<int> array = {10, 20, 30};
    size_t index = 0;

    // This comparison is always true (size_t can't be negative)
    if (index - 1 >= 0) {
        std::cout << "This will never execute" << std::endl;
    } else {
        std::cout << "index - 1 is treated as unsigned, becomes huge number" << std::endl;
    }

    // In TypeScript: No such issues
    // for (let i = 5; i >= 0; i--) { console.log(i); }  // Works
}

// =============================================================================
// 4. FUNCTION PARAMETER CONVERSIONS
// =============================================================================
// Function calls can cause unexpected conversions

void process_quantity(uint32_t quantity) {
    std::cout << "Processing quantity: " << quantity << std::endl;
}

void process_offset(int32_t offset) {
    std::cout << "Processing offset: " << offset << std::endl;
}

void demonstrate_function_parameters() {
    std::cout << "\n=== Function Parameter Conversions ===\n";

    int32_t signed_quantity = 100;
    uint32_t unsigned_offset = 50;

    // This is OK: int32_t promotes to uint32_t
    process_quantity(signed_quantity);  // 100

    // This is OK: uint32_t converts to int32_t (if value fits)
    process_offset(unsigned_offset);  // 50

    // DANGER: What if unsigned_offset > INT32_MAX?
    uint32_t large_unsigned = 3000000000U;  // > 2^31 - 1
    // process_offset(large_unsigned);  // UNDEFINED BEHAVIOR!

    std::cout << "Large unsigned value: " << large_unsigned << std::endl;
    std::cout << "INT32_MAX: " << INT32_MAX << std::endl;
    std::cout << "Passing large unsigned to int32_t parameter: DANGEROUS!" << std::endl;

    // In TypeScript: No such conversion issues
    // function processQuantity(quantity: number) { ... }
    // processQuantity(100);  // Just works
}

// =============================================================================
// 5. ARITHMETIC OPERATIONS MIXING TYPES
// =============================================================================
// Show how different operations handle mixed types

void demonstrate_arithmetic_mixing() {
    std::cout << "\n=== Arithmetic Operations with Mixed Types ===\n";

    int16_t small_signed = 1000;
    uint32_t large_unsigned = 50000;

    // Addition: int16_t promotes to uint32_t
    auto sum = small_signed + large_unsigned;
    std::cout << "int16_t(" << small_signed << ") + uint32_t(" << large_unsigned
              << ") = " << sum << " (type: " << typeid(sum).name() << ")" << std::endl;

    // Subtraction: Can cause wraparound
    int32_t negative = -100;
    uint32_t positive = 50;

    auto diff = positive - negative;  // uint32_t - int32_t
    std::cout << "uint32_t(" << positive << ") - int32_t(" << negative
              << ") = " << diff << std::endl;

    // What actually happens:
    // negative (-100) converts to unsigned: 4294967196
    // Then: 50 - 4294967196 = wraparound result
    std::cout << "What really happened: -100 → " << static_cast<uint32_t>(negative)
              << " (unsigned), then 50 - that value" << std::endl;

    // In TypeScript: No conversion drama
    // const sum = 1000 + 50000;  // 51000
    // const diff = 50 - (-100);  // 150
}

// =============================================================================
// 6. BITWISE OPERATIONS WITH MIXED TYPES
// =============================================================================
// Bitwise ops also trigger conversions

void demonstrate_bitwise_mixing() {
    std::cout << "\n=== Bitwise Operations with Mixed Types ===\n";

    int8_t signed_byte = -1;    // 11111111 in two's complement
    uint8_t unsigned_byte = 255; // 11111111 in binary

    // Same bit pattern, different interpretation
    std::cout << "signed_byte: " << static_cast<int>(signed_byte) << std::endl;
    std::cout << "unsigned_byte: " << static_cast<unsigned>(unsigned_byte) << std::endl;

    // Bitwise operations promote to int (usually 32-bit)
    int bitwise_and = signed_byte & unsigned_byte;
    std::cout << "signed_byte & unsigned_byte = " << bitwise_and << std::endl;

    // But comparisons still convert to unsigned!
    bool comparison = signed_byte == unsigned_byte;
    std::cout << "signed_byte == unsigned_byte: " << (comparison ? "true" : "false") << std::endl;
    std::cout << "Why? signed_byte converts to unsigned: "
              << static_cast<unsigned>(signed_byte) << std::endl;

    // In TypeScript: Bitwise operations work on 32-bit signed integers
    // let result = -1 & 255;  // -1 (both are treated as signed)
    // let comparison = -1 === 255;  // false (different values)
}

// =============================================================================
// 7. ARRAY INDEXING DANGERS
// =============================================================================
// Negative indices and unsigned sizes

void demonstrate_array_indexing() {
    std::cout << "\n=== Array Indexing Dangers ===\n";

    std::vector<int> data = {10, 20, 30, 40, 50};
    size_t size = data.size();  // unsigned

    std::cout << "Array size: " << size << std::endl;

    // DANGER: Comparing signed with unsigned
    int user_index = -1;  // Maybe from user input

    if (user_index < size) {  // size is unsigned, user_index converts to unsigned!
        std::cout << "Accessing array[" << user_index << "]" << std::endl;
        // user_index (-1) becomes UINT_MAX, which is >= size
        // So this condition might be false when you expect true
        data[user_index];  // This could crash or access wrong memory
    } else {
        std::cout << "Index out of bounds" << std::endl;
    }

    // SAFE way: Check bounds properly
    if (user_index >= 0 && static_cast<size_t>(user_index) < size) {
        std::cout << "Safe access: data[" << user_index << "] = " << data[user_index] << std::endl;
    } else {
        std::cout << "Index out of bounds (safe check)" << std::endl;
    }

    // In TypeScript: No such issues
    // const data = [10, 20, 30, 40, 50];
    // if (-1 < data.length) { data[-1]; }  // Just doesn't work, no crash
}

// =============================================================================
// 8. BLOOMBERG-STYLE MIXED TYPE HANDLING
// =============================================================================
// How Bloomberg handles these issues in practice

namespace bloomberg {
    namespace safe_types {

        // Safe wrapper for array indices
        class ArrayIndex {
            size_t index_;
        public:
            explicit ArrayIndex(size_t idx) : index_(idx) {}
            size_t value() const { return index_; }
        };

        // Safe wrapper for quantities (always positive)
        class Quantity {
            uint64_t quantity_;
        public:
            explicit Quantity(uint64_t qty) : quantity_(qty) {}
            uint64_t value() const { return quantity_; }
        };

        // Safe wrapper for amounts (can be negative)
        class Amount {
            int64_t amount_;
        public:
            explicit Amount(int64_t amt) : amount_(amt) {}
            int64_t value() const { return amount_; }
        };

        // Safe arithmetic operations
        class SafeMath {
        public:
            static bool is_valid_index(int64_t idx, size_t container_size) {
                return idx >= 0 && static_cast<size_t>(idx) < container_size;
            }

            static bool is_valid_quantity(int64_t qty) {
                return qty >= 0;
            }

            static std::optional<uint64_t> safe_cast_to_unsigned(int64_t value) {
                if (value < 0) return std::nullopt;
                return static_cast<uint64_t>(value);
            }
        };

        void demonstrate_bloomberg_safe_types() {
            std::cout << "\n=== Bloomberg Safe Type Handling ===\n";

            std::vector<int> data = {100, 200, 300};

            // Safe indexing
            int64_t user_input = 1;  // Could be from user (might be negative)
            if (SafeMath::is_valid_index(user_input, data.size())) {
                ArrayIndex safe_idx(static_cast<size_t>(user_input));
                std::cout << "Safe access: data[" << safe_idx.value() << "] = "
                          << data[safe_idx.value()] << std::endl;
            } else {
                std::cout << "Invalid index: " << user_input << std::endl;
            }

            // Safe quantity handling
            int64_t raw_quantity = -100;  // Invalid quantity
            auto safe_qty = SafeMath::safe_cast_to_unsigned(raw_quantity);
            if (safe_qty) {
                Quantity qty(*safe_qty);
                std::cout << "Valid quantity: " << qty.value() << std::endl;
            } else {
                std::cout << "Invalid quantity: " << raw_quantity << std::endl;
            }

            // Financial amounts (can be negative)
            Amount credit(50000);   // +$500.00
            Amount debit(-25000);   // -$250.00
            Amount net = Amount(credit.value() + debit.value());

            std::cout << "Credit: $" << credit.value() / 100.0 << std::endl;
            std::cout << "Debit: $" << debit.value() / 100.0 << std::endl;
            std::cout << "Net: $" << net.value() / 100.0 << std::endl;
        }

    } // namespace safe_types
} // namespace bloomberg

// =============================================================================
// 9. TYPESCRIPT WORKAROUNDS
// =============================================================================
// How TypeScript avoids these issues

void demonstrate_typescript_workarounds() {
    std::cout << "\n=== TypeScript Workarounds ===\n";

    std::cout << "TypeScript avoids C++ mixed type issues by:" << std::endl;
    std::cout << "1. All numbers are 64-bit IEEE 754 floats" << std::endl;
    std::cout << "2. No integer overflow (precision loss instead)" << std::endl;
    std::cout << "3. No signed/unsigned distinction" << std::endl;
    std::cout << "4. Automatic type coercion in operations" << std::endl;
    std::cout << "5. BigInt for arbitrary precision integers" << std::endl;

    std::cout << "\nTypeScript equivalents:" << std::endl;
    std::cout << "// Instead of: uint32_t qty; int64_t price;" << std::endl;
    std::cout << "// Use: let quantity: number; let price: number;" << std::endl;

    std::cout << "\n// Instead of checking mixed comparisons:" << std::endl;
    std::cout << "// Use: if (index >= 0 && index < array.length)" << std::endl;

    std::cout << "\n// For safety: use BigInt" << std::endl;
    std::cout << "// let safeQuantity: bigint = 100n;" << std::endl;
    std::cout << "// let safePrice: bigint = -5000n;" << std::endl;
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "Mixed Signed/Unsigned Operations and Pitfalls - TypeScript Developer Edition\n";
    std::cout << "============================================================================\n";

    demonstrate_usual_conversions();
    demonstrate_comparison_pitfalls();
    demonstrate_loop_problems();
    demonstrate_function_parameters();
    demonstrate_arithmetic_mixing();
    demonstrate_bitwise_mixing();
    demonstrate_array_indexing();
    bloomberg::safe_types::demonstrate_bloomberg_safe_types();
    demonstrate_typescript_workarounds();

    std::cout << "\n=== Mixed Operations Takeaways for TypeScript Devs ===\n";
    std::cout << "1. Usual conversions: unsigned 'wins', signed converts to unsigned\n";
    std::cout << "2. Comparisons: -1 > 1 becomes false (conversion to unsigned)\n";
    std::cout << "3. Loops: Never use unsigned for countdown (infinite loop risk)\n";
    std::cout << "4. Functions: Parameter conversion can cause undefined behavior\n";
    std::cout << "5. Arithmetic: Mixed ops convert to unsigned, can overflow\n";
    std::cout << "6. Arrays: size_t (unsigned) vs int indices cause issues\n";
    std::cout << "7. Bloomberg: Uses wrapper classes for type safety\n";
    std::cout << "8. TypeScript: No such issues - all numbers are signed floats\n";
    std::cout << "9. Prevention: Use same signedness, or explicit casting\n";
    std::cout << "10. Testing: Always test edge cases with mixed operations\n";

    return 0;
}
