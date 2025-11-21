//! AuroraDB Monitoring & Metrics: Revolutionary System Observability
//!
//! UNIQUENESS: Advanced monitoring fusing research-backed approaches:
//! - Multi-dimensional metrics collection with intelligent sampling
//! - ML-powered anomaly detection for system metrics
//! - Predictive monitoring with automated diagnostics
//! - Real-time dashboards with adaptive visualization
//! - Performance profiling with automated bottleneck detection
//! - Cost monitoring with resource usage optimization
//! - Automated alerting with contextual recommendations
//! - Historical analytics with trend forecasting

pub mod metrics;
pub mod alerting;
pub mod profiling;
pub mod diagnostics;
pub mod dashboards;
pub mod analytics;
pub mod cost_monitoring;
pub mod predictive;
pub mod exporters;

pub use metrics::*;
pub use alerting::*;
pub use profiling::*;
pub use diagnostics::*;
pub use dashboards::*;
pub use analytics::*;
pub use cost_monitoring::*;
pub use predictive::*;
pub use exporters::*;