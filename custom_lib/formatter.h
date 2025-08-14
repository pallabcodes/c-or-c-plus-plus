// formatter.h
// Type conversion and formatting for custom printf
// References: ISO C Standard, Google C++ Style Guide

#ifndef FORMATTER_H
#define FORMATTER_H

#include "printf_parser.h"
#include <stdarg.h>
#include <stddef.h>

// Format a single argument according to format_spec_t
// Returns number of bytes written to out
size_t format_arg(char *out, size_t out_size, const format_spec_t *spec, va_list *args);

#endif // FORMATTER_H
