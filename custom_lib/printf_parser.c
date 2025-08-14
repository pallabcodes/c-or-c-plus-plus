// printf_parser.c
// Implementation of format string parser for custom printf
// References: ISO C Standard, ACM papers on parsing

#include "printf_parser.h"
#include <string.h>
#include <ctype.h>

size_t parse_format_string(const char *fmt, format_spec_t *specs, size_t max_specs) {
    // Robust format string parsing implementation
    size_t count = 0;
    const char *p = fmt;
    while (*p && count < max_specs) {
        if (*p == '%') {
            p++;
            format_spec_t spec = {0};
            // Parse flags
            while (*p == '-' || *p == '+' || *p == ' ' || *p == '0' || *p == '#') {
                switch (*p) {
                    case '-': spec.flag_minus = 1; break;
                    case '+': spec.flag_plus = 1; break;
                    case ' ': spec.flag_space = 1; break;
                    case '0': spec.flag_zero = 1; break;
                    case '#': spec.flag_hash = 1; break;
                }
                p++;
            }
            // Parse width
            if (isdigit((unsigned char)*p)) {
                spec.width = 0;
                while (isdigit((unsigned char)*p)) {
                    spec.width = spec.width * 10 + (*p - '0');
                    p++;
                }
            }
            // Parse precision
            if (*p == '.') {
                p++;
                spec.precision = 0;
                while (isdigit((unsigned char)*p)) {
                    spec.precision = spec.precision * 10 + (*p - '0');
                    p++;
                }
            } else {
                spec.precision = -1; // No precision specified
            }
            // Parse length modifier
            if (*p == 'h' && *(p+1) == 'h') {
                spec.length_modifier[0] = 'h';
                spec.length_modifier[1] = 'h';
                spec.length_modifier[2] = '\0';
                p += 2;
            } else if (*p == 'l' && *(p+1) == 'l') {
                spec.length_modifier[0] = 'l';
                spec.length_modifier[1] = 'l';
                spec.length_modifier[2] = '\0';
                p += 2;
            } else if (*p == 'h' || *p == 'l' || *p == 'L') {
                spec.length_modifier[0] = *p;
                spec.length_modifier[1] = '\0';
                p++;
            } else {
                spec.length_modifier[0] = '\0';
            }
            // Parse specifier
            if (strchr("diuoxXfFeEgGaAcspn%", *p)) {
                spec.specifier = *p;
                p++;
            } else {
                // Invalid specifier, skip
                continue;
            }
            specs[count++] = spec;
        } else {
            p++;
        }
    }
    return count;
}
