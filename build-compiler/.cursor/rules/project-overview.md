# Compiler Construction Module Overview

## Context
This code is written by an SDE 2 backend and low level system engineer working with top tier product companies including Google, Microsoft, Apple, Bloomberg, PayPal, Stripe, Uber, Amazon, and other top tier silicon valley companies. This compiler implementation must meet enterprise production standards suitable for principal level engineering review and must be comparable to top tier compiler implementations used in production systems at GCC, Clang/LLVM, Rustc, and other production compilers.

## Purpose
This module covers the design and implementation of production grade compilers in C and C++. All code must follow production grade standards suitable for principal level code review and must demonstrate correct, efficient, and maintainable compiler implementations including lexical analysis, parsing, semantic analysis, intermediate representation, optimization, and code generation.

## Scope
* Applies to all C and C++ code in build-compiler directory
* Extends repository root rules defined in the root `.cursor/rules/` files
* Covers all aspects of compiler construction from lexical analysis to code generation
* Code quality standards align with expectations from top tier compiler companies like LLVM, GCC, and Rust

## Top Tier Compiler Comparisons

### LLVM/Clang (Apple, Google, Microsoft)
* Modular compiler infrastructure
* LLVM IR intermediate representation
* Comprehensive optimization passes
* Production tested at massive scale
* Efficient code generation

### GCC (GNU)
* Mature compiler infrastructure
* GIMPLE intermediate representation
* Extensive optimization support
* Production tested for decades
* Cross platform support

### Rustc (Mozilla, Rust Foundation)
* Modern compiler architecture
* MIR (Mid-level IR) intermediate representation
* Advanced type system
* Production tested at scale
* Memory safety guarantees

### Standard Compilers
* C++ Standard Library compiler patterns
* Standard compiler implementations
* Production grade compiler practices

## Compiler Phases

### Frontend
* **Lexical Analysis**: Tokenization of source code
* **Parsing**: Syntax analysis and AST construction
* **Semantic Analysis**: Type checking and name resolution
* **Rationale**: Frontend validates and structures source code

### Intermediate Representation
* **IR Design**: Language independent representation
* **SSA Form**: Static Single Assignment form
* **IR Transformations**: Optimization friendly representation
* **Rationale**: IR enables optimization and code generation

### Optimization
* **Data Flow Analysis**: Program analysis for optimization
* **Loop Optimizations**: Loop transformations
* **Inter-procedural Analysis**: Cross function optimizations
* **Rationale**: Optimization improves generated code quality

### Backend
* **Instruction Selection**: Choose target instructions
* **Register Allocation**: Assign variables to registers
* **Instruction Scheduling**: Order instructions for performance
* **Code Generation**: Emit target code
* **Rationale**: Backend generates executable code

## Modern Compiler Features

### Incremental Compilation
* **Definition**: Recompile only changed code
* **Benefits**: Faster compile times
* **Use cases**: Large codebases, development workflow
* **Rationale**: Incremental compilation improves productivity

### Language Server Protocol
* **Definition**: IDE integration protocol
* **Benefits**: Rich IDE features
* **Use cases**: Code completion, navigation, diagnostics
* **Rationale**: LSP enables developer experience

### Profile Guided Optimization
* **Definition**: Optimization using runtime profiles
* **Benefits**: Better optimization decisions
* **Use cases**: Performance critical code
* **Rationale**: PGO improves code quality

### Link Time Optimization
* **Definition**: Optimization across translation units
* **Benefits**: Whole program optimization
* **Use cases**: Final optimization pass
* **Rationale**: LTO enables cross file optimizations

## Production Standards

### Code Quality
* Functions limited to 50 lines
* Files limited to 200 lines
* Cyclomatic complexity ≤ 10
* Comprehensive error handling
* Input validation on all public APIs
* Memory safety and leak prevention

### Performance
* Fast compile times
* Efficient optimizations
* Low memory usage
* Scalable to large codebases
* Benchmark critical paths

### Correctness
* Correct language semantics
* Proper error handling
* Comprehensive test coverage
* Standards compliance
* Security considerations

### Documentation
* API documentation for all public functions
* Compiler phase documentation
* Optimization documentation
* Error message documentation
* Research paper citations

## Research Papers and References

### Compiler Theory
* "Compilers: Principles, Techniques, and Tools" (Aho, Lam, Sethi, Ullman)
* "Advanced Compiler Design and Implementation" (Muchnick)
* "Modern Compiler Implementation in C" (Appel)
* "Engineering a Compiler" (Cooper, Torczon)

### Open Source References
* LLVM compiler infrastructure
* GCC compiler collection
* Clang C/C++ compiler
* Rust compiler
* Swift compiler

## Implementation Goals

### Correctness
* Correct language semantics
* Proper error handling
* Standards compliance
* Comprehensive testing
* Security considerations

### Performance
* Fast compilation
* Efficient optimizations
* Low memory usage
* Scalable architecture
* Benchmark and optimize

### Maintainability
* Clean, readable code
* Comprehensive documentation
* Extensive test coverage
* Clear compiler phases
* Well documented trade offs
