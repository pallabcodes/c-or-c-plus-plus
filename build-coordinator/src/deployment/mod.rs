//! Deployment & Orchestration: UNIQUENESS Production Deployment
//!
//! Research-backed deployment automation for distributed coordination:
//! - **Kubernetes Operator**: Custom resource definitions and controllers
//! - **Helm Charts**: Package management for Kubernetes deployments
//! - **Docker Images**: Multi-stage optimized container images
//! - **Service Mesh**: Istio integration for traffic management
//! - **Rolling Updates**: Zero-downtime deployment strategies
//! - **Multi-Region Deployment**: Cross-region coordination setup

pub mod k8s_operator;
pub mod helm_charts;
pub mod docker_images;
pub mod service_mesh;
pub mod rolling_updates;
pub mod multi_region;

pub use k8s_operator::KubernetesOperator;
pub use helm_charts::HelmChartManager;
pub use docker_images::DockerImageBuilder;
pub use service_mesh::ServiceMeshIntegration;
pub use rolling_updates::RollingUpdateManager;
pub use multi_region::MultiRegionManager;

// UNIQUENESS Research Citations:
// - **Kubernetes Operators**: Red Hat Operator Framework
// - **Service Mesh**: Istio, Linkerd research papers
// - **Rolling Deployments**: Netflix deployment strategies
// - **Multi-Region Architecture**: AWS, Google multi-region research
