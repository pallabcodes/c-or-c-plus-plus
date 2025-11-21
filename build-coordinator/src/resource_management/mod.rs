//! Resource Management: UNIQUENESS Low-Level Optimization
//!
//! Research-backed resource management for optimal performance:
//! - **CPU Pinning**: Thread-to-core affinity for cache optimization
//! - **NUMA Awareness**: Memory allocation on correct NUMA nodes
//! - **Huge Pages**: Reduced TLB misses with large page sizes
//! - **I/O Scheduling**: Optimized disk and network I/O priorities
//! - **Memory Locking**: Prevent page swapping for critical threads
//! - **Resource Limits**: cgroup integration for resource isolation

pub mod cpu_pinning;
pub mod numa_awareness;
pub mod huge_pages;
pub mod io_scheduling;
pub mod memory_locking;
pub mod cgroup_integration;

pub use cpu_pinning::CPUPinner;
pub use numa_awareness::NUMAOptimizer;
pub use huge_pages::HugePageManager;
pub use io_scheduling::IOScheduler;
pub use memory_locking::MemoryLocker;
pub use cgroup_integration::CGroupManager;

// UNIQUENESS Research Citations:
// - **NUMA Optimization**: Torrellas et al. (2010) - Cache-coherent NUMA
// - **CPU Pinning**: Linux sched_setaffinity research
// - **Huge Pages**: Linux kernel huge page implementation
// - **Control Groups**: Linux cgroups for resource management
