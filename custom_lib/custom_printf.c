// custom_printf.c
// Implementation of Google-grade custom printf
// References: ISO C Standard, Google C++ Style Guide

#include "custom_printf.h"
#include "printf_parser.h"
#include "buffer_manager.h"
#include "formatter.h"
#include <stdarg.h>
#include <unistd.h>
#include <string.h>

#define MAX_SPECS 128

int my_printf(const char *fmt, ...) {
    buffer_t buf;
    buffer_init(&buf);
    format_spec_t specs[MAX_SPECS];
    size_t spec_count = parse_format_string(fmt, specs, MAX_SPECS);
    va_list args;
    va_start(args, fmt);
    const char *p = fmt;
    size_t spec_idx = 0;
    while (*p) {
        if (*p == '%') {
            p++;
            // Skip flags, width, precision, length modifiers
            while (*p && !strchr("diuoxXfFeEgGaAcspn%", *p)) p++;
            if (*p) p++; // Skip specifier
            // Format argument
            char temp[512];
            size_t len = format_arg(temp, sizeof(temp), &specs[spec_idx++], &args);
            buffer_write(&buf, temp, len, STDOUT_FILENO);
        } else {
            buffer_write(&buf, p, 1, STDOUT_FILENO);
            p++;
        }
    }
    va_end(args);
    buffer_flush(&buf, STDOUT_FILENO);
    return 0;
}
