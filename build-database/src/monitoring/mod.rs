//! AuroraDB Enterprise Monitoring System
//!
//! Production-grade monitoring with Prometheus metrics and Grafana dashboards:
//! - Real-time metrics collection
//! - Prometheus exposition format
//! - Grafana dashboard templates
//! - Alerting rules and thresholds
//! - Performance monitoring and anomaly detection

pub mod prometheus_metrics;
pub mod grafana_dashboards;
pub mod alerting;
pub mod health_checks;
pub mod performance_monitor;

pub use prometheus_metrics::*;
pub use grafana_dashboards::*;
pub use alerting::*;
pub use health_checks::*;
pub use performance_monitor::*;