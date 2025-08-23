#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "../custom_lib/custom_write.h"
#include "../custom_lib/custom_printf.h"

int main () {
    // Standard printf demonstration
    // Uses C library, handles formatting, buffering, and is portable
    printf("Hello, World!\n");

    // System call write demonstration
    // Directly interacts with the OS, bypasses C library buffering/formatting
    write(STDOUT_FILENO, "Hello, World!\n", strlen("Hello, World!\n"));

    // Custom write demonstration (robust, production-grade)
    my_write(STDOUT_FILENO, "Custom write: Hello, World!\n", strlen("Custom write: Hello, World!\n"));

    // Custom printf demonstration (Google-grade)
    my_printf("Custom printf: %s %d %f\n", "Number:", 42, 3.14159);

    return 0;
}