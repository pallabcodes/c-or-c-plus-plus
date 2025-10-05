# Bitwise Learning - Bits, Bytes, and Bit Manipulation

Production-grade curriculum covering fundamentals to god-modded techniques used at Google, Uber, Bloomberg, Amazon, PayPal, Stripe, and more.

## Modules

- 01-fundamentals
  - binary representation, two's complement, masks, shifts, rotates
  - endianness, byte order, alignment
  - core bit tricks (set/clear/toggle/test, isolate lsb/msb)
- 02-advanced
  - SWAR techniques, parallel bit ops, branchless logic
  - fast popcount, clz/ctz, parity, bit reversal
  - bitboards, Morton/Z-order, Gray codes
- 03-enterprise
  - Google: bloom filters, hash mixing, sketches
  - Uber: geo encoding, geohash, H3 style concepts
  - Bloomberg: market flags, protocol fields, compact encodings
- 04-performance
  - SIMD bit ops, BMI1/BMI2 (PDEP/PEXT), AVX2/AVX-512
  - cache-aware bitsets, roaring bitmaps
  - atomic bitfields and contention reduction
- 05-system
  - protocols and checksums, CRCs, Fletcher
  - byte order conversions, unaligned access
  - memory mapped registers, masks, and barriers
- 06-god-modded
  - succinct data structures, rank/select
  - bitslicing, boolean SIMD
  - compressed bitvectors, Elias/Fano, Golomb-Rice

## Quality bar
- enterprise comments, safety notes, and portability guidance
- demos compile out of the box with `make`
- CI builds on gcc/clang, linux/macos

See BUILD.md for build instructions.
