//! AuroraDB Views: Intelligent Virtual Tables
//!
//! Revolutionary view implementation that goes beyond traditional database views
//! with intelligent caching, automatic refresh, and performance optimization.

pub mod view_manager;
pub mod view_cache;
pub mod view_optimizer;
pub mod materialized_views;

pub use view_manager::*;
pub use view_cache::*;
pub use view_optimizer::*;
pub use materialized_views::*;
