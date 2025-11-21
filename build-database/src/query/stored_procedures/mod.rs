//! AuroraDB Stored Procedures: JIT-Compiled Procedural Code
//!
//! Revolutionary stored procedure system that eliminates traditional database
//! stored procedure pain points through JIT compilation, multi-language support,
//! and intelligent security controls.

pub mod procedure_manager;
pub mod jit_compiler;
pub mod security_engine;
pub mod runtime_environment;
pub mod version_control;
pub mod performance_monitor;

pub use procedure_manager::*;
pub use jit_compiler::*;
pub use security_engine::*;
pub use runtime_environment::*;
pub use version_control::*;
pub use performance_monitor::*;
