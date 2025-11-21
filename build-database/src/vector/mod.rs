//! AuroraDB Vector Search: Revolutionary Similarity Search
//!
//! UNIQUENESS: Advanced vector search fusing research-backed approaches:
//! - HNSW (Hierarchical Navigable Small World) for billion-scale search
//! - IVF (Inverted File Index) with PQ (Product Quantization) for efficiency
//! - Multiple distance metrics optimized for different use cases
//! - Hardware-accelerated similarity computation with SIMD
//! - Adaptive indexing with dynamic parameter tuning
//!
//! Advanced Features with UNIQUENESS:
//! - Real-time index updates without full rebuilds (fuses streaming + static indexing)
//! - Metadata filtering for pre-search candidate selection (fuses vector + relational filtering)
//! - Hybrid search combining vector + keyword + metadata (fuses multiple modalities)
//! - Distributed search across multiple nodes (fuses distributed systems + vector search)
//! - GPU acceleration support (fuses hardware acceleration + similarity search)

pub mod vector_index;
pub mod distance_metrics;
pub mod hnsw_index;
pub mod ivf_index;
pub mod pq_quantization;
pub mod vector_operations;
pub mod vector_storage;
pub mod vector_query;

// Advanced vector search features
pub mod advanced {
    pub mod realtime_updates;
    pub mod filtering;
    pub mod hybrid_search;
    pub mod distributed_search;
}

// Performance and memory optimizations
pub mod optimization {
    pub mod memory_optimization;
    pub mod performance_optimization;
}

pub use vector_index::*;
pub use distance_metrics::*;
pub use hnsw_index::*;
pub use ivf_index::*;
pub use pq_quantization::*;
pub use vector_operations::*;
pub use vector_storage::*;
pub use vector_query::*;

// Advanced feature re-exports
pub use advanced::realtime_updates::*;
pub use advanced::filtering::*;
pub use advanced::hybrid_search::*;
pub use advanced::distributed_search::*;

// Optimization re-exports
pub use optimization::memory_optimization::*;
pub use optimization::performance_optimization::*;
