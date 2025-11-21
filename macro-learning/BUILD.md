# Build guide for macro-learning

## Quick start
```bash
cd macro-learning
make
```

## Per module
```bash
make fundamentals
make advanced
make enterprise
make performance
make system
make advanced_techniques
```

## Customize
```bash
make CC=clang CFLAGS="-O3 -std=c11 -Wall -Wextra -Werror"
```

## Build requirements
* C99 or C11 compatible compiler (gcc or clang)
* Make
* Standard C library

## Compiler flags
Default flags include:
* `-Wall -Wextra -Werror -Wpedantic` for strict warnings
* `-O2 -g` for optimization and debug symbols
* `-fstack-protector-strong` for stack protection
* `-D_FORTIFY_SOURCE=2` for additional security checks
* `-std=c11` for C11 standard features

## Platform support
* Linux (x86_64, aarch64)
* macOS (x86_64, arm64)
* Any C11 compliant compiler

## Preprocessor output
To see macro expansion:
```bash
gcc -E source.c -o source.i
```

