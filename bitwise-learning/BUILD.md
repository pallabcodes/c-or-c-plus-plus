# Build guide for bitwise-learning

## Quick start
```bash
cd bitwise-learning
make
```

## Per module
```bash
make fundamentals
make advanced
make enterprise
make performance
make system
make godmodded
```

## Customize
```bash
make CXX=clang++ OPT=-O3 SAN="-fsanitize=address -fno-omit-frame-pointer" MARCH="-march=native"
```

## Build requirements
* C++17 compatible compiler (g++ or clang++)
* Make
* For SIMD examples: AVX2 support (x86_64)
* For BMI2 examples: BMI2 instruction set support

## Compiler flags
Default flags include:
* `-Wall -Wextra -Werror -Wpedantic -Wconversion` for strict warnings
* `-O2 -g` for optimization and debug symbols
* `-fstack-protector-strong` for stack protection
* `-D_FORTIFY_SOURCE=2` for additional security checks

## Platform support
* Linux (x86_64, aarch64)
* macOS (x86_64, arm64)
* Some examples require specific CPU features (AVX2, BMI2)
