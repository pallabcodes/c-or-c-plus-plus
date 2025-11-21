//! Comprehensive Testing Framework for AuroraDB
//!
//! UNIQUENESS: Research-backed testing combining property-based testing + chaos engineering + performance benchmarking
//! Research: Property-based testing (Claessen & Hughes), Chaos engineering (Netflix), Benchmarking (SPEC, TPC)

pub mod integration_tests;
pub mod performance_benchmarks;
pub mod property_tests;
pub mod chaos_tests;
pub mod mock_implementations;
pub mod test_utils;

// Re-export main testing components
pub use integration_tests::*;
pub use performance_benchmarks::*;
pub use property_tests::*;
pub use chaos_tests::*;
pub use mock_implementations::*;
pub use test_utils::*;
