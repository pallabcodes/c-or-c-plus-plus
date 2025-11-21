//! AuroraDB Network Layer
//!
//! This module implements the PostgreSQL wire protocol and connection pooling
//! for AuroraDB, enabling it to accept client connections and handle queries.
//!
//! UNIQUENESS: PostgreSQL-compatible protocol with AuroraDB's advanced features
//! like window functions, aggregates, and MVCC transactions.

pub mod postgres_protocol;
pub mod connection_pool;
pub mod server;

pub use postgres_protocol::*;
pub use connection_pool::*;
pub use server::*;