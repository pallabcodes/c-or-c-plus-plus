// formatter.c
// Implementation of type conversion and formatting for custom printf
// References: ISO C Standard, Google C++ Style Guide

#include "formatter.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

size_t format_arg(char *out, size_t out_size, const format_spec_t *spec, va_list *args) {
    // Robust formatting for supported specifiers
    size_t written = 0;
    char temp[512];
    temp[0] = '\0';
    switch (spec->specifier) {
        case 'd':
        case 'i': {
            int val = va_arg(*args, int);
            snprintf(temp, sizeof(temp), "%*.*d", spec->width, spec->precision >= 0 ? spec->precision : 0, val);
            break;
        }
        case 'u': {
            unsigned int val = va_arg(*args, unsigned int);
            snprintf(temp, sizeof(temp), "%*.*u", spec->width, spec->precision >= 0 ? spec->precision : 0, val);
            break;
        }
        case 'f':
        case 'F': {
            double val = va_arg(*args, double);
            snprintf(temp, sizeof(temp), "%*.*f", spec->width, spec->precision >= 0 ? spec->precision : 6, val);
            break;
        }
        case 's': {
            const char *val = va_arg(*args, const char *);
            if (spec->precision >= 0) {
                snprintf(temp, sizeof(temp), "%.*s", spec->precision, val);
            } else {
                snprintf(temp, sizeof(temp), "%s", val);
            }
            break;
        }
        case 'c': {
            int val = va_arg(*args, int);
            temp[0] = (char)val;
            temp[1] = '\0';
            break;
        }
        case '%': {
            temp[0] = '%';
            temp[1] = '\0';
            break;
        }
        default:
            temp[0] = '\0';
            break;
    }
    // Copy formatted string to output buffer
    written = strnlen(temp, out_size - 1);
    strncpy(out, temp, out_size - 1);
    out[written] = '\0';
    return written;
}
