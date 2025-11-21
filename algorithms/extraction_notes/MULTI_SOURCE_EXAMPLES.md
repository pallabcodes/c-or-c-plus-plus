# Multi-Source Extraction Examples

This document shows examples of extracting patterns from multiple sources (GitHub repos, research papers, blogs) beyond just local codebases.

## Example 1: Hash Table Extraction (Multi-Source)

### Source 1: Redis (GitHub Repository)

**Repository**: https://github.com/redis/redis
**File**: `src/dict.c`
**Lines**: 50-200 (example)

**What to Extract**:
- Open addressing with incremental rehashing
- Power-of-2 table sizes
- SipHash for hashing
- Rehashing strategy (progressive rehashing)

**How to Extract**:
1. Clone repository: `git clone https://github.com/redis/redis.git`
2. Read `src/dict.c`
3. Extract key functions: `dictAdd`, `dictRehash`, `dictFind`
4. Document optimizations
5. Create variant file: `production_patterns/hash_table/variants/redis_open_addressing.cpp`

**Key Features**:
- Incremental rehashing (doesn't block)
- Power-of-2 sizes (bitwise modulo)
- SipHash for security

### Source 2: PostgreSQL (GitHub Repository)

**Repository**: https://github.com/postgres/postgres
**File**: `src/backend/utils/hash/dynahash.c`
**Lines**: 100-300 (example)

**What to Extract**:
- Chaining with dynamic resizing
- Separate chaining implementation
- Dynamic hash table growth

**How to Extract**:
1. Clone repository: `git clone https://github.com/postgres/postgres.git`
2. Read `src/backend/utils/hash/dynahash.c`
3. Extract key functions: `hash_create`, `hash_search`, `hash_grow`
4. Document chaining strategy
5. Create variant file: `production_patterns/hash_table/variants/postgresql_chaining.cpp`

**Key Features**:
- Separate chaining
- Dynamic resizing
- Memory-efficient

### Source 3: Cuckoo Hashing (Research Paper)

**Paper**: "Cuckoo Hashing" by Rasmus Pagh and Flemming Friche Rodler
**Source**: https://www.cs.tau.ac.il/~shanir/advanced-seminar-data-structures-2011/lecture-1.pdf

**What to Extract**:
- Cuckoo hashing algorithm
- O(1) worst-case lookup
- Two hash functions
- Kick-out strategy

**How to Extract**:
1. Read paper
2. Understand algorithm
3. Implement based on paper description
4. Create variant file: `production_patterns/hash_table/variants/cuckoo_hashing.cpp`

**Key Features**:
- O(1) worst-case lookup
- Two hash functions
- Kick-out on collision

### Source 4: Robin Hood Hashing (Research Paper)

**Paper**: "Robin Hood Hashing" by Pedro Celis
**Source**: Various papers and implementations

**What to Extract**:
- Robin Hood hashing algorithm
- Reduced variance in probe length
- Backward shift deletion

**How to Extract**:
1. Read papers on Robin Hood hashing
2. Understand algorithm
3. Implement based on papers
4. Create variant file: `production_patterns/hash_table/variants/robin_hood_hashing.cpp`

**Key Features**:
- Reduced variance
- Backward shift deletion
- Better cache performance

### Source 5: Technical Blog

**Blog**: "Redis Hash Table Internals" (hypothetical)
**URL**: https://redis.io/blog/hash-table-internals

**What to Extract**:
- Explanation of Redis hash table design
- Performance characteristics
- Design decisions

**How to Extract**:
1. Read blog post
2. Extract key insights
3. Reference in documentation
4. Link to blog in source attribution

## Example 2: Red-Black Tree Extraction (Multi-Source)

### Source 1: Linux Kernel (Local)

**File**: `linux/include/linux/rbtree.h`
**Lines**: Entire file

**What to Extract**:
- Generic red-black tree implementation
- Intrusive tree nodes
- Lock-free operations

**How to Extract**:
1. Read `linux/include/linux/rbtree.h`
2. Read `linux/lib/rbtree.c`
3. Extract implementation
4. Create variant file: `production_patterns/red_black_tree/variants/linux_kernel.cpp`

### Source 2: PostgreSQL (GitHub)

**Repository**: https://github.com/postgres/postgres
**File**: `src/backend/access/nbtree/`

**What to Extract**:
- B-tree implementation (similar to red-black)
- Disk-based structure
- Concurrency control

**How to Extract**:
1. Clone PostgreSQL repository
2. Read B-tree implementation
3. Extract key concepts
4. Create variant file: `production_patterns/b_tree/variants/postgresql.cpp`

### Source 3: Research Paper

**Paper**: "Left-Leaning Red-Black Trees" by Robert Sedgewick
**Source**: Various papers

**What to Extract**:
- Left-leaning variant
- Simplified implementation
- Fewer cases to handle

**How to Extract**:
1. Read paper
2. Implement variant
3. Create variant file: `production_patterns/red_black_tree/variants/left_leaning.cpp`

## Example 3: Graph Algorithms Extraction (Multi-Source)

### Source 1: React (GitHub)

**Repository**: https://github.com/facebook/react
**File**: `packages/react-reconciler/src/ReactFiberReconciler.js`

**What to Extract**:
- Fiber reconciliation (graph traversal)
- Depth-first traversal
- Work scheduling

**How to Extract**:
1. Clone React repository
2. Read fiber reconciliation code
3. Extract graph traversal patterns
4. Create variant file: `production_patterns/graph_algorithms/variants/react_fiber.cpp`

### Source 2: LLVM (GitHub)

**Repository**: https://github.com/llvm/llvm-project
**File**: `llvm/lib/Analysis/`

**What to Extract**:
- Control flow graph algorithms
- Dominator tree
- Data flow analysis

**How to Extract**:
1. Clone LLVM repository
2. Read graph algorithm implementations
3. Extract patterns
4. Create variant files

### Source 3: Research Papers

**Paper**: "A Simple, Fast Dominance Algorithm" by Keith D. Cooper et al.
**Source**: Various graph algorithm papers

**What to Extract**:
- Dominator tree algorithms
- Graph traversal optimizations
- Advanced graph algorithms

**How to Extract**:
1. Read papers
2. Implement algorithms
3. Create variant files

## Extraction Workflow Template

### For GitHub Repositories:

1. **Clone Repository**
   ```bash
   git clone https://github.com/repo/name.git
   cd name
   ```

2. **Search for Pattern**
   ```bash
   grep -r "pattern_name" .
   find . -name "*.c" -o -name "*.cpp" | xargs grep "pattern"
   ```

3. **Read Relevant Files**
   - Identify key files
   - Read implementation
   - Understand optimizations

4. **Extract Implementation**
   - Copy relevant code
   - Document source (file, lines, commit hash)
   - Create variant file

5. **Document Source**
   ```markdown
   **Source**: https://github.com/repo/name/blob/commit/src/file.c#L123-145
   **Repository**: repo/name
   **File**: `src/file.c`
   **Lines**: 123-145
   **Commit**: abc123def456
   ```

### For Research Papers:

1. **Find Paper**
   - Search Google Scholar
   - Search ACM/IEEE
   - Search arXiv

2. **Read Paper**
   - Understand algorithm
   - Note key features
   - Understand optimizations

3. **Implement Variant**
   - Implement based on paper
   - Add optimizations
   - Create variant file

4. **Document Source**
   ```markdown
   **Source**: "Paper Title" by Author Name
   **Paper**: Conference/Journal Year
   **DOI**: 10.1145/1234567.1234568
   **URL**: https://doi.org/10.1145/1234567.1234568
   ```

### For Technical Blogs:

1. **Find Blog Post**
   - Search engineering blogs
   - Look for "how we built X" posts
   - Find technical deep dives

2. **Read and Extract**
   - Read blog post
   - Extract code examples
   - Understand explanations

3. **Document Source**
   ```markdown
   **Source**: "Blog Title" by Author Name
   **Blog**: https://engineering.company.com/post
   **Date**: 2023-01-15
   ```

## Tools for Multi-Source Extraction

### GitHub Search
- **Web Interface**: https://github.com/search
- **Advanced Search**: https://github.com/search/advanced
- **Code Search**: `language:c "hash table" filename:*.c`

### Research Papers
- **Google Scholar**: https://scholar.google.com/
- **ACM Digital Library**: https://dl.acm.org/
- **IEEE Xplore**: https://ieeexplore.ieee.org/
- **arXiv**: https://arxiv.org/

### Technical Blogs
- **Google Research Blog**: https://research.google/blog/
- **Facebook Engineering**: https://engineering.fb.com/
- **Netflix Tech Blog**: https://netflixtechblog.com/
- **Uber Engineering**: https://eng.uber.com/

## Best Practices

1. **Always Attribute Sources**: Document where each variant comes from
2. **Verify Implementations**: Test extracted code
3. **Understand Context**: Know why variant was chosen
4. **Document Trade-offs**: Note pros/cons of each variant
5. **Keep Sources Updated**: Track if sources change

## Next Patterns to Extract (Multi-Source)

### Hash Tables
- [ ] Redis (GitHub) - Open addressing
- [ ] PostgreSQL (GitHub) - Chaining
- [ ] Linux kernel (Local) - Lock-free
- [ ] Cuckoo hashing (Research) - O(1) worst-case
- [ ] Robin Hood hashing (Research) - Reduced variance

### Red-Black Trees
- [ ] Linux kernel (Local) - Generic implementation
- [ ] PostgreSQL (GitHub) - B-tree variant
- [ ] Left-leaning (Research) - Simplified

### Graph Algorithms
- [ ] React (GitHub) - Fiber reconciliation
- [ ] LLVM (GitHub) - Control flow graphs
- [ ] Research papers - Advanced algorithms

