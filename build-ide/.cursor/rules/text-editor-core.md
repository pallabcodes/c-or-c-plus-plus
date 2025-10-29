# Text Editor Core Standards

## Scope
Applies to all text editor core code including text buffers, editing operations, and undo/redo mechanisms. Extends repository root rules.

## Text Buffer Data Structures

### Gap Buffer
* Gap buffer for efficient insertion at cursor
* Gap represents editable region
* Move gap to cursor position for edits
* Reference: "Gap Buffers for Fast Text Editing" (Larus, 1988)
* Used by Vim for text representation
* O(n) worst case but O(1) amortized for typical editing

### Piece Table
* Piece table for efficient editing
* Original and add buffers
* Piece list with references
* Reference: Used by Sublime Text
* Efficient for large files
* Good for undo/redo support

### Rope Data Structure
* Balanced binary tree of strings
* Efficient concatenation and splitting
* Log time operations
* Reference: "Efficient Text Editing with Rope Data Structures" (Boehm et al.)
* Used by Xi editor and others
* Memory efficient for large documents

## Edit Operations

### Insert Operations
* Insert text at cursor position
* Handle multi line insertion
* Update line number mappings
* Trigger change notifications

### Delete Operations
* Delete characters or ranges
* Handle backspace and forward delete
* Update line number mappings
* Merge adjacent deletions when possible

### Replace Operations
* Replace selected text
* Handle multi selection replacements
* Batch multiple edits together
* Efficient for find and replace operations

## Undo/Redo Mechanisms

### Operation History
* Maintain operation history stack
* Undo stack and redo stack
* Operation serialization
* Memory efficient storage

### Undo Tree
* Tree structure for branching undo
* Support multiple undo paths
* Merge strategies for linear editing
* Persistent undo across sessions

### Operation Merging
* Merge consecutive typing operations
* Merge consecutive deletions
* Configurable merge distance
* Preserve meaningful edit boundaries

## Line Management

### Line Number Tracking
* Efficient line number calculation
* Incremental updates on edits
* Cache line start positions
* Handle very long lines

### Line Ending Handling
* Support CRLF, LF, and CR
* Auto detection of line endings
* Conversion on save
* Preserve original line endings

## Implementation Requirements
* Efficient memory usage for large files
* Fast cursor movement and positioning
* Handle files larger than memory
* Proper encoding support (UTF 8, etc.)
* Thread safety for concurrent access
* Change notifications for UI updates

## Performance Considerations
* Minimize memory allocations
* Batch edit operations when possible
* Lazy evaluation of line numbers
* Incremental updates for UI
* Profile edit latency

