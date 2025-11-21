# Graph Algorithm Pattern Recognition

## When to Recognize Graph Algorithm Opportunity

### Input Characteristics That Suggest Graph Algorithms

1. **Relationships Between Entities**
   - Entities have relationships/connections
   - Need to traverse relationships
   - Network/graph structure naturally

2. **Hierarchical/Tree Structure**
   - Parent-child relationships
   - Tree-like data
   - Need to traverse hierarchy

3. **Dependencies**
   - Entities depend on each other
   - Need to resolve dependencies
   - Topological ordering needed

4. **Path Finding**
   - Need to find paths between nodes
   - Shortest path problems
   - Route optimization

## Variant Selection Guide

### Decision Tree

```
Need graph algorithm?
├─ Tree/hierarchy traversal?
│  ├─ Need scheduling/priority?
│  │  └─ YES → React Fiber or React Scheduler
│  ├─ Need tree diffing/reconciliation?
│  │  └─ YES → React Diffing
│  └─ Standard traversal?
│     └─ YES → React Fiber
├─ Control flow analysis?
│  └─ YES → LLVM CFG or Dominator Tree
├─ Need dominator information?
│  └─ YES → Dominator Tree Algorithm
├─ Need CFG construction?
│  └─ YES → LLVM CFG
├─ Need work scheduling with priorities?
│  └─ YES → React Scheduler
└─ General graph traversal?
   └─ React Fiber or standard DFS/BFS
```

### Specific Variant Selection

#### React Fiber Reconciliation

**Use When**:
- Tree/hierarchy traversal with scheduling
- Incremental processing needed
- Priority-based traversal
- Need to pause/resume traversal
- UI rendering systems

**Key Features**:
- Depth-first traversal
- Work scheduling with priority
- Incremental processing (time-slicing)
- Can pause and resume

**Trade-offs**:
- More complex than standard DFS
- Better for interactive systems
- Overhead for simple cases

#### React Scheduler

**Use When**:
- Need to schedule work with priorities
- Time-sliced processing (pause/resume)
- Keep UI responsive during heavy computation
- Incremental processing of large tasks
- Priority-based task scheduling

**Key Features**:
- Time-sliced work loop
- Priority-based scheduling
- Work expiration tracking
- Continuous work loop
- Can pause/resume work

**Trade-offs**:
- More complex than simple queues
- Better for responsive systems
- Overhead for simple cases

#### React Diffing Algorithm

**Use When**:
- Tree reconciliation/diffing
- Minimize update operations
- Efficient tree comparison
- UI rendering optimization
- Incremental tree updates

**Key Features**:
- Three-way diffing
- Key-based reconciliation
- Minimal DOM operations
- Multi-pass diffing
- O(n) complexity with keys

**Trade-offs**:
- Requires keys for optimal performance
- More complex than simple comparison
- O(n²) without keys

#### LLVM Control Flow Graph

**Use When**:
- Compiler construction
- Code analysis
- Need CFG representation
- Static analysis
- Optimization passes

**Key Features**:
- CFG construction from code
- Basic block representation
- Edge representation
- Foundation for analysis

**Trade-offs**:
- Compiler-specific
- More complex than simple graphs
- Powerful for code analysis

#### Dominator Tree Algorithm

**Use When**:
- Need dominator information
- Compiler optimizations
- Static code analysis
- Dead code elimination
- Loop detection

**Key Features**:
- O(n log n) or better complexity
- Simple and efficient
- Production-proven algorithm
- Used in GCC, LLVM

**Trade-offs**:
- Specific to dominator computation
- More complex than simple traversal
- Essential for compiler optimizations

## Input Characteristics → Variant Mapping

| Input Characteristic | Recommended Variant | Reason |
|---------------------|---------------------|--------|
| Tree/hierarchy, scheduling | React Fiber | Depth-first with scheduling |
| Work scheduling, priorities | React Scheduler | Priority-based work loop |
| Tree diffing/reconciliation | React Diffing | Key-based efficient diffing |
| Compiler, CFG needed | LLVM CFG | CFG construction |
| Dominator information | Dominator Tree | Fast dominator algorithm |
| General traversal | React Fiber | Flexible traversal |
| Incremental processing | React Fiber/Scheduler | Time-slicing support |
| UI rendering optimization | React Diffing | Minimal updates |

## Performance Characteristics Comparison

| Variant | Traversal | Scheduling | Best For |
|---------|-----------|------------|----------|
| React Fiber | O(n) | O(log n) | Tree traversal with scheduling |
| React Scheduler | O(n) | O(log n) | Work scheduling with priorities |
| React Diffing | O(n) with keys | N/A | Tree reconciliation/diffing |
| LLVM CFG | O(n+e) | N/A | CFG construction |
| Dominator Tree | O(n log n) | N/A | Dominator computation |

## Real-World Examples

### React Fiber Reconciliation
- **React UI Rendering**: Component tree reconciliation
- **UI Frameworks**: Incremental rendering
- **Interactive Systems**: Responsive UI updates

### React Scheduler
- **React Concurrent Rendering**: Work scheduling for fiber
- **UI Frameworks**: Responsive rendering
- **Incremental Processing**: Large task processing

### React Diffing
- **React Reconciliation**: Efficient UI updates
- **Virtual DOM**: Tree diffing
- **UI Frameworks**: Minimal update operations

### LLVM Control Flow Graph
- **LLVM Compiler**: Code optimization
- **Static Analysis**: Code understanding
- **Compiler Backends**: Code generation

### Dominator Tree Algorithm
- **GCC Compiler**: Optimization passes
- **LLVM Compiler**: Optimization passes
- **Static Analysis**: Dead code elimination

## Pattern Recognition Checklist

Before choosing a graph algorithm variant, ask:

1. **What's the graph structure?**
   - Tree/hierarchy? → React Fiber
   - Control flow? → LLVM CFG
   - General graph? → Standard algorithms

2. **Need scheduling/priority?**
   - YES → React Fiber
   - NO → Standard DFS/BFS

3. **Need incremental processing?**
   - YES → React Fiber
   - NO → Standard algorithms

4. **Is it for compiler/analysis?**
   - YES → LLVM CFG or Dominator Tree
   - NO → React Fiber or standard

5. **Need dominator information?**
   - YES → Dominator Tree Algorithm
   - NO → Other variants

## Common Mistakes to Avoid

1. **Using complex algorithm for simple case**
   - React Fiber for simple DFS
   - Dominator Tree when not needed

2. **Not considering scheduling needs**
   - Standard DFS when scheduling needed
   - React Fiber when not needed

3. **Wrong algorithm for use case**
   - Dominator Tree for general traversal
   - React Fiber for compiler analysis

4. **Ignoring incremental needs**
   - Blocking traversal when incremental needed
   - Not using time-slicing when beneficial

5. **Not understanding graph structure**
   - Using tree algorithms for general graphs
   - Using general algorithms for trees

## Universal Applications

- **React Fiber**: UI rendering, tree traversal, incremental processing
- **React Scheduler**: Work scheduling, priority queues, responsive systems
- **React Diffing**: Tree reconciliation, UI updates, virtual DOM
- **LLVM CFG**: Compilers, static analysis, code understanding
- **Dominator Tree**: Compiler optimizations, static analysis, program understanding

