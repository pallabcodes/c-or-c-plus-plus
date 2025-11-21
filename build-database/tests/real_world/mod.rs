//! Real-World Testing Scenarios for AuroraDB
//!
//! This module contains comprehensive tests that simulate production workloads,
//! demonstrating UNIQUENESS through validated performance at scale.

pub mod data_generator;
pub mod workload_simulator;
pub mod stress_tester;
pub mod scenario_runner;

pub use data_generator::*;
pub use workload_simulator::*;
pub use stress_tester::*;
pub use scenario_runner::*;
