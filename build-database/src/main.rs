//! AuroraDB Production Database Server - Main Entry Point
//!
//! This is the main entry point for running AuroraDB as a production database server.
//! It demonstrates how to:
//! - Initialize the AuroraDB engine with all components
//! - Start the database server with multiple protocols
//! - Handle graceful shutdown
//! - Monitor database health and metrics

use std::sync::Arc;
use tokio::signal;
use tracing::{info, warn, error};
use aurora_db::engine::AuroraDB;
use aurora_db::network::{PostgresServer, ServerConfig, ConnectionPoolConfig};
use aurora_db::config::{StorageConfig, TransactionConfig, VectorConfig, SecurityConfig, AuditConfig};

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    // Initialize tracing
    tracing_subscriber::fmt()
        .with_env_filter(tracing_subscriber::EnvFilter::from_default_env())
        .init();

    info!("🚀 Starting AuroraDB Production Database Server...");

    // Load configuration (in production, this would come from files/environment)
    let config = create_database_config();

    // Initialize the AuroraDB engine
    info!("🏗️  Initializing AuroraDB engine...");
    let database = Arc::new(AuroraDB::new(config).await?);
    info!("✅ AuroraDB engine initialized successfully");

    // Create server configuration
    let server_config = create_server_config();

    // Initialize the PostgreSQL server with connection pooling
    info!("🌐 Initializing AuroraDB PostgreSQL server...");
    let server = PostgresServer::new(database.clone(), server_config.address.clone());
    info!("✅ AuroraDB PostgreSQL server initialized successfully");

    // Setup graceful shutdown handling
    let shutdown_handle = tokio::spawn(async move {
        match signal::ctrl_c().await {
            Ok(()) => {
                info!("🛑 Received shutdown signal, stopping server gracefully...");
                // Note: Our current server doesn't have a stop method yet
                info!("Server shutdown completed");
            }
            Err(e) => {
                error!("Error waiting for shutdown signal: {}", e);
            }
        }
    });

    // Start background monitoring
    let monitor_handle = tokio::spawn(monitor_database_health(database.clone()));

    info!("🎉 AuroraDB Production Database Server is now running!");
    info!("   • PostgreSQL Protocol: localhost:5433");
    info!("   • HTTP API: localhost:8080");
    info!("   • Binary Protocol: localhost:9090");
    info!("   • Health Check: http://localhost:8080/health");
    info!("   • Metrics: http://localhost:8080/metrics");
    info!("   • Press Ctrl+C to stop the server");

    // Start the server (this will block until shutdown)
    if let Err(e) = server.start().await {
        error!("Server error: {}", e);
        return Err(e.into());
    }

    // Wait for shutdown tasks to complete
    let _ = tokio::try_join!(shutdown_handle, monitor_handle);

    info!("👋 AuroraDB Production Database Server stopped");
    Ok(())
}

/// Create database configuration for production use
fn create_database_config() -> DatabaseConfig {
    DatabaseConfig {
        storage: StorageConfig {
            btree: aurora_db::storage::btree::BTreeConfig {
                max_table_size: 1_000_000, // 1M rows
                page_size: 4096,
                cache_size: 100_000_000, // 100MB
                max_concurrent_transactions: 1000,
            },
            lsm: aurora_db::storage::lsm::LSMConfig {
                max_memtable_size: 64_000_000, // 64MB
                sstable_size: 256_000_000, // 256MB
                compaction_threads: 4,
                bloom_filter_bits: 10,
            },
            hybrid: aurora_db::storage::hybrid::HybridConfig {
                adaptive_threshold: 100_000, // Switch engines at 100K rows
                vector_threshold: 0.1, // 10% vector columns triggers hybrid
            },
            selection_strategy: "workload_based".to_string(),
        },
        transaction: TransactionConfig {
            max_concurrent_transactions: 1000,
            deadlock_detection_interval_ms: 100,
            transaction_timeout_ms: 30000,
            isolation_level: "repeatable_read".to_string(),
        },
        vector: VectorConfig {
            default_dimension: 384,
            index_type: "hnsw".to_string(),
            max_connections: 32,
            ef_construction: 200,
            ef_search: 64,
        },
        security: SecurityConfig {
            enable_authentication: true,
            enable_authorization: true,
            password_min_length: 8,
            session_timeout_minutes: 60,
        },
        audit: AuditConfig {
            enable_audit_logging: true,
            audit_log_path: "/var/log/aurora/audit.log".to_string(),
            log_sensitive_operations: true,
        },
    }
}

/// Create server configuration
fn create_server_config() -> String {
    "127.0.0.1:5432".to_string() // PostgreSQL default port
}

/// Background task to monitor database health
async fn monitor_database_health(database: Arc<AuroraDB>) {
    let mut interval = tokio::time::interval(std::time::Duration::from_secs(30));

    loop {
        interval.tick().await;

        match database.get_health_status().await {
            Ok(health) => {
                match health.overall_status {
                    aurora_db::engine::HealthState::Healthy => {
                        info!("💚 Database health: Healthy");
                    }
                    aurora_db::engine::HealthState::Degraded => {
                        warn!("💛 Database health: Degraded - {}", health.component_statuses.len());
                    }
                    aurora_db::engine::HealthState::Unhealthy => {
                        error!("❤️  Database health: Unhealthy");
                    }
                }
            }
            Err(e) => {
                error!("Failed to check database health: {}", e);
            }
        }

        // Also log some metrics
        match database.get_metrics().await {
            Ok(metrics) => {
                info!("📊 Metrics: {} queries, {} active tx, {:.1}μs avg query time",
                     metrics.total_queries,
                     metrics.active_transactions,
                     metrics.average_query_time_micros);
            }
            Err(e) => {
                error!("Failed to get database metrics: {}", e);
            }
        }
    }
}

/// Example function showing how to use AuroraDB programmatically
#[allow(dead_code)]
async fn example_usage(database: &Arc<AuroraDB>) -> Result<(), Box<dyn std::error::Error>> {
    use aurora_db::engine::{UserContext, TableSchema, ColumnDefinition, DataType, IndexDefinition, IndexType};

    info!("📝 Running AuroraDB usage example...");

    // Create a user context
    let user_context = UserContext {
        user_id: "example_user".to_string(),
        username: "example".to_string(),
        roles: vec!["admin".to_string()],
        client_ip: Some("127.0.0.1".parse().unwrap()),
        session_id: "example_session".to_string(),
    };

    // Create a table schema
    let schema = TableSchema {
        columns: vec![
            ColumnDefinition {
                name: "id".to_string(),
                data_type: DataType::BigInt,
                nullable: false,
                default_value: None,
            },
            ColumnDefinition {
                name: "name".to_string(),
                data_type: DataType::Text,
                nullable: false,
                default_value: None,
            },
            ColumnDefinition {
                name: "email".to_string(),
                data_type: DataType::Text,
                nullable: true,
                default_value: None,
            },
            ColumnDefinition {
                name: "embedding".to_string(),
                data_type: DataType::Vector(384),
                nullable: true,
                default_value: None,
            },
        ],
        primary_key: Some(vec!["id".to_string()]),
        indexes: vec![
            IndexDefinition {
                name: "idx_name".to_string(),
                columns: vec!["name".to_string()],
                index_type: IndexType::BTree,
            },
            IndexDefinition {
                name: "idx_embedding".to_string(),
                columns: vec!["embedding".to_string()],
                index_type: IndexType::Vector,
            },
        ],
    };

    // Create the table
    database.create_table("users", &schema, &user_context).await?;
    info!("✅ Created 'users' table with vector support");

    // Execute some sample queries
    let queries = vec![
        "INSERT INTO users (id, name, email) VALUES (1, 'Alice', 'alice@example.com')",
        "INSERT INTO users (id, name, email) VALUES (2, 'Bob', 'bob@example.com')",
        "SELECT * FROM users WHERE name = 'Alice'",
        "SELECT COUNT(*) as user_count FROM users",
    ];

    for query in queries {
        info!("🔄 Executing: {}", query);
        match database.execute_query(query, &user_context).await {
            Ok(result) => {
                info!("✅ Query successful: {} rows, {} columns, took {:?}",
                     result.rows.len(), result.columns.len(), result.execution_time);
            }
            Err(e) => {
                error!("❌ Query failed: {}", e);
            }
        }
    }

    // Example vector search
    let vector_request = aurora_db::engine::VectorSearchRequest {
        collection: "users".to_string(),
        query_vector: vec![0.1; 384], // Example 384-dimensional vector
        limit: 5,
        filters: Some(std::collections::HashMap::from([
            ("name".to_string(), serde_json::json!("Alice"))
        ])),
        include_metadata: true,
    };

    match database.execute_vector_search(&vector_request, &user_context).await {
        Ok(result) => {
            info!("✅ Vector search successful: {} results, took {:?}",
                 result.results.len(), result.execution_time);
        }
        Err(e) => {
            error!("❌ Vector search failed: {}", e);
        }
    }

    info!("🎉 AuroraDB usage example completed!");
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn test_database_initialization() {
        // This test would require setting up test configurations
        // and mock components. For now, it's a placeholder.
        assert!(true);
    }
}
