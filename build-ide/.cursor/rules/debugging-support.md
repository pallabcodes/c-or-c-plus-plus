# Debugging Support Standards

## Overview
Debugging support is essential for development workflow. This document defines standards for implementing production grade debugging support including debugger integration, breakpoints, and variable inspection that matches the quality of top tier IDEs.

## Scope
* Applies to all debugging code including debugger integration, breakpoints, and variable inspection
* Extends repository root rules defined in the root `.cursor/rules/` files
* Covers all aspects of debugging from basic breakpoint support to advanced multi process debugging
* Code quality standards align with expectations from top tier IDE companies like Microsoft, JetBrains, and others

## Top Tier IDE Comparisons

### Visual Studio Code Debugging
* Debug Adapter Protocol (DAP) implementation
* Support for multiple debuggers (GDB, LLDB, etc.)
* Breakpoint management
* Variable inspection
* Call stack navigation
* Used by millions of developers

### IntelliJ IDEA Debugging
* Advanced debugger integration
* Advanced breakpoint types
* Advanced variable inspection
* Memory debugging
* Production tested at scale

### GDB/LLDB Command Line
* Industry standard debuggers
* Powerful debugging capabilities
* Scriptable debugging
* Reference implementations

## Debug Adapter Protocol (DAP)

### Protocol Overview
* **Purpose**: Language agnostic debugger interface
* **Communication**: JSON-RPC based communication
* **Standardization**: Standardizes debugger integration
* **Applications**: VSCode, IntelliJ IDEA, Eclipse
* **Reference**: Debug Adapter Protocol specification
* **Rationale**: DAP enables IDE debugger support

### DAP Features
* **Launch/attach**: Launch or attach to processes
* **Breakpoints**: Set and manage breakpoints
* **Execution control**: Step, continue, pause
* **Variable inspection**: Inspect variables and expressions
* **Call stack**: Navigate call stack
* **Rationale**: Comprehensive debugging features

## Breakpoint Management

### Breakpoint Types
* **Line breakpoints**: Break at specific line
* **Conditional breakpoints**: Break when condition is true
* **Logpoints**: Log message without stopping
* **Exception breakpoints**: Break on exceptions
* **Function breakpoints**: Break at function entry
* **Rationale**: Multiple breakpoint types enable flexible debugging

### Breakpoint Operations
* **Set/remove**: Set and remove breakpoints
* **Enable/disable**: Enable and disable breakpoints
* **Persistence**: Persist breakpoints across sessions
* **Validation**: Validate breakpoint locations
* **Complexity**: O(log n) for breakpoint lookup
* **Rationale**: Breakpoint management enables debugging

### Breakpoint State
* **Verified**: Breakpoint is valid and set
* **Unverified**: Breakpoint cannot be set (e.g., invalid location)
* **Conditions**: Conditional breakpoint conditions
* **Hit counts**: Track breakpoint hit counts
* **Rationale**: Breakpoint state enables user feedback

### Example Breakpoint Management
```cpp
// Thread safety: Thread safe (uses mutex)
// Ownership: Caller owns debugger session
// Complexity: O(log n) for breakpoint lookup
// Failure modes: Returns false on invalid location
bool set_breakpoint(DebuggerSession* session,
                    const char* file,
                    int line) {
    if (!session || !file || line < 1) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(session->mutex);
    
    Breakpoint bp;
    bp.file = file;
    bp.line = line;
    bp.enabled = true;
    
    return debugger_set_breakpoint(session->debugger, &bp);
}
```

## Execution Control

### Control Flow
* **Continue**: Continue execution until next breakpoint
* **Step over**: Execute current line, don't step into functions
* **Step into**: Step into function calls
* **Step out**: Step out of current function
* **Pause**: Pause execution
* **Restart**: Restart debugging session
* **Rationale**: Execution control enables code exploration

### Call Stack
* **Display**: Display call stack
* **Navigate**: Navigate between stack frames
* **Inspect**: Inspect variables in each frame
* **Jump**: Jump to stack frame
* **Complexity**: O(n) where n is stack depth
* **Rationale**: Call stack enables understanding execution flow

## Variable Inspection

### Variable Display
* **Values**: Display variable values
* **Scope hierarchy**: Display variables by scope
* **Watch expressions**: Evaluate and watch expressions
* **Evaluate**: Evaluate expressions in current context
* **Complexity**: O(1) for variable lookup
* **Rationale**: Variable inspection enables state understanding

### Data Visualization
* **Complex structures**: Display complex data structures
* **Tree views**: Expandable tree views for structures
* **Memory visualization**: Visualize memory layout
* **Custom visualizers**: Custom data visualizers
* **Rationale**: Visualization improves debugging experience

## Debugging Sessions

### Session Lifecycle
* **Launch**: Launch process for debugging
* **Attach**: Attach to running process
* **Start/stop**: Start and stop debugging sessions
* **Cleanup**: Cleanup resources on termination
* **Rationale**: Session lifecycle management enables debugging

### Multi Process Debugging
* **Multiple sessions**: Support multiple debug sessions
* **Process selection**: Select active process
* **Inter process**: Handle inter process communication
* **Synchronized**: Synchronized debugging across processes
* **Rationale**: Multi process debugging enables complex scenarios

## Implementation Standards

### Correctness
* **Protocol compliance**: Correct DAP implementation
* **Breakpoint accuracy**: Accurate breakpoint setting
* **State consistency**: Consistent debugger state
* **Rationale**: Correctness is critical for debugging

### Performance
* **Efficient communication**: Efficient debugger communication
* **Async operations**: Non blocking async operations
* **Lazy loading**: Lazy load variable data
* **Rationale**: Performance is critical for responsiveness

### Error Handling
* **Graceful degradation**: Handle errors gracefully
* **Timeout handling**: Handle timeouts appropriately
* **Resource cleanup**: Cleanup resources on errors
* **Rationale**: Robust error handling improves reliability

## Testing Requirements

### Unit Tests
* **Breakpoint management**: Test breakpoint operations
* **Execution control**: Test execution control
* **Variable inspection**: Test variable inspection
* **Edge cases**: Test edge cases
* **Rationale**: Comprehensive testing ensures correctness

### Integration Tests
* **Debugger integration**: Test with real debuggers
* **Multi process**: Test multi process debugging
* **Large programs**: Test with large programs
* **Rationale**: Integration tests verify system behavior

## Research Papers and References

### Debugging
* "Debug Adapter Protocol Specification" (Microsoft)
* "Debugging Techniques" - Research on debugging
* GDB/LLDB documentation

### Open Source References
* VSCode debugging implementation
* IntelliJ IDEA debugger integration
* GDB/LLDB debuggers

## Implementation Checklist

- [ ] Implement Debug Adapter Protocol
- [ ] Implement breakpoint management
- [ ] Implement execution control
- [ ] Implement variable inspection
- [ ] Implement call stack navigation
- [ ] Add error handling
- [ ] Write comprehensive unit tests
- [ ] Test with real debuggers
- [ ] Document protocol compliance

