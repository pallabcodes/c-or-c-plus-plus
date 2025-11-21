//! Query Planner Module
//!
//! AI-Native query planning with machine learning optimization:
//! - Cost-based optimization with ML cost models
//! - Adaptive planning based on workload patterns
//! - Learning from past query performance
//! - Vector-aware query optimization
//!
//! UNIQUENESS: Fuses System R optimizer + ML cost models + workload learning
//! Research: Volcano optimizer + reinforcement learning + Bayesian optimization

pub mod planner;
pub mod optimizer;
pub mod cost_model;
pub mod statistics;
pub mod learning;
pub mod logical;
pub mod core;
pub mod optimization;

// Re-export main planner components
pub use core::*;
pub use optimizer::{QueryOptimizer, OptimizationResult};
