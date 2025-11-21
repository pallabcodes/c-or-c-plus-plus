# Flood Fill Pattern Recognition

## When to Recognize Flood Fill Opportunity

### Input Characteristics That Suggest Flood Fill

1. **Region Filling Tasks**
   - Filling connected areas with a new color/value
   - Bucket fill tools in image editors
   - Area selection and modification
   - Region growing algorithms

2. **Connected Component Processing**
   - Filling entire connected regions
   - Boundary-based region identification
   - Morphological operations
   - Image segmentation

3. **Interactive Applications**
   - Paint program bucket fill
   - Game level editing
   - Medical image segmentation
   - Geographic region filling

4. **Grid-Based Modifications**
   - Terrain generation and modification
   - Maze solving and path marking
   - Board game area filling
   - Cellular automaton updates

## Variant Selection Guide

### Decision Tree

```
Need flood fill?
│
├─ Memory constraints?
│  └─ YES → Iterative flood fill (queue/stack)
│
├─ Simple boundary filling?
│  └─ YES → Boundary fill (4-way connectivity)
│
├─ Performance critical?
│  └─ YES → Scanline flood fill (cache-efficient)
│
├─ Interactive/paint app?
│  └─ YES → Recursive flood fill (natural interaction)
│
├─ Large grids/depth limits?
│  └─ YES → Iterative approaches
│
└─ General filling?
   └─ YES → Choose based on connectivity and requirements
```

### Variant Comparison

| Variant | Best For | Key Feature | Time Complexity | Space Complexity |
|---------|----------|-------------|-----------------|------------------|
| Recursive Fill | Simple grids | Natural recursion | O(pixels) | O(depth) worst |
| Iterative Queue | Memory safe | Bounded memory | O(pixels) | O(width) typical |
| Iterative Stack | Memory safe | Stack-based | O(pixels) | O(width) typical |
| Boundary Fill | Boundary following | Edge detection | O(pixels) | O(width) |
| Scanline Fill | Performance | Horizontal spans | O(pixels) | O(width) |

## Detailed Variant Selection

### 1. Recursive Flood Fill

**When to Use:**
- Simple grid structures
- Interactive paint applications
- When recursion depth is manageable
- Educational implementations
- Small to medium sized areas

**Key Characteristics:**
- Natural recursive implementation
- Easy to understand and implement
- Follows connected pixels directly
- Stack overflow risk on large areas
- Good for irregular shapes

**Real-World Examples:**
- Classic paint program bucket fill
- Simple image editors
- Game level editors
- Educational algorithm demonstrations

### 2. Iterative Flood Fill (Queue-Based)

**When to Use:**
- Memory-constrained environments
- Large grid processing
- Real-time applications
- Need predictable memory usage
- Production systems

**Key Characteristics:**
- Uses queue for breadth-first exploration
- Bounded memory usage
- No recursion depth limits
- Good cache performance
- Predictable performance

**Real-World Examples:**
- Professional image editing software
- Game engines
- Real-time graphics applications
- Embedded systems

### 3. Iterative Flood Fill (Stack-Based)

**When to Use:**
- Memory efficiency is critical
- Stack-based architectures
- Embedded systems
- When queue allocation is expensive
- Depth-first filling preference

**Key Characteristics:**
- Uses explicit stack
- Memory efficient
- Mimics recursive behavior
- Good for constrained environments
- Depth-first filling order

**Real-World Examples:**
- Embedded graphics libraries
- Mobile applications
- Resource-constrained devices
- Real-time systems

### 4. Boundary Fill

**When to Use:**
- Boundary-defined regions
- Edge-based filling
- When you have boundary pixels
- Interactive boundary selection
- Medical imaging applications

**Key Characteristics:**
- Starts from boundary pixels
- Fills inward from edges
- Good for boundary-defined regions
- Natural for user-drawn boundaries
- Edge-following behavior

**Real-World Examples:**
- Medical image segmentation
- CAD boundary filling
- Interactive drawing tools
- Geographic boundary filling

### 5. Scanline Flood Fill

**When to Use:**
- Performance-critical applications
- Large image processing
- Cache-efficient processing needed
- Horizontal span optimization
- Professional graphics software

**Key Characteristics:**
- Processes horizontal spans
- Excellent cache performance
- Fastest for large areas
- Complex implementation
- Industrial-grade performance

**Real-World Examples:**
- Professional image editors (Photoshop)
- Graphics processing libraries
- High-performance computing
- Video game engines

## Performance Characteristics

### Algorithm Efficiency

| Algorithm | Time | Space | Cache Performance | Best Use Case |
|-----------|------|-------|-------------------|---------------|
| Recursive | O(N) | O(D) | Good | Simple applications |
| Queue Iterative | O(N) | O(W) | Excellent | Production software |
| Stack Iterative | O(N) | O(W) | Good | Embedded systems |
| Boundary | O(N) | O(W) | Good | Boundary-based |
| Scanline | O(N) | O(W) | Excellent | High-performance |

*Where N = total pixels, D = recursion depth, W = max width*

### Connectivity Patterns

| Connectivity | Use Case | Common In |
|--------------|----------|-----------|
| 4-way | Simple filling | Basic paint tools |
| 8-way | Diagonal connections | Image processing |
| 6-way | 3D grids | Voxel filling |
| Custom | Domain specific | Specialized applications |

## Use Case Mapping

### Paint Programs
- **Best Choice**: Recursive flood fill
- **Reason**: Natural user interaction, simple implementation
- **Alternatives**: Scanline for professional tools

### Game Development
- **Best Choice**: Iterative queue flood fill
- **Reason**: Real-time performance, predictable memory usage
- **Alternatives**: Scanline for terrain generation

### Image Processing
- **Best Choice**: Scanline flood fill
- **Reason**: Cache-efficient, high performance
- **Alternatives**: Queue iterative for general processing

### Embedded Systems
- **Best Choice**: Stack iterative flood fill
- **Reason**: Memory efficient, no dynamic allocation
- **Alternatives**: Recursive for simple cases

### Medical Imaging
- **Best Choice**: Boundary fill
- **Reason**: Works with boundary-defined regions
- **Alternatives**: Queue iterative for general segmentation

## Key Patterns Extracted

### Pattern 1: Memory-Bounded Filling
- **Found in**: Production graphics libraries
- **Technique**: Queue-based iterative approach
- **Benefit**: Predictable memory usage, no stack overflow
- **Trade-off**: Slightly more complex implementation

### Pattern 2: Span-Based Optimization
- **Found in**: Professional image editors
- **Technique**: Horizontal span processing
- **Benefit**: Excellent cache performance, fast filling
- **Trade-off**: Complex implementation, harder to understand

### Pattern 3: Boundary-First Approach
- **Found in**: Medical imaging, CAD software
- **Technique**: Start from boundary pixels
- **Benefit**: Natural for boundary-defined regions
- **Trade-off**: May miss interior pixels if boundary is broken

### Pattern 4: Connectivity Flexibility
- **Found in**: Computer graphics libraries
- **Technique**: Support multiple connectivity patterns
- **Benefit**: Adaptable to different use cases
- **Trade-off**: Parameter complexity

### Pattern 5: Early Termination
- **Found in**: Real-time applications
- **Technique**: Check bounds and visited states early
- **Benefit**: Performance optimization, avoids unnecessary work
- **Trade-off**: Slightly more complex logic

## Real-World Examples

### Paint Bucket Tool
- **Pattern**: Recursive flood fill with 4-way connectivity
- **Usage**: Interactive painting in image editors
- **Why**: Natural user interaction, simple implementation

### Photoshop Magic Wand
- **Pattern**: Iterative flood fill with tolerance
- **Usage**: Color-based selection tools
- **Why**: Precise control over fill boundaries

### Game Terrain Generation
- **Pattern**: Scanline flood fill for large areas
- **Usage**: Procedural level generation
- **Why**: Performance on large grids, cache efficiency

### Medical Image Segmentation
- **Pattern**: Boundary fill with region growing
- **Usage**: Organ identification in medical scans
- **Why**: Works with boundary-defined anatomical structures

### Embedded Graphics
- **Pattern**: Stack-based iterative flood fill
- **Usage**: Mobile device graphics libraries
- **Why**: Memory constraints, predictable performance

## References

### Production Codebases
- GIMP: https://github.com/GNOME/gimp
- ImageMagick: Graphics processing library
- OpenCV: Computer vision algorithms
- Qt Graphics: GUI framework painting

### Research Papers
- "An Efficient Flood Fill Algorithm" - Graphics Gems
- "Fast Seed Filling" - Computer Graphics
- "Scanline Flood Fill Algorithms" - Graphics libraries

### Books and Textbooks
- "Computer Graphics: Principles and Practice"
- "Digital Image Processing" by Gonzalez
- "Real-Time Rendering" (game graphics)

### Online Resources
- Graphics Gems repository
- Computer graphics algorithm collections
- Game development forums
- Image processing tutorials
