# Refactoring Standards

## Scope
Applies to all refactoring code including code transformations, rename operations, and extract operations. Extends repository root rules.

## Refactoring Operations

### Rename Refactoring
* Rename symbol across workspace
* Handle shadowing and scoping
* Update all references
* Preview changes before applying
* Reference: "Refactoring: Improving the Design of Existing Code" (Fowler, 1999)

### Extract Operations
* Extract method/variable
* Extract function
* Extract constant
* Extract interface
* Maintain code semantics

### Move Operations
* Move symbol to different location
* Move symbol to different file
* Update all references
* Handle namespace changes

### Inline Operations
* Inline function
* Inline variable
* Remove unused code
* Simplify code structure

## Safe Refactoring

### Validation
* Validate refactoring applicability
* Check preconditions
* Detect conflicts
* Ensure semantic preservation

### Change Preview
* Show diff preview
* Highlight affected regions
* Allow selective application
* Cancel refactoring operation

### Rollback Support
* Undo refactoring operations
* Restore original code
* Handle partial failures
* Transaction like semantics

## Refactoring Analysis

### Impact Analysis
* Find all affected code
* Analyze dependencies
* Check for conflicts
* Estimate change scope

### Safety Checks
* Type checking after refactoring
* Symbol resolution validation
* Compile time checks
* Test execution validation

## Implementation Requirements
* Accurate symbol resolution
* Efficient workspace analysis
* Reliable change application
* Proper error handling
* User feedback and progress
* Undo/redo support

## Performance Considerations
* Incremental analysis
* Background processing
* Cache analysis results
* Minimize workspace scans
* Efficient diff generation

## Integration Points
* Language server integration
* Editor component integration
* Source control integration
* Extension system integration

