# Query Processing Standards

## Scope
Applies to all query processing code including parsing, planning, optimization, and execution. Extends repository root rules.

## SQL Parsing

### Lexical Analysis
* Tokenization of SQL statements
* Handle SQL dialects and extensions
* Proper error reporting for syntax errors
* Unicode support for identifiers

### Parsing
* Recursive descent or generated parser (e.g., ANTLR, Lemon)
* Abstract syntax tree (AST) generation
* Syntax error recovery where possible
* Preserve source location for error messages

### Semantic Analysis
* Type checking and resolution
* Name resolution (tables, columns, functions)
* Constraint validation
* View expansion

## Query Planning

### Logical Plan Generation
* Convert AST to logical query plan
* Relational algebra operators (select, project, join, aggregate, union)
* Push down predicates where possible
* Eliminate redundant operations

### Physical Plan Generation
* Choose physical operators for logical operators
* Select access methods (table scan, index scan, index only scan)
* Join algorithm selection (nested loop, hash join, sort merge)
* Materialization vs pipelining decisions

## Query Optimization

### Rule Based Optimization
* Predicate pushdown
* Projection pushdown
* Join reordering
* Constant folding
* Dead code elimination

### Cost Based Optimization
* Statistics collection and maintenance (histograms, distinct value counts)
* Cost models for operators
* Join order optimization (dynamic programming or greedy)
* Access method selection based on selectivity
* Reference: "Architecture of a Database System" (Hellerstein et al., 2007)

### Advanced Optimization
* Query rewriting for common patterns
* Parallel query optimization
* Adaptive query execution
* Materialized view selection

## Query Execution

### Execution Models

#### Volcano Iterator Model
* Pull based iterator interface
* Lazy evaluation
* Standard implementation pattern
* Works well with pipelining

#### Vectorized Execution
* Process batches of tuples
* SIMD optimizations
* Cache efficient
* Reference: "MonetDB/X100: Hyper-Pipelining Query Execution" (Boncz et al., 2005)
* Reference: "Efficiently Compiling Efficient Query Plans for Modern Hardware" (Neumann, 2011)

#### Push Based Execution
* Producer consumer model
* Better for parallel execution
* Reduces function call overhead

### Operator Implementation

#### Scans
* Sequential scan with prefetching
* Index scan with range support
* Index only scan when possible
* Parallel scan partitioning

#### Joins
* Nested loop join with block optimization
* Hash join with graceful degradation
* Sort merge join for sorted inputs
* Adaptive join selection at runtime

#### Aggregations
* Hash based aggregation
* Sort based aggregation
* Partial aggregations for parallel execution
* Window function support

## JIT Compilation
* LLVM integration for query compilation
* Compile hot query paths to native code
* Optimize tight loops
* Reference: "JIT-compiling SQL queries in MonetDB" (Neumann & Kemper, 2015)

## Implementation Requirements
* Proper error handling throughout query processing pipeline
* Memory management for intermediate results
* Query cancellation support
* Resource limit enforcement
* Progress reporting for long running queries

