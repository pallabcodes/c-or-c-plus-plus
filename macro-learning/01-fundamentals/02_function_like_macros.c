/*
 * Macro Fundamentals: Function-Like Macros
 * 
 * Demonstrates function-like macros with proper parenthesization
 * to prevent operator precedence issues.
 */

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

// Thread-safety: N/A (compile-time)
// Ownership: N/A (compile-time)
// Invariants: a and b must be comparable
// Failure modes: Multiple evaluation of parameters
// Side effects: Parameters evaluated twice
#define MIN(a, b) ((a) < (b) ? (a) : (b))

// Thread-safety: N/A (compile-time)
// Ownership: N/A (compile-time)
// Invariants: a and b must be comparable
// Failure modes: Multiple evaluation of parameters
// Side effects: Parameters evaluated twice
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// Thread-safety: N/A (compile-time)
// Ownership: N/A (compile-time)
// Invariants: x must be numeric
// Failure modes: None
#define ABS(x) ((x) < 0 ? -(x) : (x))

// Thread-safety: N/A (compile-time)
// Ownership: N/A (compile-time)
// Invariants: x must be numeric
// Failure modes: None
#define SQUARE(x) ((x) * (x))

// Thread-safety: N/A (compile-time)
// Ownership: N/A (compile-time)
// Invariants: x must be numeric, n must be non-negative
// Failure modes: Undefined behavior if n < 0
#define POWER(x, n) ((n) == 0 ? 1 : (n) == 1 ? (x) : (x) * POWER((x), (n) - 1))

// Thread-safety: N/A (compile-time)
// Ownership: N/A (compile-time)
// Invariants: arr must be array (not pointer)
// Failure modes: Undefined behavior if arr is pointer
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

// Thread-safety: N/A (compile-time)
// Ownership: N/A (compile-time)
// Invariants: ptr must be valid pointer, member must exist in type
// Failure modes: Undefined behavior if invalid
#define OFFSETOF(type, member) ((size_t)&(((type*)0)->member))

int main(void) {
    // Test MIN/MAX macros
    int a = 10, b = 20;
    printf("MIN(%d, %d) = %d\n", a, b, MIN(a, b));
    printf("MAX(%d, %d) = %d\n", a, b, MAX(a, b));
    
    // Test with expressions (demonstrates parenthesization importance)
    printf("MIN(%d, %d) = %d\n", a + 5, b - 5, MIN(a + 5, b - 5));
    
    // Test ABS macro
    int x = -42;
    printf("ABS(%d) = %d\n", x, ABS(x));
    printf("ABS(%d) = %d\n", -x, ABS(-x));
    
    // Test SQUARE macro
    int y = 7;
    printf("SQUARE(%d) = %d\n", y, SQUARE(y));
    printf("SQUARE(%d + 1) = %d\n", y, SQUARE(y + 1));
    
    // Test ARRAY_SIZE macro
    int arr[] = {1, 2, 3, 4, 5};
    printf("Array size: %zu\n", ARRAY_SIZE(arr));
    
    // Demonstrate structure offset calculation
    struct example {
        int a;
        char b;
        double c;
    };
    
    printf("Offset of 'b' in struct: %zu\n", OFFSETOF(struct example, b));
    printf("Offset of 'c' in struct: %zu\n", OFFSETOF(struct example, c));
    
    return 0;
}

