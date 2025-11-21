//! AuroraDB Triggers: Intelligent Event-Driven Architecture
//!
//! Revolutionary trigger system that eliminates traditional database
//! trigger pain points through event-driven architecture, intelligent
//! filtering, and performance-optimized execution.

pub mod trigger_manager;
pub mod event_engine;
pub mod execution_engine;
pub mod condition_evaluator;
pub mod performance_monitor;
pub mod conflict_resolver;

pub use trigger_manager::*;
pub use event_engine::*;
pub use execution_engine::*;
pub use condition_evaluator::*;
pub use performance_monitor::*;
pub use conflict_resolver::*;
