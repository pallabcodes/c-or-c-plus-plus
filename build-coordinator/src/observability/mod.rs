//! Advanced Observability: UNIQUENESS End-to-End Tracing
//!
//! Research-backed distributed tracing and observability:
//! - **OpenTelemetry**: Industry-standard distributed tracing
//! - **Correlation IDs**: Request tracking across services
//! - **Service Mesh Integration**: Istio, Linkerd compatibility
//! - **Log Correlation**: Structured logging with tracing context
//! - **Performance Profiling**: Continuous profiling and flame graphs

pub mod distributed_tracing;
pub mod correlation_tracking;
pub mod service_mesh;
pub mod performance_profiling;
pub mod log_correlation;
pub mod metrics_aggregation;

pub use distributed_tracing::DistributedTracer;
pub use correlation_tracking::CorrelationTracker;
pub use service_mesh::ServiceMeshIntegration;
pub use performance_profiling::PerformanceProfiler;
pub use log_correlation::LogCorrelator;
pub use metrics_aggregation::MetricsAggregator;

// UNIQUENESS Research Citations:
// - **Distributed Tracing**: OpenTelemetry, Dapper (Google, 2010)
// - **Correlation Tracking**: Request tracing research
// - **Service Mesh**: Istio, Linkerd research papers
