/*
 * Performance Macros: Macro vs Inline Functions
 * 
 * Demonstrates the trade-offs between macros and inline functions,
 * including performance considerations, type safety, and debugging support.
 */

#include <stdio.h>
#include <stdint.h>
#include <time.h>

// Macro version - no function call overhead, but no type checking
#define MAX_MACRO(a, b) ((a) > (b) ? (a) : (b))

// Inline function version - type checking, debugging support
static inline int max_inline_int(int a, int b) {
    return a > b ? a : b;
}

static inline double max_inline_double(double a, double b) {
    return a > b ? a : b;
}

// Macro with statement expression (GCC extension) - avoids multiple evaluation
#ifdef __GNUC__
#define MAX_SAFE_MACRO(a, b) ({ \
    typeof(a) _a = (a); \
    typeof(b) _b = (b); \
    _a > _b ? _a : _b; \
})
#else
#define MAX_SAFE_MACRO(a, b) ((a) > (b) ? (a) : (b))
#endif

// Compile-time constant macro
#define CACHE_LINE_SIZE 64
#define ALIGN_TO_CACHE(x) (((x) + CACHE_LINE_SIZE - 1) & ~(CACHE_LINE_SIZE - 1))

// Performance-critical macro for hot paths
#define FAST_INCREMENT(ptr) (++(*(ptr)))

// Benchmark helper
static double get_time(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main(void) {
    const int iterations = 100000000;
    int a = 10, b = 20;
    double start, end;
    
    // Benchmark macro version
    start = get_time();
    for (int i = 0; i < iterations; i++) {
        int result = MAX_MACRO(a, b);
        (void)result;
    }
    end = get_time();
    printf("Macro version: %.6f seconds\n", end - start);
    
    // Benchmark inline function version
    start = get_time();
    for (int i = 0; i < iterations; i++) {
        int result = max_inline_int(a, b);
        (void)result;
    }
    end = get_time();
    printf("Inline function version: %.6f seconds\n", end - start);
    
    // Demonstrate type safety difference
    int int_val = 10;
    double double_val = 20.5;
    
    // Macro accepts mixed types (may cause issues)
    double macro_result = MAX_MACRO(int_val, double_val);
    printf("Macro with mixed types: %.2f\n", macro_result);
    
    // Inline function provides type safety
    // double func_result = max_inline_int(int_val, double_val);  // Compile error
    
    // Demonstrate compile-time evaluation
    size_t size = 100;
    size_t aligned = ALIGN_TO_CACHE(size);
    printf("Size %zu aligned to cache line: %zu\n", size, aligned);
    
    // Demonstrate safe macro (avoids multiple evaluation)
    int counter = 0;
    int result_safe = MAX_SAFE_MACRO(++counter, 5);
    printf("Safe macro result: %d, counter: %d\n", result_safe, counter);
    
    counter = 0;
    int result_unsafe = MAX_MACRO(++counter, 5);  // Counter incremented twice!
    printf("Unsafe macro result: %d, counter: %d\n", result_unsafe, counter);
    
    // Fast increment macro for hot paths
    int value = 0;
    FAST_INCREMENT(&value);
    printf("After fast increment: %d\n", value);
    
    return 0;
}

