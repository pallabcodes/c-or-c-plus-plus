//! JIT Compilation and Performance Optimization
//!
//! Advanced query compilation and runtime optimization:
//! - LLVM-based JIT compilation for query execution plans
//! - SIMD vectorization for analytical workloads
//! - Runtime code generation and optimization
//! - Performance profiling and adaptive optimization
//!
//! UNIQUENESS: Fuses LLVM JIT + Cranelift + SIMD research + adaptive compilation
//! Research: Query compilation (Hyper, Umbra) + SIMD databases (ClickHouse, DuckDB)

pub mod compiler;
pub mod optimizer;
pub mod vectorizer;
pub mod profiler;
pub mod cache;

// Re-export main JIT components
pub use compiler::{JITCompiler, CompilationResult, CompiledQuery};
pub use optimizer::{QueryOptimizer, OptimizationLevel, OptimizationResult};
pub use vectorizer::{SIMDVectorizer, VectorizationResult};
pub use profiler::{PerformanceProfiler, ProfileData, OptimizationHints};
pub use cache::{JITCache, CacheEntry, CacheStatistics};
