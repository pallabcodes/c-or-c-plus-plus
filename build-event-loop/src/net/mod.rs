//! Cyclone Networking: Bleeding-Edge Research Implementation
//!
//! UNIQUENESS: Combines bleeding-edge research for 2M+ RPS networking:
//! - RDMA (RoCE/iWARP): Kernel-bypass networking (InfiniBand research)
//! - DPDK: User-space networking (Intel DPDK framework)
//! - XDP/eBPF: Kernel-level packet processing (Linux kernel research)
//! - SIMD acceleration: Vectorized processing (Intel/ARM research)
//! - Zero-copy optimization: Memory bypass techniques (Druschel & Banga, 1996)

pub mod rdma;
pub mod dpdk;
pub mod xdp;
pub mod simd_acceleration;
pub mod connection_pooling;
pub mod syscall_batching;
pub mod zero_copy_optimization;
pub mod network_optimization;
pub mod high_performance_stack;

pub use rdma::*;
pub use dpdk::*;
pub use xdp::*;
pub use simd_acceleration::*;
pub use connection_pooling::*;
pub use syscall_batching::*;
pub use zero_copy_optimization::*;
pub use network_optimization::*;
pub use high_performance_stack::*;
