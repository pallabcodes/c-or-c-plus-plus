//! Enhanced AuroraDB Networking with UNIQUENESS
//!
//! Fuses AuroraDB's multi-protocol support with Cyclone's optimizations:
//! - SIMD-accelerated protocol processing
//! - Connection pooling with health monitoring
//! - Syscall batching for kernel efficiency
//! - Zero-copy buffer management
//! - Research-backed performance optimizations

pub mod simd_protocol_processor;
pub mod enhanced_connection_pool;
pub mod syscall_batch_processor;
pub mod zero_copy_message_handler;
pub mod unified_network_optimizer;

pub use simd_protocol_processor::*;
pub use enhanced_connection_pool::*;
pub use syscall_batch_processor::*;
pub use zero_copy_message_handler::*;
pub use unified_network_optimizer::*;
