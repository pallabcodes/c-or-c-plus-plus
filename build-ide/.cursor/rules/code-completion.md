# Code Completion Standards

## Overview
Code completion (IntelliSense) dramatically improves developer productivity by providing intelligent code suggestions. This document defines standards for implementing production grade code completion that matches the quality of top tier IDEs like VSCode and IntelliJ IDEA.

## Scope
* Applies to all code completion code including IntelliSense, autocomplete, and context aware suggestions
* Extends repository root rules defined in the root `.cursor/rules/` files
* Covers all aspects of code completion from basic keyword completion to advanced context aware suggestions
* Code quality standards align with expectations from top tier IDE companies like Microsoft, JetBrains, and others

## Top Tier IDE Comparisons

### Visual Studio Code IntelliSense
* Language Server Protocol based completion
* Context aware suggestions
* Fuzzy matching for typos
* Fast response times (< 100ms)
* Signature help and documentation
* Used by millions of developers

### IntelliJ IDEA Code Completion
* Advanced context analysis
* Type aware completion
* Smart completion with multiple variants
* Postfix completion
* Live templates
* Production tested at scale

### Sublime Text Autocomplete
* Simple keyword completion
* File name completion
* Snippet completion
* Fast and lightweight
* Configurable triggers

## Completion Strategies

### Keyword Completion
* **Language keywords**: Suggest language reserved words
* **Context sensitive**: Filter keywords based on context
* **Ranking**: Rank keywords by usage frequency
* **Fuzzy matching**: Handle typos and misspellings
* **Complexity**: O(1) for keyword lookup
* **Rationale**: Fast and simple completion

### Symbol Completion
* **Variables**: Complete variable names
* **Functions**: Complete function names
* **Type aware**: Filter by expected type
* **Scope aware**: Filter by current scope
* **Import completion**: Complete import statements
* **Complexity**: O(log n) for symbol lookup
* **Rationale**: Context aware completion

### Snippet Completion
* **Templates**: Code snippet templates
* **Placeholders**: Placeholder support for variables
* **Substitution**: Variable substitution in snippets
* **Tab stops**: Navigate between placeholders
* **Complexity**: O(1) for snippet insertion
* **Rationale**: Code templates improve productivity

## IntelliSense Engine

### Context Analysis
* **Parse context**: Parse current code context
* **Type inference**: Infer expected types
* **Scope analysis**: Analyze current scope
* **Filter candidates**: Filter completion candidates appropriately
* **Complexity**: O(n) where n is context size
* **Rationale**: Context analysis improves accuracy

### Ranking Algorithms
* **Frequency**: Rank by usage frequency
* **Recency**: Rank by recent usage
* **Type compatibility**: Rank by type compatibility
* **Edit distance**: Rank by edit distance (fuzzy matching)
* **Combined scoring**: Combine multiple factors
* **Complexity**: O(n log n) for ranking n candidates
* **Rationale**: Ranking improves relevance

### Fuzzy Matching
* **Edit distance**: Calculate edit distance (Levenshtein)
* **Fuzzy matching**: Match with typos
* **Threshold**: Configurable matching threshold
* **Performance**: O(m * n) where m, n are string lengths
* **Rationale**: Fuzzy matching handles typos

### Example Completion
```cpp
// Thread safety: Thread safe (pure function)
// Ownership: Caller owns context, returns completion items
// Complexity: O(n log n) where n is candidate count
// Failure modes: Returns empty list on NULL context
std::vector<CompletionItem> get_completions(
    const CompletionContext* context,
    const char* prefix,
    size_t prefix_length) {
    
    if (!context || !prefix) {
        return {};
    }
    
    std::vector<CompletionItem> candidates = 
        collect_candidates(context, prefix, prefix_length);
    
    rank_completions(&candidates, context);
    
    return candidates;
}
```

## Completion Features

### Signature Help
* **Function signatures**: Display function signatures
* **Parameter highlighting**: Highlight current parameter
* **Documentation**: Display function documentation
* **Overloads**: Support multiple overloads
* **Complexity**: O(1) for signature lookup
* **Rationale**: Signature help improves API discovery

### Completion Details
* **Hover information**: Additional info on hover
* **Documentation**: Documentation snippets
* **Type information**: Type information display
* **Lazy loading**: Load details on demand
* **Rationale**: Details improve understanding

### Trigger Characters
* **Auto trigger**: Trigger on dot, arrow, etc.
* **Manual trigger**: Trigger via keyboard shortcut
* **Configurable**: Configurable trigger characters
* **Language specific**: Language specific triggers
* **Rationale**: Triggers improve UX

## Implementation Standards

### Performance Requirements
* **Response time**: < 100ms target for completion response
* **Lazy loading**: Lazy load completion items
* **Efficient filtering**: Efficient candidate filtering
* **Memory efficiency**: Memory efficient storage
* **Rationale**: Performance is critical for responsiveness

### Thread Safety
* **Background analysis**: Thread safe background analysis
* **State synchronization**: Synchronize completion state
* **Cancellation**: Support cancellation of completion requests
* **Rationale**: Thread safety enables background processing

### Error Handling
* **Graceful degradation**: Handle errors gracefully
* **Partial results**: Return partial results on errors
* **Timeout handling**: Handle timeouts appropriately
* **Rationale**: Robust error handling improves reliability

## Testing Requirements

### Unit Tests
* **Completion logic**: Test completion logic
* **Ranking**: Test ranking algorithms
* **Fuzzy matching**: Test fuzzy matching
* **Edge cases**: Test edge cases
* **Rationale**: Comprehensive testing ensures correctness

### Performance Tests
* **Response time**: Benchmark response time
* **Scalability**: Test with large codebases
* **Memory usage**: Test memory usage
* **Rationale**: Performance tests ensure performance goals

## Research Papers and References

### Code Completion
* "Code Completion Ranking Algorithms" - Research on completion ranking
* "Context Aware Suggestion Systems" - Research on context analysis
* Language Server Protocol completion specification

### Open Source References
* VSCode IntelliSense implementation
* IntelliJ IDEA completion engine
* Sublime Text autocomplete

## Implementation Checklist

- [ ] Implement keyword completion
- [ ] Implement symbol completion
- [ ] Implement snippet completion
- [ ] Implement context analysis
- [ ] Implement ranking algorithms
- [ ] Implement fuzzy matching
- [ ] Implement signature help
- [ ] Add error handling
- [ ] Write comprehensive unit tests
- [ ] Benchmark performance
- [ ] Document time and space complexity

