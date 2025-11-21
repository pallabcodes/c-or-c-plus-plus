//! NUMA-Aware Task Scheduler for Cyclone
//!
//! UNIQUENESS: Research-backed scheduler combining:
//! - NUMA-aware work distribution (Torrellas et al., 2010)
//! - Cache-aware scheduling (Drepper, 2007)
//! - Work-stealing algorithms (Blumofe & Leiserson, 1999)
//! - Adaptive load balancing (Boyd-Wickizer et al., 2008)

pub mod numa_aware_scheduler;
pub mod work_stealing;
pub mod cache_aware;
pub mod adaptive_load_balancer;

pub use numa_aware_scheduler::*;
pub use work_stealing::*;
pub use cache_aware::*;
pub use adaptive_load_balancer::*;
