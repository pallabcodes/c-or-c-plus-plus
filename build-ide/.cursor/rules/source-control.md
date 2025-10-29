# Source Control Standards

## Scope
Applies to all source control code including Git integration, version control operations, and diff visualization. Extends repository root rules.

## Git Integration

### Git Operations
* Status detection and display
* Commit operations
* Branch management
* Merge and rebase operations
* Reference: Magit (Emacs Git interface)

### Git Status
* Track file status (modified, added, deleted)
* Staged vs unstaged changes
* Untracked files
* Conflict detection

### History Operations
* View commit history
* Navigate commits
* Blame annotation
* Commit diff viewing

## Diff Algorithms

### Diff Generation
* Myers diff algorithm
* Patience diff algorithm
* Histogram diff algorithm
* Unified diff format
* Side by side diff view

### Diff Visualization
* Inline diff display
* Unified diff view
* Three way merge view
* Hunk navigation
* Reference: "An O(ND) Difference Algorithm" (Myers, 1986)

### Merge Conflict Resolution
* Conflict detection
* Conflict markers
* Three way merge visualization
* Conflict resolution tools
* Automatic conflict resolution where possible

## Change Tracking

### File Change Tracking
* Monitor file system changes
* Detect modifications
* Track renames
* Handle file deletions

### Repository State
* Current branch tracking
* Remote tracking
* Tag information
* Commit state

## Implementation Requirements
* Efficient Git operations
* Async Git command execution
* Proper error handling
* Handle large repositories
* Background processing
* Cache Git status

## Performance Considerations
* Lazy status checking
* Incremental updates
* Cache repository state
* Minimize Git command calls
* Efficient diff generation
* Parallel operations

## Integration Points
* Editor component integration
* Diff view integration
* Extension system integration
* UI component integration

