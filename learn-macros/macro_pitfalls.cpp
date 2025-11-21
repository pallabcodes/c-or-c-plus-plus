/**
 * Macro Pitfalls and Gotchas - JavaScript/TypeScript Developer Edition
 *
 * This file demonstrates common macro mistakes and how to avoid them.
 * Understanding these pitfalls is crucial for Bloomberg-level C++ development.
 *
 * In JS/TS, many of these issues don't exist because:
 * - Functions evaluate arguments once
 * - Type checking prevents many errors
 * - Scoping prevents name collisions
 * - No text substitution means no operator precedence issues
 *
 * These examples show what NOT to do and why!
 */

#include <iostream>
#include <string>

// =============================================================================
// PITFALL 1: MISSING PARENTHESES
// =============================================================================
// CRITICAL: Always parenthesize macro parameters and results!

// BAD: Missing parentheses
#define BAD_SQUARE(x) x * x
#define BAD_MAX(a, b) a > b ? a : b
#define BAD_DIVIDE(a, b) a / b

// GOOD: Properly parenthesized
#define GOOD_SQUARE(x) ((x) * (x))
#define GOOD_MAX(a, b) ((a) > (b) ? (a) : (b))
#define GOOD_DIVIDE(a, b) ((a) / (b))

void demonstrate_missing_parentheses() {
    std::cout << "\n=== Pitfall 1: Missing Parentheses ===\n";

    // BAD example
    int bad_result = BAD_SQUARE(3 + 2);
    // Expands to: 3 + 2 * 3 + 2 = 3 + 6 + 2 = 11 (WRONG!)
    std::cout << "BAD_SQUARE(3 + 2) = " << bad_result << " (WRONG! Should be 25)" << std::endl;

    // GOOD example
    int good_result = GOOD_SQUARE(3 + 2);
    // Expands to: ((3 + 2) * (3 + 2)) = 25 (CORRECT!)
    std::cout << "GOOD_SQUARE(3 + 2) = " << good_result << " (CORRECT!)" << std::endl;

    // BAD division example
    int bad_div = BAD_DIVIDE(10 + 5, 2 + 1);
    // Expands to: 10 + 5 / 2 + 1 = 10 + 2 + 1 = 13 (WRONG!)
    std::cout << "BAD_DIVIDE(10 + 5, 2 + 1) = " << bad_div << " (WRONG! Should be 5)" << std::endl;

    // GOOD division example
    int good_div = GOOD_DIVIDE(10 + 5, 2 + 1);
    // Expands to: ((10 + 5) / (2 + 1)) = 5 (CORRECT!)
    std::cout << "GOOD_DIVIDE(10 + 5, 2 + 1) = " << good_div << " (CORRECT!)" << std::endl;

    // In JS/TS, this isn't an issue:
    // const square = x => x * x;
    // square(3 + 2);  // Evaluates 3 + 2 = 5 first, then 5 * 5 = 25
}

// =============================================================================
// PITFALL 2: MULTIPLE EVALUATION
// =============================================================================
// Macros can evaluate arguments multiple times!

#define MAX(a, b) ((a) > (b) ? (a) : (b))

void demonstrate_multiple_evaluation() {
    std::cout << "\n=== Pitfall 2: Multiple Evaluation ===\n";

    int i = 5;
    int original_i = i;

    // DANGEROUS: Macro evaluates ++i multiple times
    int result = MAX(++i, 3);
    // Expands to: ((++i) > (3) ? (++i) : (3))
    // First ++i: i becomes 6, expression is 6
    // 6 > 3? Yes, so evaluates (++i) AGAIN: i becomes 7
    std::cout << "After MAX(++i, 3) with macro:" << std::endl;
    std::cout << "  i = " << i << " (incremented TWICE!)" << std::endl;
    std::cout << "  result = " << result << std::endl;

    // In JS/TS, this doesn't happen:
    // function max(a, b) { return a > b ? a : b; }
    // let i = 5;
    // max(++i, 3);  // ++i evaluated once, i becomes 6, result is 6

    // Solution: Use inline function instead
    auto safe_max = [](int a, int b) { return a > b ? a : b; };
    i = 5;
    int safe_result = safe_max(++i, 3);
    std::cout << "After safe_max(++i, 3) with function:" << std::endl;
    std::cout << "  i = " << i << " (incremented ONCE)" << std::endl;
    std::cout << "  result = " << safe_result << std::endl;
}

// =============================================================================
// PITFALL 3: SIDE EFFECTS
// =============================================================================
// Macros can have unexpected side effects

#define PRINT_AND_INCREMENT(x) (std::cout << (x)++, (x))

void demonstrate_side_effects() {
    std::cout << "\n=== Pitfall 3: Side Effects ===\n";

    int value = 5;
    std::cout << "Before: value = " << value << std::endl;

    // Using macro with side effects
    int result = PRINT_AND_INCREMENT(value);
    // Expands to: (std::cout << (value)++, (value))
    // This increments value AND prints it
    std::cout << "\nAfter PRINT_AND_INCREMENT(value):" << std::endl;
    std::cout << "  value = " << value << std::endl;
    std::cout << "  result = " << result << std::endl;

    // In JS/TS, you'd write:
    // function printAndIncrement(x) {
    //     console.log(x);
    //     return ++x;
    // }
    // const result = printAndIncrement(value);
    // This is clearer and more predictable
}

// =============================================================================
// PITFALL 4: OPERATOR PRECEDENCE ISSUES
// =============================================================================
// Macros don't respect operator precedence

#define BAD_ADD_MULTIPLY(a, b, c) a + b * c
#define GOOD_ADD_MULTIPLY(a, b, c) ((a) + (b) * (c))

void demonstrate_operator_precedence() {
    std::cout << "\n=== Pitfall 4: Operator Precedence ===\n";

    int result1 = BAD_ADD_MULTIPLY(1, 2, 3);
    // Expands to: 1 + 2 * 3 = 1 + 6 = 7
    std::cout << "BAD_ADD_MULTIPLY(1, 2, 3) = " << result1 << std::endl;

    int result2 = GOOD_ADD_MULTIPLY(1, 2, 3);
    // Expands to: ((1) + (2) * (3)) = 7 (same in this case, but safer)
    std::cout << "GOOD_ADD_MULTIPLY(1, 2, 3) = " << result2 << std::endl;

    // But watch out for more complex cases
    int result3 = BAD_ADD_MULTIPLY(1 + 2, 3 + 4, 5 + 6);
    // Expands to: 1 + 2 + 3 + 4 * 5 + 6 = 3 + 7 * 11 = 3 + 77 = 80 (WRONG!)
    std::cout << "BAD_ADD_MULTIPLY(1+2, 3+4, 5+6) = " << result3 << " (WRONG!)" << std::endl;

    int result4 = GOOD_ADD_MULTIPLY(1 + 2, 3 + 4, 5 + 6);
    // Expands to: ((1 + 2) + (3 + 4) * (5 + 6)) = 3 + 7 * 11 = 80 (still wrong logic!)
    std::cout << "GOOD_ADD_MULTIPLY(1+2, 3+4, 5+6) = " << result4 << std::endl;
    // Even with parentheses, the macro logic might be wrong!
    // Better to use a function: addMultiply(1+2, 3+4, 5+6)
}

// =============================================================================
// PITFALL 5: SCOPE ISSUES
// =============================================================================
// Macros are global and can't be scoped

#define DEBUG 1

void function1() {
    // DEBUG is visible here
    #ifdef DEBUG
        std::cout << "Function1: Debug enabled" << std::endl;
    #endif
}

void function2() {
    // DEBUG is still visible here - can't have local "DEBUG"
    // You can't shadow macros like you can shadow variables
    #ifdef DEBUG
        std::cout << "Function2: Debug enabled" << std::endl;
    #endif
}

void demonstrate_scope_issues() {
    std::cout << "\n=== Pitfall 5: Scope Issues ===\n";

    function1();
    function2();

    // In JS/TS, you can scope constants:
    // function function1() {
    //     const DEBUG = true;
    //     if (DEBUG) console.log("Debug enabled");
    // }
    // function function2() {
    //     const DEBUG = false;  // Can shadow the outer DEBUG
    //     if (DEBUG) console.log("Debug enabled");
    // }
}

// =============================================================================
// PITFALL 6: TYPE SAFETY
// =============================================================================
// Macros don't have type checking

#define UNSAFE_ADD(a, b) ((a) + (b))

void demonstrate_type_safety() {
    std::cout << "\n=== Pitfall 6: Type Safety ===\n";

    // These all compile, but might not make sense:
    int result1 = UNSAFE_ADD(5, 10);        // OK: int + int
    double result2 = UNSAFE_ADD(5.5, 10.2); // OK: double + double
    int result3 = UNSAFE_ADD(5, 10.5);      // OK: int + double (implicit conversion)

    std::cout << "UNSAFE_ADD(5, 10) = " << result1 << std::endl;
    std::cout << "UNSAFE_ADD(5.5, 10.2) = " << result2 << std::endl;
    std::cout << "UNSAFE_ADD(5, 10.5) = " << result3 << std::endl;

    // In JS/TS, you'd write:
    // function add(a: number, b: number): number { return a + b; }
    // TypeScript would catch type errors at compile time
}

// =============================================================================
// PITFALL 7: MACRO NAME COLLISIONS
// =============================================================================
// Macros can collide with function names or other macros

#define min(a, b) ((a) < (b) ? (a) : (b))

// This would conflict with std::min!
// Using std::min would fail because the macro replaces it

void demonstrate_name_collisions() {
    std::cout << "\n=== Pitfall 7: Name Collisions ===\n";

    int a = 5, b = 10;
    int result = min(a, b);  // Uses our macro, not std::min
    std::cout << "min(5, 10) = " << result << std::endl;

    // std::min(a, b);  // This would fail! Macro replaces std::min

    // Solution: Use uppercase names for macros
    #define MIN(a, b) ((a) < (b) ? (a) : (b))
    // Now std::min works fine

    // In JS/TS, you'd use:
    // function min(a, b) { return a < b ? a : b; }
    // Math.min(a, b);  // No conflict - different namespaces
}

// =============================================================================
// PITFALL 8: COMPLEX EXPRESSIONS IN MACROS
// =============================================================================
// Macros with complex logic are hard to debug

#define COMPLEX_MACRO(x, y, z) \
    do { \
        if ((x) > (y)) { \
            if ((y) > (z)) { \
                std::cout << "x > y > z" << std::endl; \
            } else { \
                std::cout << "x > y, but y <= z" << std::endl; \
            } \
        } else { \
            std::cout << "x <= y" << std::endl; \
        } \
    } while(0)

void demonstrate_complex_expressions() {
    std::cout << "\n=== Pitfall 8: Complex Expressions ===\n";

    COMPLEX_MACRO(10, 5, 2);
    COMPLEX_MACRO(5, 10, 2);
    COMPLEX_MACRO(10, 5, 8);

    // Problem: Hard to debug, no type checking, no single-step debugging
    // Better: Use a function
    // In JS/TS, you'd write:
    // function complexLogic(x, y, z) {
    //     if (x > y) {
    //         if (y > z) {
    //             console.log("x > y > z");
    //         } else {
    //             console.log("x > y, but y <= z");
    //         }
    //     } else {
    //         console.log("x <= y");
    //     }
    // }
}

// =============================================================================
// PITFALL 9: MACRO EXPANSION IN COMMENTS
// =============================================================================
// Macros can expand in unexpected places

#define BAD_MACRO_NAME DEBUG

void demonstrate_macro_in_comments() {
    std::cout << "\n=== Pitfall 9: Macro Expansion Issues ===\n";

    // This comment contains BAD_MACRO_NAME - but it won't expand in comments
    // However, be careful with string literals:
    std::string message = "BAD_MACRO_NAME is not expanded here";
    std::cout << message << std::endl;

    // Macros don't expand in string literals or comments
    // But they DO expand in code, which can be confusing
}

// =============================================================================
// PITFALL 10: UNDEFINED BEHAVIOR
// =============================================================================
// Using undefined macros can cause compilation errors

void demonstrate_undefined_behavior() {
    std::cout << "\n=== Pitfall 10: Undefined Macros ===\n";

    // This would cause a compilation error if UNDEFINED_MACRO is not defined:
    // int value = UNDEFINED_MACRO;

    // Check if macro is defined before using
    #ifdef UNDEFINED_MACRO
        std::cout << "UNDEFINED_MACRO is defined" << std::endl;
    #else
        std::cout << "UNDEFINED_MACRO is not defined" << std::endl;
    #endif

    // In JS/TS, you'd use:
    // if (typeof UNDEFINED_CONSTANT !== 'undefined') {
    //     console.log("Constant is defined");
    // }
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "Macro Pitfalls and Gotchas - JS/TS Developer Edition\n";
    std::cout << "====================================================\n";

    demonstrate_missing_parentheses();
    demonstrate_multiple_evaluation();
    demonstrate_side_effects();
    demonstrate_operator_precedence();
    demonstrate_scope_issues();
    demonstrate_type_safety();
    demonstrate_name_collisions();
    demonstrate_complex_expressions();
    demonstrate_macro_in_comments();
    demonstrate_undefined_behavior();

    std::cout << "\n=== Critical Macro Pitfalls to Avoid ===\n";
    std::cout << "1. ALWAYS parenthesize macro parameters and results\n";
    std::cout << "2. Macros evaluate arguments multiple times (unlike functions)\n";
    std::cout << "3. Side effects in macros can cause unexpected behavior\n";
    std::cout << "4. Operator precedence issues without parentheses\n";
    std::cout << "5. Macros are global - can't be scoped like variables\n";
    std::cout << "6. No type checking - macros accept any type\n";
    std::cout << "7. Name collisions with functions and other macros\n";
    std::cout << "8. Complex logic in macros is hard to debug\n";
    std::cout << "9. Macros don't expand in comments or string literals\n";
    std::cout << "10. Undefined macros cause compilation errors\n";

    std::cout << "\n=== Best Practices ===\n";
    std::cout << "• Use uppercase names for macros (MIN, MAX, not min, max)\n";
    std::cout << "• Always parenthesize parameters: ((x) * (x))\n";
    std::cout << "• Use do-while(0) for multi-line macros\n";
    std::cout << "• Prefer inline functions or templates when possible\n";
    std::cout << "• Document macros thoroughly\n";
    std::cout << "• Test macros with edge cases\n";
    std::cout << "• Use #undef when done with temporary macros\n";

    return 0;
}
