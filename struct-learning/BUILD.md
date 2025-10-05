# Build guide for struct-learning

## Prerequisites
- make
- g++ or clang++ with C++17 support
- Linux, macOS, or Windows with Make (MSYS2 or WSL recommended on Windows)

## Quick start
```bash
cd struct-learning
make            # builds all modules into build/.../*.out
make list       # lists discovered sources and binaries
```

## Build specific modules
```bash
make fundamentals
make advanced
make enterprise
make performance
make system
make godmodded
```

## Customize toolchain and flags
```bash
make CXX=clang++ OPT=-O3 SAN="-fsanitize=address -fno-omit-frame-pointer"
```

## Run binaries
Outputs are under `build/` mirroring the source tree.
```bash
./build/01-fundamentals/01-basic-structs.out
```

## Clean
```bash
make clean
```

## Troubleshooting
- On macOS with Apple Clang, remove `-msse` or AVX specific flags if present
- On Windows, use MSYS2 or WSL to get `make` and GNU toolchain
- For AVX code in `02-simd-structs.cpp`, ensure your CPU supports AVX (or adjust code to SSE)
