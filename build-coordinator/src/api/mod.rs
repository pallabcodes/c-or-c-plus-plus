//! API & SDK Layer: UNIQUENESS Developer Experience
//!
//! Research-backed API design for distributed coordination:
//! - **REST API**: OpenAPI 3.0 compliant HTTP endpoints
//! - **gRPC API**: High-performance protobuf-based RPC
//! - **GraphQL API**: Flexible query interface for complex operations
//! - **WebSocket API**: Real-time event streaming
//! - **SDKs**: Client libraries in Go, Python, Java, Rust

pub mod rest_api;
pub mod grpc_api;
pub mod graphql_api;
pub mod websocket_api;
pub mod sdk_generator;

pub use rest_api::RestAPI;
pub use grpc_api::GrpcAPI;
pub use graphql_api::GraphQLAPI;
pub use websocket_api::WebSocketAPI;
pub use sdk_generator::SDKGenerator;

// UNIQUENESS Research Citations:
// - **REST API Design**: Fielding (2000) - REST architectural style
// - **gRPC**: Google - High-performance RPC framework
// - **GraphQL**: Facebook (2015) - Query language for APIs
// - **OpenAPI**: Linux Foundation - API specification standard
