/**
 * Basic Macro Examples - JavaScript/TypeScript Developer Edition
 *
 * Macros are preprocessor directives that perform text substitution before compilation.
 * Think of them as "find and replace" operations that happen before your code compiles.
 *
 * In JS/TS, macros don't exist natively, but you can think of them as:
 * - Build-time code generation (like Babel transforms)
 * - Template literals that get replaced before execution
 * - Constants that are inlined everywhere (but without type checking)
 *
 * Key differences from JS/TS:
 * - Macros are text replacement (no type checking)
 * - Macros are global (no scope)
 * - Macros are compile-time only (don't exist at runtime)
 * - Macros can cause unexpected behavior if not careful
 */

#include <iostream>
#include <string>
#include <vector>

// =============================================================================
// 1. OBJECT-LIKE MACROS (Simple Constants)
// =============================================================================
// In JS/TS: const PI = 3.14159;
// In C++: #define PI 3.14159 (but it's text replacement, not a constant!)

// Simple constant macros
#define PI 3.141592653589793
#define MAX_SIZE 1024
#define COMPANY_NAME "Bloomberg"
#define VERSION_MAJOR 1
#define VERSION_MINOR 0

// In JS/TS, you'd write:
// const PI = 3.141592653589793;
// const MAX_SIZE = 1024;
// const COMPANY_NAME = "Bloomberg";

void demonstrate_object_like_macros() {
    std::cout << "\n=== Object-Like Macros (Constants) ===\n";

    // Macros are replaced with their values before compilation
    double area = PI * 10.0 * 10.0;  // PI is replaced with 3.14159...
    std::cout << "Area of circle (r=10): " << area << std::endl;

    int buffer[MAX_SIZE];  // MAX_SIZE is replaced with 1024
    std::cout << "Buffer size: " << MAX_SIZE << std::endl;

    std::cout << "Company: " << COMPANY_NAME << std::endl;
    std::cout << "Version: " << VERSION_MAJOR << "." << VERSION_MINOR << std::endl;

    // Important: Macros don't have types!
    // In JS/TS: const PI: number = 3.14159; (type-checked)
    // In C++: #define PI 3.14159 (just text, no type checking)
}

// =============================================================================
// 2. FUNCTION-LIKE MACROS
// =============================================================================
// In JS/TS: function max(a, b) { return a > b ? a : b; }
// In C++: #define MAX(a, b) ((a) > (b) ? (a) : (b))
// But macros are text replacement, not function calls!

// Simple function-like macros
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define SQUARE(x) ((x) * (x))
#define ABS(x) ((x) < 0 ? -(x) : (x))

// In JS/TS, you'd write:
// function max(a, b) { return a > b ? a : b; }
// const square = x => x * x;

void demonstrate_function_like_macros() {
    std::cout << "\n=== Function-Like Macros ===\n";

    int a = 10, b = 20;
    int max_val = MAX(a, b);  // Expands to: ((a) > (b) ? (a) : (b))
    std::cout << "MAX(10, 20) = " << max_val << std::endl;

    int min_val = MIN(a, b);
    std::cout << "MIN(10, 20) = " << min_val << std::endl;

    int x = 5;
    int squared = SQUARE(x);  // Expands to: ((x) * (x))
    std::cout << "SQUARE(5) = " << squared << std::endl;

    int negative = -42;
    int absolute = ABS(negative);  // Expands to: ((negative) < 0 ? -(negative) : (negative))
    std::cout << "ABS(-42) = " << absolute << std::endl;

    // Note: Parentheses are CRITICAL!
    // Without them: SQUARE(3 + 2) would expand to: 3 + 2 * 3 + 2 = 11 (wrong!)
    // With them: SQUARE(3 + 2) expands to: ((3 + 2) * (3 + 2)) = 25 (correct!)
    int result = SQUARE(3 + 2);
    std::cout << "SQUARE(3 + 2) = " << result << " (should be 25)" << std::endl;
}

// =============================================================================
// 3. WHY PARENTHESES MATTER
// =============================================================================

// BAD: Missing parentheses
#define BAD_SQUARE(x) x * x

// GOOD: Properly parenthesized
#define GOOD_SQUARE(x) ((x) * (x))

void demonstrate_parentheses_importance() {
    std::cout << "\n=== Why Parentheses Matter ===\n";

    // BAD macro without parentheses
    int bad_result = BAD_SQUARE(3 + 2);
    // Expands to: 3 + 2 * 3 + 2 = 3 + 6 + 2 = 11 (WRONG!)
    std::cout << "BAD_SQUARE(3 + 2) = " << bad_result << " (WRONG! Should be 25)" << std::endl;

    // GOOD macro with parentheses
    int good_result = GOOD_SQUARE(3 + 2);
    // Expands to: ((3 + 2) * (3 + 2)) = 25 (CORRECT!)
    std::cout << "GOOD_SQUARE(3 + 2) = " << good_result << " (CORRECT!)" << std::endl;

    // In JS/TS, this isn't an issue because functions evaluate arguments first:
    // const square = x => x * x;
    // square(3 + 2);  // Evaluates 3 + 2 = 5, then 5 * 5 = 25
}

// =============================================================================
// 4. MULTIPLE EVALUATION PROBLEM
// =============================================================================
// This is a CRITICAL difference from JS/TS functions!

void demonstrate_multiple_evaluation() {
    std::cout << "\n=== Multiple Evaluation Problem ===\n";

    // In JS/TS: function max(a, b) { return a > b ? a : b; }
    // When you call max(++i, 10), the ++i is evaluated ONCE

    // In C++ macros: #define MAX(a, b) ((a) > (b) ? (a) : (b))
    // When you call MAX(++i, 10), the ++i is evaluated MULTIPLE TIMES!

    int i = 5;
    int original_i = i;

    // Using macro - DANGEROUS!
    int macro_result = MAX(++i, 10);
    // Expands to: ((++i) > (10) ? (++i) : (10))
    // First ++i evaluates: i becomes 6, expression is 6
    // Then compares: 6 > 10? No, so evaluates (10)
    // But if it was true, it would evaluate (++i) AGAIN!
    std::cout << "After MAX(++i, 10) with macro:" << std::endl;
    std::cout << "  i = " << i << " (incremented once in this case)" << std::endl;
    std::cout << "  result = " << macro_result << std::endl;

    // Reset and show the problem case
    i = 5;
    int macro_result2 = MAX(++i, 3);
    // Expands to: ((++i) > (3) ? (++i) : (3))
    // First ++i: i becomes 6, expression is 6
    // 6 > 3? Yes, so evaluates (++i) AGAIN: i becomes 7
    std::cout << "After MAX(++i, 3) with macro:" << std::endl;
    std::cout << "  i = " << i << " (incremented TWICE!)" << std::endl;
    std::cout << "  result = " << macro_result2 << std::endl;

    // In JS/TS, this doesn't happen:
    // function max(a, b) { return a > b ? a : b; }
    // let i = 5;
    // max(++i, 3);  // ++i evaluated once, i becomes 6, result is 6
}

// =============================================================================
// 5. STRINGIFICATION (#)
// =============================================================================
// Converts macro arguments to string literals
// In JS/TS: You'd use template literals: `Variable name: ${variableName}`

#define STRINGIFY(x) #x
#define PRINT_VAR(x) std::cout << #x << " = " << x << std::endl
#define DEBUG_PRINT(x) std::cout << "[DEBUG] " << #x << " = " << x << std::endl

void demonstrate_stringification() {
    std::cout << "\n=== Stringification (#) ===\n";

    int myVariable = 42;
    std::string varName = STRINGIFY(myVariable);
    std::cout << "Stringified: " << varName << std::endl;

    // Useful for debugging
    int counter = 100;
    PRINT_VAR(counter);  // Outputs: counter = 100

    double price = 150.25;
    DEBUG_PRINT(price);  // Outputs: [DEBUG] price = 150.25

    // In JS/TS, you'd write:
    // const myVariable = 42;
    // console.log(`Variable name: ${'myVariable'}`);  // Manual string
    // console.log(`myVariable = ${myVariable}`);      // Template literal
}

// =============================================================================
// 6. TOKEN CONCATENATION (##)
// =============================================================================
// Combines tokens into a single token
// In JS/TS: You'd use template literals or string concatenation

#define CONCAT(a, b) a##b
#define MAKE_VAR(name, type) type CONCAT(name, _var)
#define MAKE_FUNCTION(name) void CONCAT(print_, name)() { std::cout << #name << std::endl; }

void demonstrate_token_concatenation() {
    std::cout << "\n=== Token Concatenation (##) ===\n";

    // Create variable names dynamically
    int counter_var;  // Created by: MAKE_VAR(counter, int)
    MAKE_VAR(price, double);  // Expands to: double price_var;
    price_var = 150.25;
    std::cout << "price_var = " << price_var << std::endl;

    // In JS/TS, you'd use object properties or template literals:
    // const vars = {};
    // vars[`${name}_var`] = value;
    // Or: const price_var = 150.25;  // Just name it directly
}

// =============================================================================
// 7. MULTI-LINE MACROS
// =============================================================================
// Use backslash (\) to continue macro definition on next line
// In JS/TS: You'd use a function with multiple statements

#define SWAP(a, b) \
    do { \
        auto temp = (a); \
        (a) = (b); \
        (b) = temp; \
    } while(0)

#define PRINT_PAIR(a, b) \
    do { \
        std::cout << "First: " << (a) << ", Second: " << (b) << std::endl; \
    } while(0)

void demonstrate_multiline_macros() {
    std::cout << "\n=== Multi-Line Macros ===\n";

    int x = 10, y = 20;
    std::cout << "Before swap: x = " << x << ", y = " << y << std::endl;
    SWAP(x, y);
    std::cout << "After swap: x = " << x << ", y = " << y << std::endl;

    PRINT_PAIR(100, 200);

    // The do-while(0) pattern ensures the macro can be used like a statement
    // In JS/TS, you'd just write:
    // function swap(a, b) {
    //     const temp = a;
    //     a = b;
    //     b = temp;
    // }
    // But C++ macros need this pattern for safety
}

// =============================================================================
// 8. UNDEFINING MACROS
// =============================================================================
// You can undefine macros when done with them
// In JS/TS: You can't "undefine" constants, but you can use scoped variables

#define TEMP_MACRO(x) ((x) * 2)

void demonstrate_undefining() {
    std::cout << "\n=== Undefining Macros ===\n";

    int value = TEMP_MACRO(5);  // Uses TEMP_MACRO
    std::cout << "TEMP_MACRO(5) = " << value << std::endl;

    // Undefine the macro
    #undef TEMP_MACRO

    // Now TEMP_MACRO is no longer defined
    // TEMP_MACRO(5);  // This would cause a compilation error
    std::cout << "TEMP_MACRO has been undefined" << std::endl;

    // In JS/TS, you'd use block scope:
    // {
    //     const TEMP_MACRO = x => x * 2;
    //     const value = TEMP_MACRO(5);
    // }
    // // TEMP_MACRO is out of scope here
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "C++ Macros Basic Examples - JS/TS Developer Edition\n";
    std::cout << "====================================================\n";

    demonstrate_object_like_macros();
    demonstrate_function_like_macros();
    demonstrate_parentheses_importance();
    demonstrate_multiple_evaluation();
    demonstrate_stringification();
    demonstrate_token_concatenation();
    demonstrate_multiline_macros();
    demonstrate_undefining();

    std::cout << "\n=== Key Takeaways for JS/TS Developers ===\n";
    std::cout << "1. Macros = Text replacement before compilation (no type checking)\n";
    std::cout << "2. Object-like macros = Constants (but without type safety)\n";
    std::cout << "3. Function-like macros = Functions (but with text substitution)\n";
    std::cout << "4. ALWAYS parenthesize macro parameters and results\n";
    std::cout << "5. Macros can evaluate arguments multiple times (unlike functions)\n";
    std::cout << "6. Stringification (#) = Converting to string literal\n";
    std::cout << "7. Token concatenation (##) = Combining tokens\n";
    std::cout << "8. Use do-while(0) for multi-line macros\n";
    std::cout << "9. Prefer modern C++ alternatives (constexpr, inline functions)\n";
    std::cout << "10. Macros are global - use #undef when done\n";

    return 0;
}
