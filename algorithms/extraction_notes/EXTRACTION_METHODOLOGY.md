# Pattern Extraction Methodology

## Multi-Source Extraction Process

### Phase 1: Pattern Identification
1. Identify pattern to extract (e.g., hash table, red-black tree)
2. List known variants and optimizations
3. Determine search keywords and terms

### Phase 2: Multi-Source Search

#### 2.1 Local Codebases
- Search Linux kernel (`/linux/`)
- Search Node.js/V8 (`/node/`)
- Use `grep` and `codebase_search` tools
- Extract implementations with file paths and line numbers

#### 2.2 GitHub Repositories
- Clone or search GitHub repos
- Use GitHub search: `language:c hash table`
- Search specific files in repos
- Extract with commit hashes and URLs

**Example Search Queries**:
- `redis dict.c hash table`
- `postgresql hash table implementation`
- `nginx hash table`

#### 2.3 Research Papers
- Search Google Scholar
- Search ACM Digital Library
- Search arXiv
- Read papers and extract algorithms
- Implement variants based on papers

**Example Searches**:
- "lock-free hash table"
- "cuckoo hashing"
- "robin hood hashing"

#### 2.4 Technical Blogs
- Search engineering blogs
- Look for "how we built X" posts
- Extract code examples and explanations
- Document blog URLs and dates

**Example Searches**:
- "redis hash table implementation"
- "postgresql b-tree internals"
- "high performance hash table"

### Phase 3: Variant Extraction

For each source found:

1. **Read and Understand**
   - Read the code/paper/blog
   - Understand the optimization/technique
   - Identify key features

2. **Extract Implementation**
   - Copy relevant code (if codebase)
   - Implement based on description (if paper/blog)
   - Create standalone variant file

3. **Document Source**
   - File path and line numbers (codebase)
   - GitHub URL and commit hash (GitHub)
   - Paper title, authors, DOI (research)
   - Blog URL and date (blog)

4. **Identify Key Features**
   - What makes it ingenious?
   - What optimizations does it use?
   - When should it be used?

### Phase 4: Variant Categorization

Group variants by:
- **Optimization technique**: Lock-free, cache-friendly, etc.
- **Use case**: Small data, large data, concurrent, etc.
- **Performance characteristics**: O(1) vs O(log n), etc.

### Phase 5: Pattern Recognition Guide

Create/update pattern recognition guide with:
- When to use each variant
- Input characteristics mapping
- Decision tree
- Real-world examples

### Phase 6: Documentation

Update:
- Pattern recognition guide
- Extraction notes
- Source tracking document
- Summary document

## Example: Hash Table Extraction

### Step 1: Identify Pattern
- Pattern: Hash Table
- Known variants: Open addressing, chaining, cuckoo hashing, robin hood hashing

### Step 2: Multi-Source Search

#### Local Codebases
- Linux kernel: `linux/include/linux/hashtable.h`
- Node.js/V8: Search for hash table implementations

#### GitHub Repositories
- Redis: `src/dict.c` (open addressing with incremental rehashing)
- PostgreSQL: Hash table implementations
- nginx: Hash table for configuration

#### Research Papers
- "Cuckoo Hashing" by Pagh and Rodler
- "Robin Hood Hashing" papers
- Lock-free hash table papers

#### Technical Blogs
- Redis blog: "Redis hash table internals"
- PostgreSQL blog: "Hash table optimizations"

### Step 3: Extract Variants

1. **Redis Variant** (GitHub)
   - Source: `redis/src/dict.c`
   - Features: Open addressing, incremental rehashing
   - Extract: Implementation code

2. **PostgreSQL Variant** (GitHub)
   - Source: `postgres/src/backend/utils/hash/dynahash.c`
   - Features: Chaining, dynamic resizing
   - Extract: Implementation code

3. **Linux Kernel Variant** (Local)
   - Source: `linux/include/linux/hashtable.h`
   - Features: Lock-free, RCU-safe
   - Extract: Implementation code

4. **Cuckoo Hashing** (Research Paper)
   - Source: "Cuckoo Hashing" paper
   - Features: O(1) worst-case lookup
   - Extract: Implement based on paper

5. **Robin Hood Hashing** (Research Paper)
   - Source: Robin Hood hashing papers
   - Features: Reduced variance in probe length
   - Extract: Implement based on paper

### Step 4: Categorize

- **Open Addressing**: Redis, Cuckoo, Robin Hood
- **Chaining**: PostgreSQL
- **Lock-Free**: Linux kernel

### Step 5: Create Pattern Recognition Guide

- When to use open addressing vs chaining
- When to use cuckoo hashing
- When to use lock-free variants
- Decision tree for variant selection

### Step 6: Document

- Update `SOURCE_TRACKING.md` with sources
- Update extraction notes
- Create variant files with source attribution

## Tools and Commands

### Local Codebase Search
```bash
# Search Linux kernel
grep -r "hash_table" linux/

# Search Node.js
grep -r "hash.*table" node/
```

### GitHub Search
- Use GitHub web interface
- Search: `language:c "hash table" filename:*.c`
- Browse specific repositories

### Research Paper Search
- Google Scholar: Search terms
- ACM Digital Library: Advanced search
- arXiv: Search by subject

### Web Search
- Use `web_search` tool for technical blogs
- Search: "redis hash table implementation"
- Search: "postgresql b-tree internals"

## Quality Criteria

For each variant, ensure:

1. **Source Attribution**: Clear source (file, URL, paper, blog)
2. **Understanding**: We understand how it works
3. **Key Features**: Documented what makes it special
4. **Use Cases**: When to use this variant
5. **Implementation**: Working code example

## Continuous Improvement

- Regularly search for new variants
- Update pattern recognition guides
- Add new sources as discovered
- Keep extraction notes current

