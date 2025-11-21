/*
 * Advanced Macros: Token Pasting (##)
 * 
 * Demonstrates the ## operator for concatenating tokens to create
 * new identifiers. Useful for code generation and generic programming.
 */

#include <stdio.h>
#include <stdint.h>

// Basic token pasting
#define CONCAT(a, b) a##b

// Token pasting for variable names
#define DECLARE_VAR(type, name) type var_##name
#define DEFINE_VAR(type, name, value) type var_##name = value

// Token pasting for function names
#define FUNC_NAME(prefix, suffix) prefix##_##suffix

// Token pasting for type names
#define TYPE_NAME(prefix, suffix) prefix##_##suffix##_t

// Token pasting for enum values
#define ENUM_VALUE(prefix, name) prefix##_##name

// Token pasting for structure members
#define MEMBER_NAME(struct_name, member) struct_name##_##member

// Token pasting for creating getter/setter functions
#define GETTER(type, name) \
    static inline type get_##name(void) { return name; }

#define SETTER(type, name) \
    static inline void set_##name(type value) { name = value; }

// Token pasting for creating multiple related functions
#define CREATE_FUNCS(prefix) \
    static void prefix##_init(void) { printf(#prefix " initialized\n"); } \
    static void prefix##_cleanup(void) { printf(#prefix " cleaned up\n"); }

// Helper variables for getter/setter demo
static int counter = 0;
static float temperature = 25.5f;

GETTER(int, counter)
SETTER(int, counter)
GETTER(float, temperature)
SETTER(float, temperature)

// Create functions using token pasting
CREATE_FUNCS(module)

int main(void) {
    // Basic token pasting
    int var1 = 10, var2 = 20;
    int CONCAT(var, 1) = 100;  // Creates var1
    printf("var1 = %d\n", var1);
    
    // Declare variables using token pasting
    DECLARE_VAR(int, counter);
    var_counter = 42;
    printf("var_counter = %d\n", var_counter);
    
    DEFINE_VAR(int, value, 99);
    printf("var_value = %d\n", var_value);
    
    // Function name generation
    typedef int (*FUNC_NAME(math, add))(int, int);
    
    // Type name generation
    typedef uint32_t TYPE_NAME(u, int32);
    TYPE_NAME(u, int32) my_uint = 42;
    printf("my_uint = %u\n", my_uint);
    
    // Enum value generation
    enum {
        ENUM_VALUE(STATUS, OK),
        ENUM_VALUE(STATUS, ERROR),
        ENUM_VALUE(STATUS, PENDING)
    };
    printf("STATUS_OK = %d\n", STATUS_OK);
    
    // Getter/setter usage
    printf("counter = %d\n", get_counter());
    set_counter(100);
    printf("counter = %d\n", get_counter());
    
    printf("temperature = %.1f\n", get_temperature());
    set_temperature(30.0f);
    printf("temperature = %.1f\n", get_temperature());
    
    // Generated functions
    module_init();
    module_cleanup();
    
    // Token pasting for creating multiple related identifiers
    #define CREATE_VARS(base) \
        int base##_a = 1; \
        int base##_b = 2; \
        int base##_c = 3;
    
    CREATE_VARS(test)
    printf("test_a = %d, test_b = %d, test_c = %d\n", test_a, test_b, test_c);
    
    return 0;
}

