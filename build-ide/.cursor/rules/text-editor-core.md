# Text Editor Core Standards

## Overview
The text editor core is the foundation of any IDE. This document defines standards for implementing production grade text buffer management, editing operations, and undo/redo mechanisms that match the quality of top tier IDEs like VSCode, IntelliJ IDEA, Vim, Emacs, and Sublime Text.

## Scope
* Applies to all text editor core code including text buffers, editing operations, and undo/redo systems
* Extends repository root rules defined in the root `.cursor/rules/` files
* Covers all aspects of text editing from basic buffer management to advanced undo systems
* Code quality standards align with expectations from top tier IDE companies like Microsoft, JetBrains, and others

## Top Tier IDE Comparisons

### Visual Studio Code
* Uses piece table data structure for efficient editing
* Virtual scrolling for large files
* Incremental parsing with Tree-sitter
* Fast edit operations (sub-millisecond)
* Efficient memory usage

### IntelliJ IDEA
* Custom text buffer implementation
* Efficient line tracking
* Advanced undo/redo system
* Background processing for large files
* Memory optimized for large codebases

### Vim/Neovim
* Gap buffer implementation for sequential editing
* Efficient for modal editing patterns
* Minimal memory footprint
* Fast cursor movement operations
* Highly optimized for text editing

### Emacs
* Gap buffer with efficient gap management
* Advanced undo tree system
* Efficient text representation
* Support for very large files
* Memory efficient operations

### Sublime Text
* Piece table data structure
* Fast editing operations
* Efficient for large files
* Low memory usage
* High performance rendering

## Text Buffer Data Structures

### Gap Buffer
* **Structure**: Array with gap for efficient insertion
* **Complexity**: O(n) worst case, O(1) amortized for sequential edits
* **Applications**: Vim, simple text editors
* **Trade-offs**: Fast sequential edits, slower random access
* **Memory**: O(n) where n is document size
* **Rationale**: Efficient for sequential editing patterns
* **Implementation**: Array with gap, move gap to insertion point
* **Research**: "Gap Buffers for Fast Text Editing" (Larus, 1988)

### Piece Table
* **Structure**: Table of pieces referencing original and added buffers
* **Complexity**: O(log n) for edits, O(n) for full text retrieval
* **Applications**: Sublime Text, modern editors
* **Trade-offs**: Efficient for large files, more complex implementation
* **Memory**: O(n) where n is number of edits
* **Rationale**: Handles large files efficiently
* **Implementation**: Table of pieces, original buffer, added buffer
* **Research**: Piece table data structure papers

### Rope Data Structure
* **Structure**: Balanced binary tree of string fragments
* **Complexity**: O(log n) for edits, O(n) for full text retrieval
* **Applications**: Text editors requiring efficient string operations
* **Trade-offs**: Good balance of edit and retrieval performance
* **Memory**: O(n) where n is document size
* **Rationale**: Efficient for both editing and retrieval
* **Implementation**: Balanced binary tree, string fragments
* **Research**: "Efficient Text Editing with Rope Data Structures" (Boehm et al.)

## Editing Operations

### Insert Operation
* **Position validation**: Validate insertion position
* **Efficient insertion**: Use appropriate data structure
* **Line tracking**: Update line numbers
* **Complexity**: O(log n) for rope, O(1) amortized for gap buffer
* **Performance target**: Sub-millisecond for typical edits
* **Error handling**: Return error on invalid position
* **Rationale**: Fast insertion is critical for responsive editing

### Delete Operation
* **Range validation**: Validate deletion range
* **Efficient deletion**: Use appropriate data structure
* **Line tracking**: Update line numbers
* **Complexity**: O(log n) for rope, O(1) amortized for gap buffer
* **Performance target**: Sub-millisecond for typical deletions
* **Error handling**: Return error on invalid range
* **Rationale**: Fast deletion is critical for responsive editing

### Replace Operation
* **Range validation**: Validate replacement range
* **Efficient replacement**: Combine delete and insert
* **Line tracking**: Update line numbers
* **Complexity**: O(log n) for rope, O(1) amortized for gap buffer
* **Performance target**: Sub-millisecond for typical replacements
* **Error handling**: Return error on invalid range
* **Rationale**: Replace is common operation, must be fast

### Example Insert Operation
```cpp
// Thread safety: Not thread safe (caller must synchronize)
// Ownership: Caller owns buffer
// Performance: O(log n) for rope, O(1) amortized for gap buffer
// Failure modes: Returns false on invalid position or allocation failure
bool buffer_insert(TextBuffer* buffer, size_t position, const char* text, size_t length) {
    if (!buffer || !text || length == 0) {
        return false;
    }
    
    if (position > buffer->size) {
        return false;
    }
    
    return buffer_insert_internal(buffer, position, text, length);
}
```

### Example Delete Operation
```cpp
// Thread safety: Not thread safe (caller must synchronize)
// Ownership: Caller owns buffer
// Performance: O(log n) for rope, O(1) amortized for gap buffer
// Failure modes: Returns false on invalid range
bool buffer_delete(TextBuffer* buffer, size_t start, size_t end) {
    if (!buffer) {
        return false;
    }
    
    if (start >= end || end > buffer->size) {
        return false;
    }
    
    return buffer_delete_internal(buffer, start, end);
}
```

## Undo/Redo System

### Operation History
* **Store operations**: Store edit operations for undo
* **Undo stack**: Stack of operations for undo
* **Redo stack**: Stack of operations for redo
* **Complexity**: O(1) for undo/redo operations
* **Memory**: O(n) where n is number of operations
* **Rationale**: Undo/redo is essential for user experience
* **Implementation**: Two stacks, store inverse operations

### Undo Tree
* **Branching history**: Support branching undo history
* **Merge strategies**: Merge operations when possible
* **Complexity**: O(log n) for undo tree operations
* **Memory**: O(n) where n is number of operations
* **Rationale**: Advanced undo system for complex editing
* **Implementation**: Tree structure, merge compatible operations
* **Research**: Undo tree papers and implementations

### Operation Types
* **Insert operation**: Store deleted text for undo
* **Delete operation**: Store deleted text for undo
* **Replace operation**: Store old and new text
* **Batch operations**: Group related operations
* **Rationale**: Different operations need different undo strategies

## Line Management

### Line Tracking
* **Line breaks**: Track line break positions
* **Line numbers**: Efficient line number lookup
* **Line length**: Track line lengths for wrapping
* **Complexity**: O(log n) for line lookup
* **Memory**: O(n) where n is number of lines
* **Rationale**: Line tracking enables efficient line operations
* **Implementation**: Balanced tree or array of line positions

### Line Number Lookup
* **Position to line**: Convert position to line number
* **Line to position**: Convert line number to position
* **Complexity**: O(log n) for tree-based, O(1) for array-based
* **Performance target**: Sub-millisecond lookup
* **Rationale**: Fast line lookup is critical for rendering

### Line Wrapping
* **Wrap calculation**: Calculate wrapped lines
* **Wrap caching**: Cache wrap calculations
* **Complexity**: O(n) for calculation, O(1) for cached lookup
* **Performance target**: Fast wrap calculation
* **Rationale**: Line wrapping affects rendering performance

## Cursor Management

### Cursor Position
* **Position tracking**: Track cursor position
* **Position validation**: Validate cursor position
* **Position updates**: Update position on edits
* **Complexity**: O(1) for position operations
* **Rationale**: Cursor management is essential for editing

### Multi-Cursor Support
* **Multiple cursors**: Support multiple cursors
* **Cursor operations**: Operations on all cursors
* **Complexity**: O(k) where k is number of cursors
* **Rationale**: Multi-cursor editing improves productivity

## Selection Management

### Selection Range
* **Range tracking**: Track selection range
* **Range validation**: Validate selection range
* **Range updates**: Update range on edits
* **Complexity**: O(1) for range operations
* **Rationale**: Selection management is essential for editing

### Multiple Selections
* **Multiple selections**: Support multiple selections
* **Selection operations**: Operations on all selections
* **Complexity**: O(k) where k is number of selections
* **Rationale**: Multiple selections enable advanced editing

## Implementation Standards

### Correctness
* **Position validation**: Validate all positions
* **Range validation**: Validate all ranges
* **Invariants**: Maintain buffer invariants
* **Error handling**: Handle all error cases
* **Rationale**: Correctness is critical for text editing

### Performance
* **Fast operations**: Optimize edit operations
* **Memory efficiency**: Efficient memory usage
* **Large files**: Handle large files efficiently
* **Virtual scrolling**: Use virtual scrolling for rendering
* **Rationale**: Performance is critical for responsiveness

### Memory Management
* **Allocation**: Efficient memory allocation
* **Deallocation**: Proper memory deallocation
* **Memory pools**: Use memory pools for frequent allocation
* **Leak prevention**: Prevent memory leaks
* **Rationale**: Memory efficiency enables scalability

## Testing Requirements

### Unit Tests
* **Operations**: Test all editing operations
* **Edge cases**: Test boundary conditions
* **Large files**: Test with large files (100MB+)
* **Undo/redo**: Test undo/redo functionality
* **Line tracking**: Test line tracking correctness
* **Rationale**: Comprehensive testing ensures correctness

### Performance Tests
* **Edit performance**: Benchmark edit operations
* **Memory usage**: Measure memory usage
* **Large file handling**: Test with very large files
* **Scalability**: Test scalability limits
* **Rationale**: Performance tests ensure performance goals

### Stress Tests
* **Rapid edits**: Test rapid edit operations
* **Memory pressure**: Test under memory pressure
* **Concurrent access**: Test concurrent access patterns
* **Rationale**: Stress tests ensure robustness

## Research Papers and References

### Text Editing
* "Gap Buffers for Fast Text Editing" (Larus, 1988)
* "Efficient Text Editing with Rope Data Structures" (Boehm et al.)
* "The Structure of a Text Editor" (Bret Victor)
* "Piece Tables: A Data Structure for Text Editing"

### Open Source References
* Vim gap buffer implementation
* Sublime Text piece table implementation
* Atom rope implementation
* VSCode text buffer implementation

## Implementation Checklist

- [ ] Choose text buffer data structure (gap buffer, piece table, or rope)
- [ ] Implement insert operation with validation
- [ ] Implement delete operation with validation
- [ ] Implement replace operation
- [ ] Implement undo/redo system
- [ ] Implement line tracking
- [ ] Implement cursor management
- [ ] Implement selection management
- [ ] Add comprehensive error handling
- [ ] Write comprehensive unit tests
- [ ] Write performance benchmarks
- [ ] Test with large files (100MB+)
- [ ] Optimize for performance
- [ ] Document time and space complexity
- [ ] Document thread safety guarantees
