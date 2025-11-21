/*
 * Enterprise Macros: Linux Kernel Patterns
 * 
 * Demonstrates production macro patterns from the Linux kernel,
 * including container_of, ARRAY_SIZE, BUILD_BUG_ON, and min/max macros.
 * These patterns are production-tested in billions of devices.
 */

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Linux kernel container_of macro pattern
// Get containing structure from member pointer
#define container_of(ptr, type, member) ({ \
    const typeof(((type *)0)->member) *__mptr = (ptr); \
    (type *)((char *)__mptr - offsetof(type, member)); \
})

// Linux kernel ARRAY_SIZE macro
// Compile-time array size calculation
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

// Linux kernel BUILD_BUG_ON macro pattern
// Compile-time assertion - fails compilation if condition is true
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))

// Linux kernel min/max macros with type safety
#define min(x, y) ({ \
    typeof(x) _x = (x); \
    typeof(y) _y = (y); \
    (void)(&_x == &_y); \
    _x < _y ? _x : _y; \
})

#define max(x, y) ({ \
    typeof(x) _x = (x); \
    typeof(y) _y = (y); \
    (void)(&_x == &_y); \
    _x > _y ? _x : _y; \
})

// Linux kernel likely/unlikely macros for branch prediction
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

// Linux kernel alignment macros
#define ALIGN(x, a) (((x) + (a) - 1) & ~((a) - 1))
#define ALIGN_DOWN(x, a) ((x) & ~((a) - 1))
#define IS_ALIGNED(x, a) (((x) & ((a) - 1)) == 0)

// Example structure for container_of demonstration
struct list_node {
    struct list_node *next;
    struct list_node *prev;
};

struct my_struct {
    int data;
    struct list_node node;
    char name[32];
};

int main(void) {
    // Demonstrate ARRAY_SIZE
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    printf("Array size: %zu\n", ARRAY_SIZE(arr));
    
    // Demonstrate BUILD_BUG_ON (compile-time check)
    // Uncomment to see compilation error:
    // BUILD_BUG_ON(sizeof(int) != 4);  // Fails if int is not 4 bytes
    
    // Demonstrate min/max with type checking
    int a = 10, b = 20;
    printf("min(%d, %d) = %d\n", a, b, min(a, b));
    printf("max(%d, %d) = %d\n", a, b, max(a, b));
    
    // Demonstrate container_of pattern
    struct my_struct obj = {
        .data = 42,
        .name = "test"
    };
    
    // Get pointer to containing structure from member pointer
    struct list_node *node_ptr = &obj.node;
    struct my_struct *container = container_of(node_ptr, struct my_struct, node);
    
    printf("Original data: %d\n", obj.data);
    printf("Container data: %d\n", container->data);
    printf("Pointers match: %s\n", (container == &obj) ? "yes" : "no");
    
    // Demonstrate alignment macros
    size_t addr = 100;
    size_t aligned_addr = ALIGN(addr, 16);
    printf("Address %zu aligned to 16: %zu\n", addr, aligned_addr);
    printf("Is aligned: %s\n", IS_ALIGNED(aligned_addr, 16) ? "yes" : "no");
    
    // Demonstrate likely/unlikely for branch prediction
    int value = 1;
    if (likely(value > 0)) {
        printf("Likely branch taken\n");
    }
    
    if (unlikely(value < 0)) {
        printf("Unlikely branch (should not print)\n");
    }
    
    return 0;
}

