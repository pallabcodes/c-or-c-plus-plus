// custom_printf.h
// Main API for Google-grade custom printf
// References: ISO C Standard, Google C++ Style Guide

#ifndef CUSTOM_PRINTF_H
#define CUSTOM_PRINTF_H

#include <stdarg.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Custom printf function
int my_printf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif // CUSTOM_PRINTF_H
