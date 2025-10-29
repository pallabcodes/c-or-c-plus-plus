# Performance Optimization Standards

## Scope
Applies to all performance optimization code including virtual scrolling, lazy loading, and incremental parsing. Extends repository root rules.

## Virtual Scrolling

### Viewport Rendering
* Only render visible lines
* Calculate viewport bounds
* Virtual line positioning
* Efficient line measurement

### Scrolling Performance
* Smooth scrolling (< 60 FPS target)
* Minimize layout calculations
* Cache line heights
* Handle variable line heights

### Large File Handling
* Efficient memory usage for large files
* Lazy loading of file content
* Memory mapped file access
* Handle files larger than memory

## Incremental Operations

### Incremental Parsing
* Parse only changed regions
* Reuse parse tree where possible
* Efficient tree updates
* Reference: "Incremental Parsing" (Tim Wagner, 1998)
* Tree sitter for incremental parsing

### Incremental Highlighting
* Rehighlight only changed regions
* Reuse tokenization results
* Efficient region updates
* Background highlighting

### Incremental Analysis
* Incremental symbol indexing
* Update index on changes
* Efficient delta updates
* Background indexing

## Lazy Loading

### Lazy Component Loading
* Load UI components on demand
* Defer initialization
* Progressive enhancement
* Reduce startup time

### Lazy Data Loading
* Load data on demand
* Pagination for large lists
* Virtual scrolling integration
* Background data fetching

## Background Processing

### Background Tasks
* Move heavy operations to background threads
* Async operation handling
* Progress reporting
* Cancellation support

### Worker Threads
* Worker thread pool
* Task queue management
* Resource sharing
* Thread safety

## Caching Strategies

### Result Caching
* Cache expensive computations
* Cache query results
* Cache UI components
* Invalidation strategies

### Memory Caching
* LRU cache for frequently accessed data
* Cache size limits
* Memory pressure handling
* Cache eviction policies

## Implementation Requirements
* Profile critical paths
* Measure frame times
* Monitor memory usage
* Optimize hot paths
* Use appropriate data structures
* Minimize allocations

## Performance Targets
* UI responsiveness: < 16ms per frame (60 FPS)
* Completion latency: < 100ms
* Navigation latency: < 50ms
* Startup time: < 2 seconds
* Memory usage: reasonable for large workspaces

## Integration Points
* Editor component integration
* UI framework integration
* Language server integration
* Extension system integration

