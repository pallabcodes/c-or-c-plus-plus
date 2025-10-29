# IDE Module Overview

## Context
This code is written by an SDE 2 backend and low level system engineer working with top tier product companies including Google, Atlassian, Bloomberg, PayPal, Stripe, Uber, Amazon, and other top tier silicon valley companies. This IDE implementation must meet enterprise production standards suitable for principal level engineering review and must be comparable to top tier IDE products like Visual Studio Code, IntelliJ IDEA, Vim, Emacs, Sublime Text, and other professional development environments.

## Purpose
This module covers the design and implementation of a production grade integrated development environment in C and C plus plus. All code must follow production grade standards suitable for principal level code review and must be production ready for deployment as a professional IDE supporting multiple languages, with responsive editing, intelligent code assistance, and comprehensive developer tooling.

## Scope
* Applies to all C and C plus plus code in build ide directory
* Extends repository root rules defined in the root `.cursorrules` file
* Covers all aspects of IDE systems from text editing to language servers, debugging, version control, and extensibility
* Code quality standards align with expectations from top tier IDE companies like Microsoft (VSCode), JetBrains (IntelliJ), and open source leaders

## Top Tier IDE Product Comparisons

### Visual Studio Code (Microsoft)
Reference: https://code.visualstudio.com/
* Electron based architecture with TypeScript
* Language Server Protocol implementation
* Extension marketplace with thousands of extensions
* Integrated terminal and debugger
* Debug Adapter Protocol (DAP)
* Multi root workspaces
* IntelliSense engine with semantic highlighting
* Git integration with diff visualization
* Remote development support
* WebAssembly support for browser execution

### IntelliJ IDEA (JetBrains)
Reference: https://www.jetbrains.com/idea/
* JVM based architecture
* Comprehensive plugin system
* Code inspection engine with static analysis
* Advanced refactoring support
* Build system integration (Maven, Gradle, etc.)
* Version control integration (Git, SVN, etc.)
* Database tools and SQLife support
* Framework specific tooling
* Code generation and templates

### Vim/Neovim
* Modal editing paradigm with modes (normal, insert, visual)
* Extensible via Vimscript and Lua
* Large plugin ecosystem (Neovim plugins)
* Terminal integration
* Language server support via plugins (nvim-lspconfig)
* Lightweight and fast
* Highly customizable
* Batch editing capabilities

### Emacs
* Lisp based extensibility (Emacs Lisp)
* Org mode for literate programming
* Magit for Git interface
* Tramp for remote file access
* Customizable keybindings and macros
* Integrated environment (email, calendar, etc.)
* Unique editing philosophy

### Sublime Text
* C++ core for high performance
* Python plugin API
* Multiple selections and editing
* Command palette for quick access
* Goto anything for navigation
* Lightweight and responsive
* Customizable UI themes

## IDE Architecture Components

### Core Components
1. Text Editor Core: Text buffers, editing operations, undo/redo mechanisms
2. Language Server Protocol: LSP implementation for language support
3. Syntax Highlighting: Lexical analysis, tokenization, highlighting engines
4. Code Completion: IntelliSense, autocomplete, context aware suggestions
5. Code Navigation: Symbol resolution, go to definition, find references
6. Refactoring: Code transformations, rename, extract, move operations
7. Debugging Support: Debugger integration, breakpoints, variable inspection
8. Build System Integration: Compiler integration, build tools, task runners
9. Source Control: Git integration, version control, diff visualization
10. Extensibility: Plugin architecture, APIs, extension marketplace
11. Performance Optimization: Virtual scrolling, lazy loading, incremental parsing
12. UI Rendering: Text rendering, UI frameworks, graphics acceleration

## Code Quality Standards
All IDE code must demonstrate:
* Comprehensive error handling with clear messages
* Proper resource management with deterministic cleanup
* Correct synchronization for concurrent operations
* Memory safety through bounds checking and proper alignment
* Responsive UI with minimal latency
* Testing of both success and failure scenarios
* Performance optimization for smooth editing experience
* Research backed implementations with proper citations from academic papers and open source projects

## Reference Material
* See existing examples in system programming directories for low level patterns
* Reference research papers cited in individual rule files
* Study open source implementations of VSCode, IntelliJ, Vim, Emacs, Sublime Text
* Language Server Protocol specification and implementations
* Tree sitter for incremental parsing
* Debug Adapter Protocol for debugging support

## Related Rules
Refer to the other rule files in this directory for specific guidance on text editing, language servers, syntax highlighting, code completion, navigation, refactoring, debugging, build systems, version control, extensibility, performance, UI rendering, and testing.
