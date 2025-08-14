// printf_parser.h
// Google-grade format string parser for custom printf
// References: ISO C Standard, ACM papers on parsing

#ifndef PRINTF_PARSER_H
#define PRINTF_PARSER_H

#include <stddef.h>

// Format specifier structure
typedef struct {
    char flag_minus;
    char flag_plus;
    char flag_space;
    char flag_zero;
    char flag_hash;
    int width;
    int precision;
    char length_modifier[3]; // e.g., l, ll, h, hh
    char specifier; // d, s, f, etc.
} format_spec_t;

// Parse a format string and fill an array of format_spec_t
size_t parse_format_string(const char *fmt, format_spec_t *specs, size_t max_specs);

#endif // PRINTF_PARSER_H
