# Debugging Support Standards

## Scope
Applies to all debugging code including debugger integration, breakpoints, and variable inspection. Extends repository root rules.

## Debug Adapter Protocol (DAP)
* Protocol for debugger integration
* Language agnostic debugger interface
* Reference: Debug Adapter Protocol specification
* Used by VSCode and other IDEs
* Standard for IDE debugger support

## Breakpoint Management

### Breakpoint Types
* Line breakpoints
* Conditional breakpoints
* Logpoint breakpoints
* Exception breakpoints
* Function breakpoints

### Breakpoint Operations
* Set and remove breakpoints
* Enable and disable breakpoints
* Breakpoint persistence
* Breakpoint validation

### Breakpoint State
* Verified breakpoints
* Unverified breakpoints
* Breakpoint conditions
* Breakpoint hit counts

## Execution Control

### Control Flow
* Continue execution
* Step over, step into, step out
* Pause execution
* Restart debugging session

### Call Stack
* Display call stack
* Navigate stack frames
* Inspect stack frame variables
* Jump to stack frame

## Variable Inspection

### Variable Display
* Display variable values
* Variable scope hierarchy
* Watch expressions
* Evaluate expressions

### Data Visualization
* Complex data structure display
* Expandable tree views
* Memory visualization
* Custom visualizers

## Debugging Sessions

### Session Lifecycle
* Launch configurations
* Attach to process
* Session start and stop
* Cleanup on termination

### Multi Process Debugging
* Multiple debug sessions
* Process selection
* Inter process communication
* Synchronized debugging

## Implementation Requirements
* Efficient debugger communication
* Async operation handling
* Proper error handling
* Resource cleanup
* Thread safety
* Timeout management

## Performance Considerations
* Minimize debugger overhead
* Efficient variable querying
* Lazy loading of data
* Background processing
* Optimize defer format operation

## Integration Points
* Debug adapter integration
* Editor component integration
* UI component integration
* Extension system integration

