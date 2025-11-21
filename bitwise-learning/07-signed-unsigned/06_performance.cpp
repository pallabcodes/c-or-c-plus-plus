/**
 * Performance Considerations and Bit-Width Choices - TypeScript Developer Edition
 *
 * In C++, choosing the right integer type involves balancing:
 * - Memory usage (smaller types save space)
 * - Performance (alignment, cache efficiency)
 * - Range requirements (don't use 64-bit for 8-bit data)
 * - Platform considerations (32-bit vs 64-bit systems)
 *
 * In TypeScript/JavaScript:
 * - All numbers are 64-bit floats (no choice!)
 * - Performance is about avoiding boxing/unboxing
 * - Memory usage is less of a concern
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <cstdint>
#include <limits>

// =============================================================================
// 1. MEMORY USAGE COMPARISON
// =============================================================================
// Show how different integer types affect memory usage

void demonstrate_memory_usage() {
    std::cout << "\n=== Memory Usage Comparison ===\n";

    // Array sizes for different types
    const size_t ELEMENT_COUNT = 1000000;  // 1 million elements

    std::cout << "Array of " << ELEMENT_COUNT << " elements:" << std::endl;
    std::cout << "Type\tSize per element\tTotal memory" << std::endl;
    std::cout << "int8_t\t" << sizeof(int8_t) << " byte\t\t" << (ELEMENT_COUNT * sizeof(int8_t)) / 1024 << " KB" << std::endl;
    std::cout << "int32_t\t" << sizeof(int32_t) << " bytes\t\t" << (ELEMENT_COUNT * sizeof(int32_t)) / 1024 / 1024 << " MB" << std::endl;
    std::cout << "int64_t\t" << sizeof(int64_t) << " bytes\t\t" << (ELEMENT_COUNT * sizeof(int64_t)) / 1024 / 1024 << " MB" << std::endl;

    // In TypeScript: All numbers are 8 bytes regardless
    // const array = new Array(1000000).fill(0);  // 8MB for the array alone
    // But each number is still a 64-bit float
}

// =============================================================================
// 2. CACHE EFFICIENCY
// =============================================================================
// Smaller types can improve cache performance

void demonstrate_cache_efficiency() {
    std::cout << "\n=== Cache Efficiency ===\n";

    // CPU cache line is typically 64 bytes
    const size_t CACHE_LINE_SIZE = 64;

    std::cout << "Cache line efficiency (64-byte cache line):" << std::endl;

    // How many elements fit in one cache line?
    std::cout << "int8_t:  " << CACHE_LINE_SIZE / sizeof(int8_t) << " elements per cache line" << std::endl;
    std::cout << "int16_t: " << CACHE_LINE_SIZE / sizeof(int16_t) << " elements per cache line" << std::endl;
    std::cout << "int32_t: " << CACHE_LINE_SIZE / sizeof(int32_t) << " elements per cache line" << std::endl;
    std::cout << "int64_t: " << CACHE_LINE_SIZE / sizeof(int64_t) << " elements per cache line" << std::endl;

    // Smaller types = more data fits in cache = better performance
    // But: alignment requirements may add padding
}

// =============================================================================
// 3. ALIGNMENT CONSIDERATIONS
// =============================================================================
// Data alignment affects performance and memory usage

struct AlignedData {
    int8_t  a;  // 1 byte
    int32_t b;  // 4 bytes (needs 4-byte alignment)
    int16_t c;  // 2 bytes
    // Compiler adds 1 byte padding here to align b
    // Total: 12 bytes instead of 7 bytes
};

struct OptimizedData {
    int32_t b;  // 4 bytes (4-byte aligned)
    int16_t c;  // 2 bytes
    int8_t  a;  // 1 byte
    // No padding needed
    // Total: 7 bytes
};

void demonstrate_alignment() {
    std::cout << "\n=== Data Alignment ===\n";

    std::cout << "AlignedData size: " << sizeof(AlignedData) << " bytes" << std::endl;
    std::cout << "OptimizedData size: " << sizeof(OptimizedData) << " bytes" << std::endl;

    // Unaligned access can be slower or impossible on some architectures
    // Always consider struct member order for optimal alignment
}

// =============================================================================
// 4. PERFORMANCE MEASUREMENT
// =============================================================================
// Compare performance of different integer types

template<typename T>
double benchmark_operation(const std::string& operation_name, size_t iterations) {
    std::vector<T> data(iterations);
    for (size_t i = 0; i < iterations; ++i) {
        data[i] = static_cast<T>(i % std::numeric_limits<T>::max());
    }

    auto start = std::chrono::high_resolution_clock::now();

    // Perform operations
    T sum = 0;
    for (const auto& val : data) {
        sum += val;
        sum *= 2;  // Mix of operations
        sum /= 2;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << operation_name << ": " << duration.count() << " microseconds" << std::endl;
    return duration.count();
}

void demonstrate_performance_measurement() {
    std::cout << "\n=== Performance Measurement ===\n";

    const size_t ITERATIONS = 1000000;

    benchmark_operation<int8_t>("int8_t operations", ITERATIONS);
    benchmark_operation<int32_t>("int32_t operations", ITERATIONS);
    benchmark_operation<int64_t>("int64_t operations", ITERATIONS);

    // Results vary by architecture, but generally:
    // - Smaller types may be slower due to more operations needed
    // - Larger types may have alignment overhead
    // - int32_t is often fastest on 32/64-bit systems
}

// =============================================================================
// 5. BIT-WIDTH CHOICE GUIDELINES
// =============================================================================
// Bloomberg-style guidelines for choosing integer types

namespace bloomberg {
    namespace guidelines {

        // Guidelines based on typical ranges
        enum class RecommendedType {
            USE_INT8,
            USE_INT16,
            USE_INT32,
            USE_INT64,
            USE_UINT8,
            USE_UINT16,
            USE_UINT32,
            USE_UINT64
        };

        RecommendedType choose_type(const std::string& use_case, bool can_be_negative, int64_t min_val, int64_t max_val) {
            // Determine if signed or unsigned is appropriate
            bool use_signed = can_be_negative || min_val < 0;

            // Choose smallest type that fits the range
            if (!use_signed) {
                // Unsigned
                if (max_val <= UINT8_MAX) return RecommendedType::USE_UINT8;
                if (max_val <= UINT16_MAX) return RecommendedType::USE_UINT16;
                if (max_val <= UINT32_MAX) return RecommendedType::USE_UINT32;
                return RecommendedType::USE_UINT64;
            } else {
                // Signed
                if (min_val >= INT8_MIN && max_val <= INT8_MAX) return RecommendedType::USE_INT8;
                if (min_val >= INT16_MIN && max_val <= INT16_MAX) return RecommendedType::USE_INT16;
                if (min_val >= INT32_MIN && max_val <= INT32_MAX) return RecommendedType::USE_INT32;
                return RecommendedType::USE_INT64;
            }
        }

        std::string type_to_string(RecommendedType type) {
            switch (type) {
                case RecommendedType::USE_INT8: return "int8_t";
                case RecommendedType::USE_INT16: return "int16_t";
                case RecommendedType::USE_INT32: return "int32_t";
                case RecommendedType::USE_INT64: return "int64_t";
                case RecommendedType::USE_UINT8: return "uint8_t";
                case RecommendedType::USE_UINT16: return "uint16_t";
                case RecommendedType::USE_UINT32: return "uint32_t";
                case RecommendedType::USE_UINT64: return "uint64_t";
                default: return "unknown";
            }
        }

        void demonstrate_type_choice() {
            std::cout << "\n=== Bloomberg Type Choice Guidelines ===\n";

            // Common use cases
            struct UseCase {
                std::string name;
                bool can_be_negative;
                int64_t min_val;
                int64_t max_val;
            };

            std::vector<UseCase> cases = {
                {"Age", false, 0, 150},
                {"Temperature (°C)", true, -100, 100},
                {"Array index", false, 0, 1000000},
                {"File size (bytes)", false, 0, 1000000000000LL},  // 1TB
                {"Price (cents)", true, -10000000000LL, 10000000000LL},  // ±$100M
                {"Order ID", false, 1, 1000000000000LL},
                {"Error code", true, -1000, 1000},
                {"Port number", false, 0, 65535}
            };

            for (const auto& uc : cases) {
                auto recommended = choose_type(uc.name, uc.can_be_negative, uc.min_val, uc.max_val);
                std::cout << uc.name << ": " << type_to_string(recommended)
                          << " (range: " << uc.min_val << " to " << uc.max_val << ")" << std::endl;
            }
        }

    } // namespace guidelines
} // namespace bloomberg

// =============================================================================
// 6. ARCHITECTURE CONSIDERATIONS
// =============================================================================
// How integer performance varies by CPU architecture

void demonstrate_architecture_considerations() {
    std::cout << "\n=== Architecture Considerations ===\n";

    // Check if we're on 64-bit system
    std::cout << "sizeof(size_t): " << sizeof(size_t) << " bytes" << std::endl;
    std::cout << "sizeof(void*): " << sizeof(void*) << " bytes" << std::endl;

    if (sizeof(size_t) == 8) {
        std::cout << "64-bit architecture detected" << std::endl;
        std::cout << "Recommendations:" << std::endl;
        std::cout << "- Use int64_t for general-purpose integers" << std::endl;
        std::cout << "- Use size_t for array indices and sizes" << std::endl;
        std::cout << "- Consider int32_t for hot loop variables (may be faster)" << std::endl;
    } else {
        std::cout << "32-bit architecture detected" << std::endl;
        std::cout << "Recommendations:" << std::endl;
        std::cout << "- Use int32_t for general-purpose integers" << std::endl;
        std::cout << "- Be careful with large data structures" << std::endl;
    }

    // SIMD considerations
    std::cout << "\nSIMD (Single Instruction, Multiple Data):" << std::endl;
    std::cout << "- int8_t: Good for SIMD operations" << std::endl;
    std::cout << "- int32_t: Balanced for most operations" << std::endl;
    std::cout << "- int64_t: May be slower in SIMD contexts" << std::endl;
}

// =============================================================================
// 7. MEMORY BANDWIDTH CONSIDERATIONS
// =============================================================================
// How integer size affects memory bandwidth usage

void demonstrate_memory_bandwidth() {
    std::cout << "\n=== Memory Bandwidth Considerations ===\n";

    // Simulate processing large arrays
    const size_t LARGE_SIZE = 10000000;  // 10 million elements

    // For int8_t: 10MB of data
    // For int32_t: 40MB of data
    // For int64_t: 80MB of data

    std::cout << "Processing " << LARGE_SIZE << " elements:" << std::endl;
    std::cout << "int8_t:  " << (LARGE_SIZE * sizeof(int8_t)) / 1024 / 1024 << " MB" << std::endl;
    std::cout << "int32_t: " << (LARGE_SIZE * sizeof(int32_t)) / 1024 / 1024 << " MB" << std::endl;
    std::cout << "int64_t: " << (LARGE_SIZE * sizeof(int64_t)) / 1024 / 1024 << " MB" << std::endl;

    // Smaller types = less memory bandwidth usage
    // Important for cache-bound algorithms
    // But: smaller types may require more instructions
}

// =============================================================================
// 8. COMPILER OPTIMIZATION EFFECTS
// =============================================================================
// How compilers optimize different integer types

void demonstrate_compiler_optimization() {
    std::cout << "\n=== Compiler Optimization Effects ===\n";

    // Some operations are optimized differently
    std::cout << "Compiler optimizations:" << std::endl;
    std::cout << "- int32_t often has the most optimized code" << std::endl;
    std::cout << "- uint32_t good for loop counters and array indices" << std::endl;
    std::cout << "- Smaller types may generate more instructions" << std::endl;
    std::cout << "- Larger types may have alignment overhead" << std::endl;

    // Example: Some compilers optimize uint32_t loops better
    // because they can assume no overflow in certain contexts

    // For hot paths, measure performance with different types
    std::cout << "Always profile hot paths with different integer types" << std::endl;
}

// =============================================================================
// 9. CROSS-PLATFORM PERFORMANCE
// =============================================================================
// Performance differences across platforms

void demonstrate_cross_platform() {
    std::cout << "\n=== Cross-Platform Performance ===\n";

    std::cout << "Performance varies by platform:" << std::endl;
    std::cout << "x86-64: int64_t may be slower than int32_t in some cases" << std::endl;
    std::cout << "ARM: Different optimization characteristics" << std::endl;
    std::cout << "RISC-V: May have different preferences" << std::endl;

    std::cout << "\nBloomberg approach:" << std::endl;
    std::cout << "- Use fixed-width types for portability" << std::endl;
    std::cout << "- Profile on target platforms" << std::endl;
    std::cout << "- Prefer int32_t for general use unless larger range needed" << std::endl;
    std::cout << "- Use uint64_t for sizes and counts that might exceed 4GB" << std::endl;
}

// =============================================================================
// 10. TYPESCRIPT PERFORMANCE CONSIDERATIONS
// =============================================================================
// How integer performance works in TypeScript/JavaScript

void demonstrate_typescript_performance() {
    std::cout << "\n=== TypeScript Performance Considerations ===\n";

    std::cout << "TypeScript/JavaScript:" << std::endl;
    std::cout << "- All numbers are 64-bit IEEE 754 floats" << std::endl;
    std::cout << "- No integer types - everything is floating point" << std::endl;
    std::cout << "- Performance depends on V8 optimizations" << std::endl;
    std::cout << "- Use TypedArrays for true integer performance:" << std::endl;
    std::cout << "  const int32Array = new Int32Array(1000); // True 32-bit integers" << std::endl;
    std::cout << "  const uint8Array = new Uint8Array(1000);  // True 8-bit unsigned" << std::endl;

    std::cout << "\nTypedArray performance:" << std::endl;
    std::cout << "- Int8Array: 8-bit signed integers" << std::endl;
    std::cout << "- Uint8Array: 8-bit unsigned integers" << std::endl;
    std::cout << "- Int16Array: 16-bit signed integers" << std::endl;
    std::cout << "- Uint16Array: 16-bit unsigned integers" << std::endl;
    std::cout << "- Int32Array: 32-bit signed integers" << std::endl;
    std::cout << "- Uint32Array: 32-bit unsigned integers" << std::endl;
    std::cout << "- BigInt64Array: 64-bit signed BigInts" << std::endl;
    std::cout << "- BigUint64Array: 64-bit unsigned BigInts" << std::endl;
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "Performance Considerations and Bit-Width Choices - TypeScript Developer Edition\n";
    std::cout << "===============================================================================\n";

    demonstrate_memory_usage();
    demonstrate_cache_efficiency();
    demonstrate_alignment();
    demonstrate_performance_measurement();
    bloomberg::guidelines::demonstrate_type_choice();
    demonstrate_architecture_considerations();
    demonstrate_memory_bandwidth();
    demonstrate_compiler_optimization();
    demonstrate_cross_platform();
    demonstrate_typescript_performance();

    std::cout << "\n=== Performance Takeaways for TypeScript Devs ===\n";
    std::cout << "1. Choose smallest type that fits your range (memory efficiency)\n";
    std::cout << "2. Consider cache line utilization (64-byte alignment)\n";
    std::cout << "3. int32_t often fastest on modern architectures\n";
    std::cout << "4. Unsigned types good for counters, indices, sizes\n";
    std::cout << "5. Signed types needed when negative values possible\n";
    std::cout << "6. Bloomberg: Use int64_t for financial amounts, uint64_t for IDs\n";
    std::cout << "7. Always profile performance-critical code\n";
    std::cout << "8. Consider data structure alignment for performance\n";
    std::cout << "9. TypeScript: Use TypedArrays for true integer performance\n";
    std::cout << "10. Fixed-width types (int32_t) over platform types (int)\n";

    return 0;
}
