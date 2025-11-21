# Memory Management

## Scope
Applies to memory allocation, deallocation, memory pools, custom allocators, memory mapping, and memory safety in system programming.

## Memory Allocation

### malloc() and free()
* Always check malloc() return value (NULL on failure)
* Match malloc() with free()
* Never free() memory twice
* Never use memory after free()
* Use calloc() for zero initialized memory
* Use realloc() for resizing (may move memory)

### Memory Alignment
* Understand natural alignment requirements
* Use aligned_alloc() for explicit alignment
* Use posix_memalign() for aligned allocation
* Document alignment requirements

### Memory Leaks
* Always free() allocated memory
* Use RAII patterns in C++
* Use memory leak detection tools (Valgrind, ASAN)
* Document memory ownership

## Memory Mapping

### mmap() and munmap()
* Use mmap() for large allocations
* Use MAP_ANONYMOUS for heap like allocation
* Use MAP_PRIVATE for copy on write
* Always munmap() before process exit
* Check return value against MAP_FAILED

### Memory Protection
* Use mprotect() to change protections
* Understand PROT_READ, PROT_WRITE, PROT_EXEC
* Use mlock() to prevent swapping
* Document memory protection changes

## Custom Allocators

### Memory Pools
* Use memory pools for frequent allocations
* Reduce malloc()/free() overhead
* Improve cache locality
* Document pool allocation patterns

### Arena Allocators
* Allocate from contiguous memory region
* Free entire arena at once
* Very fast allocation
* Use for temporary allocations

### Stack Allocators
* Allocate from stack like structure
* Very fast allocation and deallocation
* Limited lifetime
* Use for scoped allocations

## Memory Safety

### Buffer Overflows
* Always check buffer bounds
* Use bounded string functions (strncpy, snprintf)
* Validate input sizes
* Use static analysis tools

### Use After Free
* Never use memory after free()
* Set pointers to NULL after free()
* Use RAII to manage lifetime
* Use memory sanitizers

### Double Free
* Never free() memory twice
* Set pointers to NULL after free()
* Use RAII to prevent double free
* Use memory sanitizers

## NUMA Awareness

### NUMA Topology
* Use numa_available() to check NUMA
* Allocate on specific NUMA nodes
* Set thread affinity to NUMA nodes
* Profile NUMA effects

### NUMA Allocation
* Use numa_alloc_onnode() for node specific allocation
* Use numa_set_localalloc() for local allocation
* Understand NUMA distance
* Document NUMA requirements

## Implementation Standards

### Error Handling
* Check all allocation return values
* Handle ENOMEM errors gracefully
* Provide fallback strategies
* Document memory requirements

### Resource Management
* Free all allocated memory
* Use RAII patterns where possible
* Document memory ownership
* Provide cleanup functions

### Documentation
* Document memory allocation strategies
* Explain memory ownership
* Note alignment requirements
* Document NUMA considerations

## Code Examples

### Safe Memory Allocation
```cpp
// Thread-safety: Thread-safe (allocation)
// Ownership: Caller owns returned memory, must free
// Invariants: size > 0
// Failure modes: Returns nullptr on allocation failure
void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr == NULL) {
        perror("malloc failed");
        return NULL;
    }
    return ptr;
}
```

### Memory Pool Pattern
```cpp
// Thread-safety: Not thread-safe (shared pool)
// Ownership: Owns pool memory
// Invariants: pool_size > 0
// Failure modes: Returns nullptr on pool exhaustion
class MemoryPool {
    void* pool;
    size_t pool_size;
    size_t used;
public:
    explicit MemoryPool(size_t size);
    void* allocate(size_t size);
    void reset();  // Free all allocations
    ~MemoryPool();
};
```

## Testing Requirements
* Test memory allocation and deallocation
* Test memory pool allocation
* Test memory leak detection
* Test buffer overflow detection
* Test use after free detection
* Verify memory cleanup

## Related Topics
* Process Management: Memory mapping and virtual memory
* File Operations: File backed memory mappings
* Network Programming: Zero-copy networking
* Platform-Specific: Platform-specific memory APIs
* Performance Optimization: Memory performance profiling

