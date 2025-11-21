# Recursive and Backtracking Algorithm Sources

## Overview

This document identifies production-grade repositories and research papers for extracting recursive and backtracking algorithms. Focus on real-world implementations from production systems, not LeetCode-style problems.

## Production Repositories

### 1. SAT Solvers (Backtracking)

#### MiniSAT
- **Repository**: https://github.com/niklasso/minisat
- **Language**: C++
- **Key Files**: 
  - `core/Solver.cc` - Main SAT solver with backtracking
  - `core/Solver.h` - Solver interface
- **Algorithm**: DPLL (Davis-Putnam-Logemann-Loveland) with backtracking
- **Key Features**:
  - Conflict-driven clause learning (CDCL)
  - Two-watched literal scheme
  - Backtracking with learned clauses
  - Unit propagation
- **Use Case**: Boolean satisfiability problems, constraint solving

#### Glucose SAT Solver
- **Repository**: https://github.com/audemard/glucose
- **Language**: C++
- **Key Files**:
  - `core/Solver.cc` - Advanced SAT solver
- **Algorithm**: CDCL with advanced backtracking
- **Key Features**:
  - Lazy clause generation
  - Advanced backtracking strategies
  - Restart strategies
  - Clause minimization
- **Use Case**: Industrial SAT solving, verification

#### CryptoMiniSAT
- **Repository**: https://github.com/msoos/cryptominisat
- **Language**: C++
- **Key Features**:
  - XOR clause support
  - Advanced backtracking
  - Parallel solving
- **Use Case**: Cryptographic problems, verification

### 2. Constraint Programming Solvers (Backtracking)

#### Gecode
- **Repository**: https://github.com/Gecode/gecode
- **Language**: C++
- **Key Files**:
  - `gecode/search/dfs.cpp` - Depth-first search with backtracking
  - `gecode/search/engine.cpp` - Search engine
- **Algorithm**: Constraint propagation with backtracking
- **Key Features**:
  - Constraint propagation
  - Backtracking search
  - Branch-and-bound
  - Restart strategies
- **Use Case**: Constraint satisfaction problems, scheduling, optimization

#### Choco Solver
- **Repository**: https://github.com/chocoteam/choco-solver
- **Language**: Java
- **Key Features**:
  - Constraint programming
  - Backtracking search
  - Constraint propagation
- **Use Case**: Constraint satisfaction, optimization

#### OR-Tools (Google)
- **Repository**: https://github.com/google/or-tools
- **Language**: C++, Python, Java, C#
- **Key Features**:
  - Constraint programming
  - SAT solver integration
  - Backtracking algorithms
- **Use Case**: Optimization, scheduling, routing

### 3. Compiler Parsers (Recursive Descent)

#### LLVM Parser
- **Repository**: https://github.com/llvm/llvm-project
- **Language**: C++
- **Key Files**:
  - `clang/lib/Parse/ParseExpr.cpp` - Expression parsing (recursive descent)
  - `clang/lib/Parse/ParseStmt.cpp` - Statement parsing
  - `clang/lib/Parse/ParseDecl.cpp` - Declaration parsing
- **Algorithm**: Recursive descent parsing
- **Key Features**:
  - Top-down parsing
  - Error recovery
  - Backtracking for ambiguous cases
- **Use Case**: C/C++ parsing, compiler frontend

#### GCC Parser
- **Repository**: https://github.com/gcc-mirror/gcc
- **Language**: C
- **Key Files**:
  - `gcc/c-parser.c` - C parser (recursive descent)
- **Algorithm**: Recursive descent parsing
- **Key Features**:
  - Recursive descent
  - Error recovery
  - Backtracking
- **Use Case**: C/C++ parsing

#### Rust Compiler (rustc)
- **Repository**: https://github.com/rust-lang/rust
- **Language**: Rust
- **Key Files**:
  - `compiler/rustc_parse/src/parser/expr.rs` - Expression parsing
  - `compiler/rustc_parse/src/parser/mod.rs` - Main parser
- **Algorithm**: Recursive descent with Pratt parsing
- **Key Features**:
  - Recursive descent
  - Operator precedence parsing
  - Error recovery
- **Use Case**: Rust language parsing

### 4. Algorithm X / Dancing Links

#### Knuth's Algorithm X Implementations
- **Repository**: Various implementations
- **Algorithm**: Exact cover problem solver
- **Key Features**:
  - Recursive backtracking
  - Dancing links data structure
  - Efficient constraint satisfaction
- **Use Case**: Sudoku solving, exact cover problems, N-queens

#### Sudoku Solvers (Production)
- **Repository**: Various
- **Key Features**:
  - Backtracking search
  - Constraint propagation
  - Heuristic selection
- **Use Case**: Sudoku solving, constraint satisfaction

### 5. Database Query Planners (Recursive)

#### PostgreSQL Query Planner
- **Repository**: https://github.com/postgres/postgres
- **Language**: C
- **Key Files**:
  - `src/backend/optimizer/path/allpaths.c` - Path generation (recursive)
  - `src/backend/optimizer/plan/planner.c` - Query planning
- **Algorithm**: Dynamic programming with recursive exploration
- **Key Features**:
  - Recursive query planning
  - Join ordering optimization
  - Backtracking for optimal plans
- **Use Case**: SQL query optimization

#### SQLite Query Planner
- **Repository**: https://github.com/sqlite/sqlite
- **Language**: C
- **Key Files**:
  - `src/where.c` - WHERE clause optimization
  - `src/select.c` - SELECT statement planning
- **Algorithm**: Recursive query planning
- **Key Features**:
  - Recursive optimization
  - Index selection
  - Join ordering
- **Use Case**: SQL query optimization

### 6. Game Engines (Backtracking Pathfinding)

#### Pathfinding Algorithms
- **Repository**: Various game engines
- **Algorithm**: A* with backtracking
- **Key Features**:
  - Pathfinding with backtracking
  - Constraint satisfaction
- **Use Case**: Game AI, pathfinding

### 7. Regular Expression Engines (Backtracking)

#### RE2 (Google)
- **Repository**: https://github.com/google/re2
- **Language**: C++
- **Key Features**:
  - NFA-based matching
  - Backtracking for complex patterns
  - Performance optimizations
- **Use Case**: Regular expression matching

#### PCRE (Perl Compatible Regular Expressions)
- **Repository**: https://github.com/PCRE2Project/pcre2
- **Language**: C
- **Key Features**:
  - Backtracking engine
  - Recursive pattern matching
- **Use Case**: Regular expression matching

## Research Papers

### 1. Backtracking Algorithms

#### "An Easy-to-use Scalable Framework for Parallel Recursive Backtracking"
- **Authors**: Faisal N. Abu-Khzam et al.
- **Source**: arXiv:1312.7626
- **URL**: https://arxiv.org/abs/1312.7626
- **Key Contributions**:
  - Framework for parallelizing backtracking
  - Linear speedups on thousands of cores
  - Case studies on Vertex Cover and Dominating Set
- **Use Case**: Parallel backtracking, NP-hard problems

#### "Quantum Walk Speedup of Backtracking Algorithms"
- **Author**: Ashley Montanaro
- **Source**: arXiv:1509.02374
- **URL**: https://arxiv.org/abs/1509.02374
- **Key Contributions**:
  - Quantum speedups for backtracking
  - Constraint satisfaction problems
  - Quantum walk algorithms
- **Use Case**: Quantum computing, constraint satisfaction

#### "Operational Framework for Recent Advances in Backtracking Search Optimisation Algorithm"
- **Authors**: Bryar A. Hassan, Tarik A. Rashid
- **Source**: arXiv:1911.13011
- **URL**: https://arxiv.org/abs/1911.13011
- **Key Contributions**:
  - Systematic review of backtracking search
  - Performance evaluation
  - Optimization algorithms
- **Use Case**: Optimization, search algorithms

### 2. Recursive Algorithms

#### "Recursive Algorithms" (Various Papers)
- **Topics**:
  - Divide and conquer
  - Dynamic programming
  - Tree traversal
  - Graph algorithms
- **Use Case**: General recursive algorithms

### 3. Constraint Satisfaction

#### "The Complexity of Constraint Satisfaction"
- **Authors**: Various
- **Topics**:
  - CSP algorithms
  - Backtracking strategies
  - Constraint propagation
- **Use Case**: Constraint satisfaction problems

### 4. Parsing Algorithms

#### "Recursive Descent Parsing"
- **Authors**: Various compiler researchers
- **Topics**:
  - Top-down parsing
  - Error recovery
  - Backtracking parsers
- **Use Case**: Compiler construction, parsing

## Extraction Strategy

### Priority Sources (Production-Grade)

1. **SAT Solvers** (Highest Priority)
   - MiniSAT: Core backtracking algorithm
   - Glucose: Advanced backtracking strategies
   - CryptoMiniSAT: XOR clause handling

2. **Constraint Solvers** (High Priority)
   - Gecode: Production constraint solver
   - OR-Tools: Google's optimization tools

3. **Compiler Parsers** (High Priority)
   - LLVM: Recursive descent parsing
   - GCC: C parser implementation
   - Rust: Modern parser design

4. **Database Query Planners** (Medium Priority)
   - PostgreSQL: Query optimization
   - SQLite: Lightweight planner

5. **Algorithm X / Dancing Links** (Medium Priority)
   - Knuth's Algorithm X implementations
   - Exact cover problem solvers

### Variants to Extract

#### Backtracking Variants

1. **DPLL/CDCL Backtracking** (from SAT solvers)
   - Conflict-driven clause learning
   - Unit propagation
   - Backtracking with learned clauses

2. **Constraint Propagation Backtracking** (from Gecode)
   - Constraint propagation
   - Domain reduction
   - Backtracking search

3. **Recursive Backtracking** (from Algorithm X)
   - Exact cover problem
   - Dancing links
   - Constraint satisfaction

4. **Parallel Backtracking** (from research papers)
   - Parallel recursive backtracking
   - Work stealing
   - Load balancing

#### Recursive Variants

1. **Recursive Descent Parsing** (from compilers)
   - Top-down parsing
   - Error recovery
   - Operator precedence

2. **Recursive Query Planning** (from databases)
   - Join ordering
   - Cost estimation
   - Plan optimization

3. **Tree Recursion** (from various sources)
   - Tree traversal
   - Divide and conquer
   - Dynamic programming

4. **Tail Recursion** (optimization)
   - Tail call optimization
   - Iterative conversion
   - Stack optimization

## File Structure Plan

```
algorithms/
├── production_patterns/
│   ├── backtracking/
│   │   ├── variants/
│   │   │   ├── minisat_dpll.cpp          # MiniSAT DPLL backtracking
│   │   │   ├── glucose_cdcl.cpp          # Glucose CDCL backtracking
│   │   │   ├── gecode_constraint.cpp     # Gecode constraint backtracking
│   │   │   ├── knuth_algorithm_x.cpp      # Knuth's Algorithm X
│   │   │   └── parallel_backtracking.cpp  # Parallel backtracking
│   │   └── PATTERN_RECOGNITION.md
│   ├── recursion/
│   │   ├── variants/
│   │   │   ├── llvm_recursive_descent.cpp # LLVM parser
│   │   │   ├── postgresql_query_plan.cpp  # PostgreSQL planner
│   │   │   ├── tree_recursion.cpp         # Tree recursion patterns
│   │   │   └── tail_recursion.cpp        # Tail recursion optimization
│   │   └── PATTERN_RECOGNITION.md
└── extraction_notes/
    ├── BACKTRACKING_EXTRACTION.md
    └── RECURSION_EXTRACTION.md
```

## Key Algorithms to Extract

### Backtracking Algorithms

1. **DPLL Algorithm** (from MiniSAT)
   - Boolean satisfiability
   - Unit propagation
   - Backtracking

2. **CDCL Algorithm** (from Glucose)
   - Conflict-driven learning
   - Clause learning
   - Advanced backtracking

3. **Constraint Propagation** (from Gecode)
   - Domain reduction
   - Constraint propagation
   - Backtracking search

4. **Algorithm X** (from Knuth)
   - Exact cover problem
   - Dancing links
   - Recursive backtracking

### Recursive Algorithms

1. **Recursive Descent Parser** (from LLVM/GCC)
   - Expression parsing
   - Statement parsing
   - Error recovery

2. **Query Planner** (from PostgreSQL)
   - Join ordering
   - Cost estimation
   - Recursive optimization

3. **Tree Traversal** (various)
   - Depth-first search
   - Tree recursion patterns
   - Memoization

## Source Attribution Format

### GitHub Repositories
- **Repository**: URL
- **File**: Path to file
- **Lines**: Line numbers (if specific)
- **Commit**: Commit hash (if available)
- **License**: License type

### Research Papers
- **Title**: Paper title
- **Authors**: Author names
- **Source**: Conference/journal
- **DOI**: DOI number
- **URL**: arXiv or publisher URL
- **Year**: Publication year

## Next Steps

1. **Extract SAT Solver Backtracking** (MiniSAT, Glucose)
2. **Extract Constraint Solver Backtracking** (Gecode)
3. **Extract Recursive Descent Parsing** (LLVM, GCC)
4. **Extract Query Planner Recursion** (PostgreSQL)
5. **Extract Algorithm X** (Knuth's implementation)
6. **Create Pattern Recognition Guides**
7. **Create Extraction Notes**

## References

- MiniSAT: https://github.com/niklasso/minisat
- Glucose: https://github.com/audemard/glucose
- Gecode: https://github.com/Gecode/gecode
- LLVM: https://github.com/llvm/llvm-project
- PostgreSQL: https://github.com/postgres/postgres
- OR-Tools: https://github.com/google/or-tools

