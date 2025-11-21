# Recursion Pattern Recognition

## When to Recognize Recursion Opportunity

### Input Characteristics That Suggest Recursion

1. **Tree/Hierarchical Structure**
   - Natural tree structure
   - Parent-child relationships
   - Recursive data structures
   - Nested structures

2. **Divide and Conquer**
   - Problem can be split into subproblems
   - Subproblems are similar
   - Can combine subproblem solutions
   - Base case exists

3. **Overlapping Subproblems**
   - Same subproblems computed multiple times
   - Memoization beneficial
   - Dynamic programming applicable
   - Tree recursion structure

4. **Natural Recursive Definition**
   - Problem defined recursively
   - Mathematical recurrence relations
   - Recursive data structures
   - Grammar-based problems

## Variant Selection Guide

### Decision Tree

```
Need recursion?
│
├─ Constraint satisfaction/Exact cover?
│  └─ YES → Knuth's Algorithm X (Dancing Links)
│
├─ Toeplitz matrix/Signal processing?
│  └─ YES → Levinson Recursion
│
├─ Stack overflow risk/Deep recursion?
│  └─ YES → Trampoline Recursion or CPS
│
├─ Parsing with backtracking?
│  └─ YES → Recursive Descent with Backtracking (PEG)
│
├─ Parsing/Compiler?
│  └─ YES → Recursive Descent Parser
│
├─ Mutually recursive structures?
│  └─ YES → Mutual Recursion
│
├─ Infinite sequences/Generators?
│  └─ YES → Co-recursion
│
├─ Overlapping subproblems?
│  └─ YES → Advanced Memoization
│
├─ Game AI/Pathfinding?
│  └─ YES → Recursive Pathfinding (A*, IDA*)
│
├─ Procedural generation?
│  └─ YES → Recursive Procedural Generation
│
├─ Game AI (two-player)?
│  └─ YES → Minimax with Alpha-Beta or MCTS
│
├─ Game AI (behavior)?
│  └─ YES → Recursive Behavior Tree
│
├─ Collision detection/Spatial queries?
│  └─ YES → Recursive Spatial Partitioning (Quadtree/Octree)
│
├─ Scene management/Game object hierarchy?
│  └─ YES → Recursive Scene Graph
│
├─ Cellular automata?
│  └─ YES → HashLife
│
├─ Collision detection/Spatial queries?
│  └─ YES → Recursive Spatial Partitioning (Quadtree/Octree)
│
├─ Game AI behavior?
│  └─ YES → Recursive Behavior Trees
│
├─ Character animation?
│  └─ YES → Recursive Skeletal Animation
│
├─ Large terrain/Open world?
│  └─ YES → Recursive LOD System
│
├─ Scene management/Game object hierarchy?
│  └─ YES → Recursive Scene Graph
│
├─ Complex state management?
│  └─ YES → Recursive State Machine
│
├─ Editor/Undo system?
│  └─ YES → Recursive Undo/Redo System
│
├─ Branching dialogue/Narrative?
│  └─ YES → Recursive Dialogue Tree
│
├─ Animation blending/Layering?
│  └─ YES → Recursive Animation Blending
│
├─ Particle effects/Visual effects?
│  └─ YES → Recursive Particle System
│
├─ Large matrices/Cache optimization?
│  └─ YES → Cache-Oblivious or Recursive Linear Algebra
│
├─ Compression/Encoding?
│  └─ YES → Recursive Indexing
│
├─ Graph analysis (SCC, bridges)?
│  └─ YES → Advanced Recursive Graph Algorithms
│
├─ Game AI/Adversarial search?
│  └─ YES → Minimax/Alpha-Beta or MCTS
│
├─ Cellular automata/Game of Life?
│  └─ YES → Hashlife
│
├─ Pathfinding/Maze solving?
│  └─ YES → Recursive Maze Solving
│
├─ Tree traversal?
│  └─ YES → Tree Recursion
│
├─ Tail recursive?
│  └─ YES → Tail Recursion Optimization
│
├─ Query planning?
│  └─ YES → Query Planner Recursion
│
└─ General recursion?
   └─ YES → Tree Recursion or Tail Recursion
```

### Variant Comparison

| Variant | Best For | Key Feature | Space Complexity |
|---------|----------|-------------|------------------|
| Knuth's Algorithm X | Exact cover, Sudoku | Dancing links backtracking | O(n) constraints |
| Levinson Recursion | Toeplitz matrices | O(n²) instead of O(n³) | O(n) vectors |
| Continuation Passing Style | Stack safety, control flow | Explicit continuations | O(1) stack |
| Trampoline Recursion | Deep recursion | Thunk-based iteration | O(1) stack, O(n) heap |
| Mutual Recursion | Grammar parsing, state machines | Functions calling each other | O(d) depth |
| Co-recursion | Infinite sequences | Lazy generators | O(n) memoization |
| Advanced Memoization | Overlapping subproblems | LRU cache, decorators | O(n) cache |
| Recursive Descent + Backtracking | PEG parsing | Packrat memoization | O(n) memo table |
| Recursive Descent | Parsing | Top-down parsing | O(d) recursion depth |
| Tree Recursion | Tree problems | Multiple recursive calls | O(h) tree height |
| Tail Recursion | Iterative problems | Last operation recursive | O(1) optimized |
| Query Planner | Database queries | Join ordering | O(n) query size |
| Minimax Alpha-Beta | Game AI | Optimal play with pruning | O(b^(d/2)) |
| Monte Carlo Tree Search | Game AI | UCT with simulations | O(n) simulations |
| HashLife | Cellular automata | Memoized quad-tree | O(log n) per gen |
| Recursive Pathfinding | Game pathfinding | A*, IDA* algorithms | O(|V| log |V|) |
| Recursive Procedural Gen | Level generation | BSP, maze, terrain | O(n log n) |
| Recursive Spatial Partitioning | Collision detection | Quadtree/Octree | O(log n) queries |
| Recursive Behavior Trees | Game AI | Hierarchical decision making | O(n) tree depth |
| Recursive Skeletal Animation | Character animation | Bone hierarchy traversal | O(n) bones |
| Recursive LOD System | Terrain rendering | Adaptive detail | O(log n) subdivision |
| Recursive Scene Graph | Scene management | Hierarchical objects | O(n) nodes |
| Recursive State Machine | Game AI/State management | Hierarchical states | O(h) depth |
| Recursive Undo/Redo | Game editors | Command history | O(n) history |
| Recursive Dialogue Tree | Narrative systems | Branching dialogue | O(n) depth |
| Recursive Animation Blending | Animation systems | Layered blending | O(n) layers |
| Recursive Particle System | Visual effects | Nested emitters | O(n) particles |
| Cache-Oblivious | Large matrices | Memory hierarchy aware | O(n²) but cache-friendly |
| Recursive Indexing | Compression | Recursive encoding | O(log n) encoding |
| Recursive Linear Algebra | Matrix ops | ReLAPACK-style | O(n³) but cache-optimized |
| Advanced Graph | Graph analysis | SCC, bridges, cycles | O(V + E) |
| Minimax/Alpha-Beta | Game AI | Adversarial search | O(b^(d/2)) with pruning |
| Monte Carlo Tree Search | Game AI | UCT algorithm | O(n) simulations |
| Hashlife | Cellular automata | Memoized simulation | O(log n) per generation |
| Recursive Maze Solving | Pathfinding | DFS/BFS recursive | O(V + E) |

## Detailed Variant Selection

### 1. Recursive Descent Parser

**When to Use**:
- LL(1) or LL(k) grammars
- Expression parsing
- Language parsers
- Compiler frontends

**Key Characteristics**:
- Each grammar rule = function
- Top-down parsing
- Natural for LL grammars
- Error recovery support

**Real-World Examples**:
- Clang/LLVM parser
- Many language parsers
- Expression evaluators

### 2. Tree Recursion

**When to Use**:
- Tree data structures
- Divide and conquer
- Problems with overlapping subproblems
- Tree traversal

**Key Characteristics**:
- Multiple recursive calls
- Tree-shaped call structure
- Memoization applicable
- Natural for trees

**Real-World Examples**:
- Compiler AST traversal
- File system traversal
- Tree algorithms
- Dynamic programming

### 3. Tail Recursion

**When to Use**:
- Last operation is recursive call
- Stack space limited
- Functional programming style
- Iterative algorithms written recursively

**Key Characteristics**:
- Tail call optimization
- O(1) space with optimization
- Same performance as iteration
- More readable than iteration

**Real-World Examples**:
- Functional languages
- Compiler optimizations
- Stack-constrained environments

### 4. Query Planner Recursion

**When to Use**:
- Database query optimization
- Join ordering
- Cost estimation
- Plan generation

**Key Characteristics**:
- Recursive query planning
- Dynamic programming
- Cost-based optimization
- Join enumeration

**Real-World Examples**:
- PostgreSQL query planner
- SQLite query planner
- Database optimizers

### 5. Knuth's Algorithm X (Dancing Links)

**When to Use**:
- Exact cover problems
- Constraint satisfaction problems
- Sudoku solving
- N-Queens problem
- Polyomino tiling

**Key Characteristics**:
- Dancing links: Doubly-linked circular lists
- O(1) undo operations
- Efficient backtracking
- Recursive depth-first search

**Real-World Examples**:
- Sudoku solvers
- Constraint solvers
- Combinatorial optimization

**Source**: "Dancing Links" by Donald Knuth, The Art of Computer Programming

### 6. Levinson Recursion

**When to Use**:
- Toeplitz matrix systems
- Autoregressive (AR) model estimation
- Linear prediction coefficients
- Signal processing applications
- Time series analysis

**Key Characteristics**:
- O(n²) instead of O(n³) for Gaussian elimination
- Recursive solution construction
- Exploits Toeplitz structure

**Real-World Examples**:
- Speech coding (LPC)
- Audio compression
- Time series forecasting
- Signal filtering

**Source**: "The Wiener RMS Error Criterion" by Norman Levinson (1947)

### 7. Continuation Passing Style (CPS)

**When to Use**:
- Converting to tail-recursive form
- Implementing exception handling
- Building interpreters and compilers
- Non-local control flow
- Stack-safe recursion

**Key Characteristics**:
- Explicit control flow via continuations
- All calls become tail calls
- Stack safety
- Exception handling via continuations

**Real-World Examples**:
- Scheme interpreters (call/cc)
- Compiler intermediate representations
- Functional language implementations
- Async/await implementations

### 8. Trampoline Recursion

**When to Use**:
- Deep recursion that may cause stack overflow
- Languages without tail call optimization
- Converting recursive to iterative
- Stack-constrained environments

**Key Characteristics**:
- Converts recursion to iteration
- Thunk-based evaluation
- No compiler support needed
- Pure library implementation

**Real-World Examples**:
- Scala standard library
- JavaScript functional libraries
- Python functional programming
- Compiler implementations

### 9. Mutual Recursion

**When to Use**:
- Mutually recursive data structures
- Grammar parsing with mutually recursive rules
- State machines with recursive states
- Problems with natural even/odd structure

**Key Characteristics**:
- Functions call each other recursively
- Natural for grammar rules
- State machine transitions

**Real-World Examples**:
- Compiler parsers (expression/statement)
- Interpreter implementations
- Grammar-based code generation
- State machine implementations

### 10. Co-recursion

**When to Use**:
- Generating infinite sequences
- Lazy evaluation patterns
- Stream processing
- Generator functions
- Memoized sequences

**Key Characteristics**:
- Lazy evaluation
- Infinite data structures
- Generate values on demand
- Memoization support

**Real-World Examples**:
- Haskell lazy lists
- Python generators
- Scala streams
- Functional reactive programming

### 11. Advanced Memoization

**When to Use**:
- Functions with overlapping subproblems
- Expensive recursive computations
- Dynamic programming optimizations
- Repeated computations with same inputs

**Key Characteristics**:
- Automatic memoization decorators
- LRU cache with eviction
- Custom hash functions
- Bounded and unbounded caches

**Real-World Examples**:
- Python functools.lru_cache
- JavaScript memoization libraries
- Dynamic programming solutions
- Compiler optimizations

### 12. Recursive Descent with Backtracking (PEG)

**When to Use**:
- Parsing Expression Grammars (PEG)
- Parser combinators
- Expression parsing with precedence
- Packrat parsing

**Key Characteristics**:
- Packrat parsing with memoization
- Ordered choice (first match wins)
- O(n) time with memoization
- Prevents exponential backtracking

**Real-World Examples**:
- PEG parser generators (PEG.js, pyparsing)
- Parser combinators (Parsec, attoparsec)
- Language implementations

### 13. Cache-Oblivious Recursive Algorithms

**When to Use**:
- Large data structures that don't fit in cache
- Matrix operations (multiplication, transpose)
- Sorting large arrays
- High-performance computing applications

**Key Characteristics**:
- No knowledge of cache parameters needed
- Memory hierarchy aware
- Recursive blocking for cache locality
- Better cache behavior than iterative blocked algorithms

**Real-World Examples**:
- BLAS/LAPACK libraries
- High-performance matrix libraries
- Database systems
- Scientific computing

### 14. Recursive Indexing

**When to Use**:
- Run-length encoding with large runs
- Encoding large numeric values with small alphabet
- Data compression
- Sparse data representation

**Key Characteristics**:
- Recursive encoding of large values
- Efficient for repetitive patterns
- Used in compression algorithms

**Real-World Examples**:
- Run-length encoding systems
- Data compression algorithms
- Image compression
- Sparse matrix encoding

### 15. Recursive Linear Algebra (ReLAPACK-style)

**When to Use**:
- Dense linear algebra operations
- Matrix multiplication, factorization
- High-performance computing
- When cache performance matters

**Key Characteristics**:
- Recursive blocking for cache locality
- Tuning-free algorithms
- Better than traditional blocked algorithms
- Exploits memory hierarchy automatically

**Real-World Examples**:
- ReLAPACK library
- BLAS/LAPACK implementations
- High-performance matrix libraries
- Scientific computing frameworks

**Source**: "Recursive Algorithms for Dense Linear Algebra" (ReLAPACK), arXiv:1602.06763

### 16. Advanced Recursive Graph Algorithms

**When to Use**:
- Graph traversal with advanced requirements
- Finding cycles, components, critical nodes
- Network analysis
- Compiler dependency resolution

**Key Characteristics**:
- Tarjan's algorithm for SCC
- Articulation points and bridges
- Recursive path finding
- Cycle detection

**Real-World Examples**:
- Compiler dependency graphs
- Network routing algorithms
- Social network analysis
- Web crawlers

### 17. Minimax with Alpha-Beta Pruning (Game Development)

**When to Use**:
- Two-player zero-sum games
- Turn-based games with perfect information
- Game AI development
- Adversarial search problems

**Key Characteristics**:
- Optimal play assuming opponent is optimal
- Alpha-beta pruning reduces search space
- Recursive game tree evaluation
- Dramatically faster than naive minimax

**Real-World Examples**:
- Chess engines (Stockfish, etc.)
- Checkers AI
- Tic-tac-toe solvers
- Connect Four AI

### 18. Monte Carlo Tree Search (MCTS) - Game Development

**When to Use**:
- Games with large state spaces
- Games where evaluation is expensive
- Real-time strategy games
- Board games (Go, Chess, etc.)

**Key Characteristics**:
- UCT algorithm: Upper Confidence Bound applied to Trees
- Monte Carlo simulations for evaluation
- Recursive tree building
- Balances exploration and exploitation

**Real-World Examples**:
- AlphaGo (Google DeepMind)
- Chess engines
- Game AI frameworks
- Real-time strategy game AI

### 19. Hashlife Algorithm - Game Development

**When to Use**:
- Conway's Game of Life simulation
- Cellular automata
- Pattern evolution over many generations
- Long-term simulation of grid-based systems

**Key Characteristics**:
- Memoization of computed states
- Recursive quad-tree structure
- Time skipping for efficiency
- Dramatically faster than naive simulation

**Real-World Examples**:
- Game of Life simulators
- Cellular automata research
- Pattern analysis tools

### 20. Recursive Pathfinding (A*, IDA*) - Game Development

**When to Use**:
- Game AI pathfinding
- NPC navigation
- Route planning in games
- Real-time strategy pathfinding
- Grid-based movement systems

**Key Characteristics**:
- A* algorithm: Optimal pathfinding with heuristics
- IDA*: Memory-efficient iterative deepening A*
- Recursive path reconstruction
- Heuristic-guided search

**Real-World Examples**:
- Game engines (Unity, Unreal)
- RTS game pathfinding
- RPG NPC movement
- Strategy game unit movement
- Tower defense pathfinding

**Source**: A* algorithm by Peter Hart, Nils Nilsson, and Bertram Raphael (1968)

### 21. Recursive Procedural Generation - Game Development

**When to Use**:
- Procedural dungeon generation
- Terrain generation
- Level generation
- Random map creation
- Roguelike game development

**Key Characteristics**:
- Binary Space Partitioning (BSP): Recursive space division
- Recursive maze generation: Backtracking algorithm
- Fractal terrain: Midpoint displacement
- Recursive room placement

**Real-World Examples**:
- Roguelike games (Dwarf Fortress, Nethack)
- Procedural games (Minecraft, No Man's Sky)
- Level generators
- Terrain systems
- Random map generation

### 22. Recursive Spatial Partitioning (Quadtree/Octree) - Game Development

**When to Use**:
- Collision detection in games
- Frustum culling
- Spatial queries (find objects in region)
- Physics engines
- Rendering optimization

**Key Characteristics**:
- Quadtree: 2D space subdivision into 4 quadrants
- Octree: 3D space subdivision into 8 octants
- Recursive subdivision until threshold
- Efficient collision detection: Only check nearby objects

**Real-World Examples**:
- Game engines (Unity, Unreal Engine)
- Physics engines (Box2D, Bullet Physics)
- Collision detection systems
- Spatial indexing

**Source**: Game engine spatial partitioning techniques

### 23. Recursive Behavior Tree - Game Development

**When to Use**:
- Game AI systems
- NPC behavior
- Enemy AI
- Decision making systems
- State machines alternative

**Key Characteristics**:
- Hierarchical AI decision structure
- Composite nodes: Sequence, Selector, Parallel
- Decorator nodes: Modify child behavior
- Leaf nodes: Actions and conditions
- Recursive execution

**Real-World Examples**:
- Halo series (Bungie)
- Spore (Maxis)
- Unreal Engine behavior trees
- Unity behavior trees
- Game AI frameworks

**Source**: Game AI research and industry practices

### 24. Recursive Scene Graph - Game Development

**When to Use**:
- Game object hierarchies
- Scene management
- Transform systems
- Rendering pipelines
- Culling systems

**Key Characteristics**:
- Hierarchical representation of game objects
- Recursive traversal: Update, render, cull
- Transform inheritance: Child transforms relative to parent
- Culling optimization: Recursive frustum culling

**Real-World Examples**:
- Unity Engine
- Unreal Engine
- OpenGL scene graphs
- DirectX scene management
- Custom game engines

**Source**: Computer graphics and game engine architecture

### 25. Recursive Skeletal Animation - Game Development

**When to Use**:
- Character animation
- Skeletal rigging
- Bone-based animation
- IK/FK systems
- Animation blending

**Key Characteristics**:
- Bone hierarchy: Parent-child relationships
- Recursive transformation: Apply parent transforms to children
- Forward kinematics: Calculate end effector from joint angles
- Inverse kinematics: Calculate joint angles from end effector
- Recursive bone traversal

**Real-World Examples**:
- Game engines (Unity, Unreal)
- 3D animation software (Blender, Maya)
- Character animation systems
- Skeletal mesh animation
- Procedural animation

**Source**: Computer graphics, animation systems

### 26. Recursive LOD System - Game Development

**When to Use**:
- Large-scale terrain rendering
- Open world games
- Adaptive mesh refinement
- Chunk-based world systems
- Performance optimization

**Key Characteristics**:
- Adaptive detail: More detail near camera, less far away
- Recursive subdivision: Divide terrain/geometry recursively
- Chunk-based systems: Divide world into chunks
- Frustum culling: Recursively cull invisible regions
- Distance-based LOD selection

**Real-World Examples**:
- Minecraft chunk system
- Open world games (GTA, Skyrim)
- Terrain rendering engines
- Large-scale game worlds
- Procedural generation systems

**Source**: Computer graphics, terrain rendering research

### 27. Recursive Scene Graph - Game Development

**When to Use**:
- Scene management systems
- Game object hierarchies
- Rendering systems
- Transform hierarchies
- UI systems

**Key Characteristics**:
- Hierarchical scene organization: Parent-child relationships
- Recursive transformation: Apply parent transforms to children
- Recursive rendering: Traverse and render scene objects
- Recursive culling: Cull invisible objects recursively
- Recursive node search

**Real-World Examples**:
- Game engines (Unity, Unreal Engine)
- 3D graphics engines
- Scene management systems
- Game object systems
- UI frameworks

**Source**: Computer graphics, scene graph pattern

### 28. Recursive State Machine - Game Development

**When to Use**:
- Complex game AI states
- Character state management
- UI state machines
- Game flow control
- Nested state systems

**Key Characteristics**:
- Hierarchical states: States can contain substates
- Recursive state transitions: Handle nested state changes
- State inheritance: Child states inherit parent behavior
- Recursive event handling: Events propagate through hierarchy
- Recursive state updates

**Real-World Examples**:
- Game AI systems
- Character controllers
- UI frameworks
- Game state management
- Animation state machines

**Source**: State machine pattern, hierarchical state machines

### 29. Recursive Undo/Redo System - Game Development

**When to Use**:
- Game editors
- Level editors
- Undo/redo functionality
- Command history
- Transaction systems

**Key Characteristics**:
- Command pattern: Encapsulate operations as commands
- Recursive undo: Undo composite commands recursively
- Command grouping: Group commands for atomic operations
- Macro commands: Execute multiple commands as one
- Undo/redo stacks

**Real-World Examples**:
- Game level editors
- 3D modeling software
- Game development tools
- UI frameworks with undo
- Version control systems

**Source**: Command pattern, undo/redo systems

### 30. Recursive Dialogue Tree - Game Development

**When to Use**:
- Branching dialogue systems
- Interactive narratives
- RPG dialogue systems
- Visual novels
- Story-driven games

**Key Characteristics**:
- Branching narratives: Each dialogue node can have multiple responses
- Recursive traversal: Navigate dialogue tree recursively
- Dynamic dialogue: Dialogue adapts based on player choices
- Condition evaluation: Recursively check conditions for dialogue options
- Dialogue history tracking

**Real-World Examples**:
- RPG games (Mass Effect, Dragon Age)
- Visual novels
- Interactive fiction
- Adventure games
- Narrative-driven games

**Source**: Interactive fiction, branching narrative systems

### 31. Recursive Animation Blending - Game Development

**When to Use**:
- Character animation systems
- Animation state machines
- Procedural animation
- Animation layering
- Smooth animation transitions

**Key Characteristics**:
- Layered animation: Blend multiple animation layers recursively
- Additive blending: Add animations on top of base animations
- Recursive interpolation: Blend between animation states recursively
- Animation trees: Hierarchical animation blending
- Weight-based blending

**Real-World Examples**:
- Game engines (Unity, Unreal)
- Character animation systems
- Animation middleware
- Motion capture systems
- Procedural animation

**Source**: Computer animation, animation blending techniques

### 32. Recursive Particle System - Game Development

**When to Use**:
- Visual effects systems
- Particle effects
- Explosion effects
- Fire and smoke effects
- Magic spell effects

**Key Characteristics**:
- Particle emitters: Recursively spawn particles
- Nested emitters: Particles can spawn other particles
- Recursive updates: Update particle hierarchies recursively
- Particle trails: Recursive trail generation
- Hierarchical particle management

**Real-World Examples**:
- Game engines (Unity, Unreal)
- Visual effects systems
- Particle middleware
- Special effects in games
- Environmental effects

**Source**: Computer graphics, particle systems

### 33. Recursive Maze Solving - Game Development

**When to Use**:
- Maze solving in games
- Pathfinding algorithms
- Procedural maze generation
- Game AI navigation

**Key Characteristics**:
- Recursive backtracking
- Multiple algorithms (DFS, BFS variants)
- Path reconstruction
- Natural for maze navigation

**Real-World Examples**:
- Game pathfinding systems
- Maze games
- Procedural level generation
- AI navigation

## Performance Characteristics

### Time Complexity Comparison

| Variant | Time Complexity | Notes |
|---------|----------------|-------|
| Knuth's Algorithm X | Exponential worst case | Very efficient in practice |
| Levinson Recursion | O(n²) | Instead of O(n³) for general systems |
| Continuation Passing Style | Same as original | All tail calls |
| Trampoline Recursion | Same as original | Converted to iteration |
| Mutual Recursion | Depends on problem | Same as equivalent recursion |
| Co-recursion | O(1) per value | Amortized |
| Advanced Memoization | O(1) lookup | After first computation |
| Recursive Descent + Backtracking | O(n) with memo | O(2^n) without memo |
| Recursive Descent | O(n) | n tokens |
| Tree Recursion | O(2^n) without memo | O(n) with memo |
| Tail Recursion | Same as iteration | O(n) typically |
| Query Planner | O(2^n) worst case | O(n^3) with DP |
| Cache-Oblivious | Same as standard | Better cache behavior |
| Recursive Indexing | O(log n) | n is value size |
| Recursive Linear Algebra | O(n³) | But cache-optimized |
| Advanced Graph | O(V + E) | V vertices, E edges |
| Minimax/Alpha-Beta | O(b^(d/2)) | b branching, d depth |
| Monte Carlo Tree Search | O(n) | n simulations |
| Hashlife | O(log n) per generation | For stable patterns |
| Recursive Maze Solving | O(V + E) | V cells, E connections |

### Space Complexity Comparison

| Variant | Space Complexity | Notes |
|---------|------------------|-------|
| Knuth's Algorithm X | O(n) | n constraints |
| Levinson Recursion | O(n) | Vectors |
| Continuation Passing Style | O(1) stack | O(n) heap for closures |
| Trampoline Recursion | O(1) stack | O(n) heap for thunks |
| Mutual Recursion | O(d) | d recursion depth |
| Co-recursion | O(n) | For memoization |
| Advanced Memoization | O(n) | n unique inputs |
| Recursive Descent + Backtracking | O(n) | Memoization table |
| Recursive Descent | O(d) | d recursion depth |
| Tree Recursion | O(h) | h tree height |
| Tail Recursion | O(1) optimized | O(n) without optimization |
| Query Planner | O(n) | n query size |
| Cache-Oblivious | O(n²) | But cache-friendly |
| Recursive Indexing | O(log n) | For encoding |
| Recursive Linear Algebra | O(n²) | But cache-optimized |
| Advanced Graph | O(V) | V vertices |
| Minimax/Alpha-Beta | O(d) | d recursion depth |
| Monte Carlo Tree Search | O(n) | n tree nodes |
| Hashlife | O(n) | For memoization |
| Recursive Maze Solving | O(V) | V cells |

## Use Case Mapping

### Parsing
- **Best Choice**: Recursive Descent Parser
- **Reason**: Natural for LL grammars
- **Alternatives**: LR parser (if grammar requires)

### Tree Problems
- **Best Choice**: Tree Recursion
- **Reason**: Natural tree structure
- **Alternatives**: Iterative with stack (if stack space limited)

### Iterative Problems
- **Best Choice**: Tail Recursion
- **Reason**: O(1) space with optimization
- **Alternatives**: Iteration (if optimization not available)

### Query Optimization
- **Best Choice**: Query Planner Recursion
- **Reason**: Natural for join ordering
- **Alternatives**: Dynamic programming (if applicable)

## Key Patterns Extracted

### Pattern 1: Recursive Descent
- **Found in**: LLVM parser, GCC parser
- **Technique**: Each grammar rule = function
- **Benefit**: Natural, easy to understand
- **Trade-off**: Limited to LL grammars

### Pattern 2: Memoization
- **Found in**: Tree recursion
- **Technique**: Cache recursive results
- **Benefit**: Eliminates redundant computation
- **Trade-off**: Memory overhead

### Pattern 3: Tail Call Optimization
- **Found in**: Tail recursion
- **Technique**: Convert tail calls to iteration
- **Benefit**: O(1) space complexity
- **Trade-off**: Requires compiler support

### Pattern 4: Divide and Conquer
- **Found in**: Tree recursion
- **Technique**: Split problem into subproblems
- **Benefit**: Natural problem decomposition
- **Trade-off**: Overhead for splitting

### Pattern 5: Dancing Links
- **Found in**: Knuth's Algorithm X
- **Technique**: Doubly-linked circular lists for backtracking
- **Benefit**: O(1) undo operations
- **Trade-off**: More complex data structure

### Pattern 6: Packrat Parsing
- **Found in**: PEG parsers
- **Technique**: Memoization prevents exponential backtracking
- **Benefit**: O(n) time for unambiguous grammars
- **Trade-off**: O(n) space for memoization table

### Pattern 7: Continuation Passing
- **Found in**: Functional languages, compilers
- **Technique**: Pass continuation function instead of returning
- **Benefit**: Stack safety, explicit control flow
- **Trade-off**: More complex code structure

### Pattern 8: Trampoline Pattern
- **Found in**: Functional programming libraries
- **Technique**: Convert recursion to iteration using thunks
- **Benefit**: Stack safety without compiler support
- **Trade-off**: Heap allocation for thunks

### Pattern 9: LRU Memoization
- **Found in**: Advanced memoization patterns
- **Technique**: Bounded cache with least-recently-used eviction
- **Benefit**: Memory-efficient memoization
- **Trade-off**: May evict useful entries

### Pattern 10: Cache-Oblivious Recursion
- **Found in**: High-performance computing libraries
- **Technique**: Recursive blocking for cache locality
- **Benefit**: Automatic cache optimization
- **Trade-off**: More complex implementation

### Pattern 11: Recursive Indexing
- **Found in**: Compression algorithms
- **Technique**: Recursive encoding of large values
- **Benefit**: Efficient encoding with small alphabet
- **Trade-off**: Encoding overhead

### Pattern 12: UCT Algorithm (MCTS)
- **Found in**: Game AI (AlphaGo, etc.)
- **Technique**: Upper Confidence Bound for Trees
- **Benefit**: Balances exploration and exploitation
- **Trade-off**: Requires many simulations

### Pattern 13: Alpha-Beta Pruning
- **Found in**: Game AI (Chess engines, etc.)
- **Technique**: Prunes branches that can't affect result
- **Benefit**: Dramatically reduces search space
- **Trade-off**: Requires good move ordering

## Real-World Examples

### Compiler Parsing
- **Pattern**: Recursive Descent Parser
- **Usage**: Clang/LLVM, GCC parsers
- **Why**: Natural for LL grammars

### Tree Algorithms
- **Pattern**: Tree Recursion
- **Usage**: AST traversal, file systems
- **Why**: Natural tree structure

### Functional Programming
- **Pattern**: Tail Recursion
- **Usage**: Scheme, Haskell
- **Why**: Stack optimization

### Database Optimization
- **Pattern**: Query Planner Recursion
- **Usage**: PostgreSQL, SQLite
- **Why**: Natural for join ordering

### Game AI
- **Pattern**: Minimax/Alpha-Beta, MCTS
- **Usage**: Chess engines, Go AI, game frameworks
- **Why**: Optimal decision-making in games

### High-Performance Computing
- **Pattern**: Cache-Oblivious, Recursive Linear Algebra
- **Usage**: BLAS/LAPACK, scientific computing
- **Why**: Better cache performance than iterative blocked algorithms

### Cellular Automata
- **Pattern**: Hashlife
- **Usage**: Game of Life simulators
- **Why**: Dramatically faster than naive simulation

## References

### Production Codebases
- LLVM: https://github.com/llvm/llvm-project
- PostgreSQL: https://github.com/postgres/postgres
- Redis: https://github.com/redis/redis
- V8/Chromium: https://github.com/chromium/chromium

### Research Papers
- "Dancing Links" by Donald Knuth, The Art of Computer Programming, Volume 4B
- "The Wiener RMS Error Criterion in Filter Design and Prediction" by Norman Levinson (1947)
- "Packrat Parsing: Simple, Powerful, Lazy, Linear Time" by Bryan Ford (2002)
- "Recursive Algorithms for Dense Linear Algebra" (ReLAPACK), arXiv:1602.06763
- "Monte Carlo Tree Search" - UCT algorithm research
- "Hashlife" by Bill Gosper

### Books and Textbooks
- "The Art of Computer Programming" by Donald Knuth
- "Introduction to Algorithms" (CLRS)
- "Compilers: Principles, Techniques, and Tools" (Dragon Book)
- "Structure and Interpretation of Computer Programs" (SICP)

### Online Resources
- Parsing Expression Grammars: https://en.wikipedia.org/wiki/Parsing_expression_grammar
- Continuation Passing Style: Functional programming resources
- Trampoline Pattern: Scala, JavaScript functional programming

