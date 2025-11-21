/*
 * Advanced Techniques: Type-Generic Macros (C11 _Generic)
 * 
 * Demonstrates C11 _Generic keyword for type-based dispatch in macros.
 * Provides type-safe generic operations without C++ templates.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <complex.h>

// Type-generic max macro using _Generic
#define type_max(x) _Generic((x), \
    int: max_int, \
    long: max_long, \
    double: max_double, \
    float: max_float, \
    default: max_generic \
)(x, x)  // Note: _Generic selects function, still need to handle comparison

// Helper functions for type-specific operations
static inline int max_int(int a, int b) {
    return a > b ? a : b;
}

static inline long max_long(long a, long b) {
    return a > b ? a : b;
}

static inline double max_double(double a, double b) {
    return a > b ? a : b;
}

static inline float max_float(float a, float b) {
    return a > b ? a : b;
}

static inline int max_generic(int a, int b) {
    return a > b ? a : b;
}

// Better type-generic max that actually compares two values
#define type_max2(a, b) _Generic((a), \
    int: ((a) > (b) ? (a) : (b)), \
    long: ((a) > (b) ? (a) : (b)), \
    double: ((a) > (b) ? (a) : (b)), \
    float: ((a) > (b) ? (a) : (b)), \
    default: ((a) > (b) ? (a) : (b)) \
)

// Type-generic print macro
#define type_print(x) _Generic((x), \
    int: printf("int: %d\n", x), \
    long: printf("long: %ld\n", x), \
    double: printf("double: %.2f\n", x), \
    float: printf("float: %.2f\n", x), \
    char*: printf("string: %s\n", x), \
    default: printf("unknown type\n") \
)

// Type-generic size macro
#define type_sizeof(x) _Generic((x), \
    int: sizeof(int), \
    long: sizeof(long), \
    double: sizeof(double), \
    float: sizeof(float), \
    char: sizeof(char), \
    default: sizeof(x) \
)

// Type-generic format specifier (returns string)
#define type_format(x) _Generic((x), \
    int: "%d", \
    long: "%ld", \
    double: "%.2f", \
    float: "%.2f", \
    char*: "%s", \
    default: "%p" \
)

// Type-generic square macro
#define type_square(x) _Generic((x), \
    int: ((x) * (x)), \
    long: ((x) * (x)), \
    double: ((x) * (x)), \
    float: ((x) * (x)), \
    default: ((x) * (x)) \
)

int main(void) {
    // Demonstrate type-generic max
    int a = 10, b = 20;
    printf("max(%d, %d) = %d\n", a, b, type_max2(a, b));
    
    double x = 3.14, y = 2.71;
    printf("max(%.2f, %.2f) = %.2f\n", x, y, type_max2(x, y));
    
    long l1 = 100L, l2 = 200L;
    printf("max(%ld, %ld) = %ld\n", l1, l2, type_max2(l1, l2));
    
    // Demonstrate type-generic print
    printf("\n=== Type-Generic Print ===\n");
    type_print(42);
    type_print(100L);
    type_print(3.14);
    type_print(2.71f);
    type_print("Hello, World!");
    
    // Demonstrate type-generic sizeof
    printf("\n=== Type Sizes ===\n");
    int i = 0;
    double d = 0.0;
    printf("Size of int: %zu\n", type_sizeof(i));
    printf("Size of double: %zu\n", type_sizeof(d));
    
    // Demonstrate type-generic square
    printf("\n=== Type-Generic Square ===\n");
    printf("Square of %d = %d\n", 5, type_square(5));
    printf("Square of %.2f = %.2f\n", 3.14, type_square(3.14));
    
    // Demonstrate format specifier
    printf("\n=== Format Specifiers ===\n");
    int val_int = 42;
    double val_double = 3.14159;
    printf("Formatted int: " type_format(val_int) "\n", val_int);
    printf("Formatted double: " type_format(val_double) "\n", val_double);
    
    return 0;
}

