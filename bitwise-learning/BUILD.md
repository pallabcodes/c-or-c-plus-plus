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
