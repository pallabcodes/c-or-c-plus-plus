# AuroraDB API Reference

Complete API documentation for AuroraDB's UNIQUENESS-powered database engine.

## 🏗️ Architecture Overview

AuroraDB provides multiple APIs for different use cases:

- **SQL API**: Standard SQL interface with PostgreSQL compatibility
- **HTTP API**: RESTful interface for web applications
- **gRPC API**: High-performance RPC interface
- **Rust API**: Native Rust library interface

## 📊 SQL API

### Connection

```rust
use aurora_db::{AuroraDB, ConnectionConfig};

let config = ConnectionConfig {
    host: "localhost".to_string(),
    port: 5432,
    user: "aurora".to_string(),
    password: Some("password".to_string()),
    database: "aurora".to_string(),
};

let db = AuroraDB::new(config).await?;
```

### Basic Operations

#### Execute Query

```rust
let result = db.execute_query("SELECT * FROM users WHERE age > 21").await?;
println!("Found {} users", result.row_count);
```

#### Prepared Statements

```rust
let stmt = db.prepare("SELECT * FROM users WHERE age > ?").await?;
let result = stmt.execute(&[&21]).await?;
```

#### Transactions

```rust
let mut txn = db.begin_transaction().await?;

txn.execute("INSERT INTO users (name, email) VALUES (?, ?)", &["Alice", "alice@example.com"]).await?;
txn.execute("UPDATE accounts SET balance = balance - 100 WHERE user_id = ?", &[&user_id]).await?;

txn.commit().await?;
```

### Advanced Features

#### Vector Search

```rust
// Create table with vector column
db.execute_query(r#"
    CREATE TABLE products (
        id INTEGER PRIMARY KEY,
        name VARCHAR(255),
        embedding VECTOR(384)
    )
"#).await?;

// Insert vectors
let embedding = vec![0.1f32, 0.2f32, /* ... 384 dimensions */];
db.execute_query(
    "INSERT INTO products (id, name, embedding) VALUES (?, ?, ?)",
    &[&1, &"Product A", &embedding]
).await?;

// Search similar products
let results = db.vector_search(&embedding, 10, "products", "embedding").await?;
```

#### JIT Compilation Control

```rust
// Enable JIT for specific query
db.execute_query("SET jit_enabled = true").await?;

// Check JIT status
let jit_status = db.get_jit_status().await?;
println!("JIT compilations: {}", jit_status.compilations_total);

// Clear JIT cache
db.clear_jit_cache().await?;
```

#### Analytics with SIMD

```rust
// Analytical query automatically uses SIMD vectorization
let result = db.execute_query(r#"
    SELECT
        category,
        SUM(amount) as total,
        AVG(amount) as average,
        COUNT(*) as count
    FROM transactions
    WHERE created_at >= '2024-01-01'
    GROUP BY category
    ORDER BY total DESC
"#).await?;
```

## 🌐 HTTP API

### Authentication

```bash
# Login
curl -X POST http://localhost:8080/auth/login \
  -H "Content-Type: application/json" \
  -d '{"username": "aurora", "password": "password", "database": "aurora"}'

# Response: {"token": "jwt_token_here"}
```

### Query Execution

```bash
# Execute SQL query
curl -X POST http://localhost:8080/api/query \
  -H "Authorization: Bearer jwt_token" \
  -H "Content-Type: application/json" \
  -d '{"query": "SELECT * FROM users LIMIT 10"}'

# Response:
{
  "columns": ["id", "name", "email"],
  "data": [
    [1, "Alice", "alice@example.com"],
    [2, "Bob", "bob@example.com"]
  ],
  "row_count": 2,
  "execution_time_ms": 1.23
}
```

### Health Checks

```bash
# Health status
curl http://localhost:8080/health

# Detailed status
curl http://localhost:8080/api/status
```

### Metrics

```bash
# Prometheus metrics
curl http://localhost:9090/metrics

# JSON metrics
curl http://localhost:8080/api/metrics
```

## 🚀 gRPC API

### Service Definition

```protobuf
service AuroraDB {
  rpc ExecuteQuery(QueryRequest) returns (QueryResponse);
  rpc ExecuteBatch(BatchRequest) returns (BatchResponse);
  rpc BeginTransaction(TransactionRequest) returns (TransactionResponse);
  rpc CommitTransaction(TransactionRequest) returns (TransactionResponse);
  rpc RollbackTransaction(TransactionRequest) returns (TransactionResponse);
  rpc VectorSearch(VectorSearchRequest) returns (VectorSearchResponse);
}

message QueryRequest {
  string query = 1;
  repeated Value parameters = 2;
  map<string, string> options = 3;
}

message QueryResponse {
  repeated string columns = 1;
  repeated Row data = 2;
  int32 row_count = 3;
  double execution_time_ms = 4;
  map<string, string> metadata = 5;
}
```

### Usage Example

```rust
use aurora_grpc::aurora_db_client::AuroraDbClient;
use aurora_grpc::{QueryRequest, QueryResponse};

let mut client = AuroraDbClient::connect("http://localhost:50051").await?;

let request = tonic::Request::new(QueryRequest {
    query: "SELECT * FROM users".to_string(),
    parameters: vec![],
    options: Default::default(),
});

let response = client.execute_query(request).await?;
let result: QueryResponse = response.into_inner();
```

## 🦀 Rust API

### Core Types

```rust
pub struct AuroraDB {
    // Main database instance
}

pub struct ConnectionConfig {
    pub host: String,
    pub port: u16,
    pub user: String,
    pub password: Option<String>,
    pub database: String,
    pub ssl_mode: SslMode,
    pub connection_timeout_ms: u64,
}

pub struct QueryResult {
    pub data: Vec<String>,
    pub row_count: usize,
    pub execution_time_ms: f64,
    pub columns: Vec<String>,
}

pub enum IsolationLevel {
    ReadUncommitted,
    ReadCommitted,
    RepeatableRead,
    Serializable,
}
```

### Connection Methods

```rust
impl AuroraDB {
    // Create new database instance
    pub async fn new(config: ConnectionConfig) -> Result<Self, AuroraError>;

    // Execute SQL query
    pub async fn execute_query(&self, sql: &str) -> Result<QueryResult, AuroraError>;

    // Execute prepared statement
    pub async fn execute_prepared(&self, stmt: &PreparedStatement, params: &[&dyn ToSql]) -> Result<QueryResult, AuroraError>;

    // Transaction management
    pub async fn begin_transaction(&self) -> Result<Transaction, AuroraError>;

    // Vector operations
    pub async fn vector_search(&self, query_vector: &[f32], k: usize, table: &str, column: &str) -> Result<VectorResult, AuroraError>;

    // JIT control
    pub async fn get_jit_status(&self) -> Result<JITStatus, AuroraError>;
    pub async fn clear_jit_cache(&self) -> Result<(), AuroraError>;

    // Administrative
    pub async fn create_backup(&self, path: &str) -> Result<(), AuroraError>;
    pub async fn restore_backup(&self, path: &str) -> Result<(), AuroraError>;
    pub async fn get_metrics(&self) -> Result<HashMap<String, serde_json::Value>, AuroraError>;
}
```

### Transaction API

```rust
pub struct Transaction<'a> {
    db: &'a AuroraDB,
    id: TransactionId,
}

impl<'a> Transaction<'a> {
    // Execute within transaction
    pub async fn execute(&mut self, sql: &str, params: &[&dyn ToSql]) -> Result<QueryResult, AuroraError>;

    // Commit transaction
    pub async fn commit(self) -> Result<(), AuroraError>;

    // Rollback transaction
    pub async fn rollback(self) -> Result<(), AuroraError>;

    // Set isolation level
    pub async fn set_isolation_level(&mut self, level: IsolationLevel) -> Result<(), AuroraError>;

    // Create savepoint
    pub async fn savepoint(&mut self, name: &str) -> Result<(), AuroraError>;

    // Rollback to savepoint
    pub async fn rollback_to_savepoint(&mut self, name: &str) -> Result<(), AuroraError>;
}
```

### Vector Operations

```rust
pub struct VectorResult {
    pub vectors: Vec<Vec<f32>>,
    pub distances: Vec<f32>,
    pub execution_time_ms: f64,
    pub metadata: HashMap<String, String>,
}

impl AuroraDB {
    // Basic vector search
    pub async fn vector_search(
        &self,
        query_vector: &[f32],
        k: usize,
        table: &str,
        column: &str
    ) -> Result<VectorResult, AuroraError>;

    // Advanced vector search with filters
    pub async fn vector_search_filtered(
        &self,
        query_vector: &[f32],
        k: usize,
        table: &str,
        column: &str,
        filter: &str
    ) -> Result<VectorResult, AuroraError>;

    // Batch vector search
    pub async fn vector_search_batch(
        &self,
        query_vectors: &[Vec<f32>],
        k: usize,
        table: &str,
        column: &str
    ) -> Result<Vec<VectorResult>, AuroraError>;
}
```

## 📊 Monitoring API

### Metrics Endpoints

```rust
// Get all metrics
let metrics = db.get_metrics().await?;
println!("Active connections: {}", metrics.get("active_connections").unwrap());

// Get JIT metrics
let jit_status = db.get_jit_status().await?;
println!("JIT compilations: {}", jit_status.compilations_total);

// Get cache statistics
let cache_stats = db.get_cache_stats().await?;
println!("Cache hit rate: {:.2}%", cache_stats.hit_rate * 100.0);
```

### Health Checks

```rust
// Basic health check
let health = db.health_check().await?;
assert_eq!(health.status, "healthy");

// Detailed health check
let detailed_health = db.detailed_health_check().await?;
println!("Database: {}", detailed_health.database_status);
println!("Connections: {}", detailed_health.connection_status);
println!("Storage: {}", detailed_health.storage_status);
```

## 🔧 Administrative API

### User Management

```rust
// Create user
db.create_user("newuser", "password123", &["read", "write"]).await?;

// Drop user
db.drop_user("olduser").await?;

// Change password
db.change_password("user", "newpassword").await?;

// List users
let users = db.list_users().await?;
for user in users {
    println!("User: {}, Roles: {:?}", user.name, user.roles);
}
```

### Database Management

```rust
// Create database
db.create_database("newdb", "utf8").await?;

// Drop database
db.drop_database("olddb").await?;

// List databases
let databases = db.list_databases().await?;

// Get database info
let info = db.get_database_info("mydb").await?;
println!("Size: {} MB, Tables: {}", info.size_mb, info.table_count);
```

### Table Management

```rust
// Create table
db.execute_query(r#"
    CREATE TABLE products (
        id INTEGER PRIMARY KEY,
        name VARCHAR(255) NOT NULL,
        price DECIMAL(10,2),
        embedding VECTOR(384),
        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    )
"#).await?;

// Alter table
db.alter_table("products", "ADD COLUMN category VARCHAR(100)").await?;

// Get table schema
let schema = db.get_table_schema("products").await?;
for column in schema.columns {
    println!("{}: {} ({})",
        column.name,
        column.data_type,
        if column.nullable { "nullable" } else { "not null" }
    );
}

// Analyze table
db.analyze_table("products").await?;
```

### Backup and Recovery

```rust
// Create backup
db.create_backup("/backups/aurora-$(date +%Y%m%d).sql").await?;

// List backups
let backups = db.list_backups().await?;
for backup in backups {
    println!("{}: {} MB", backup.name, backup.size_mb);
}

// Restore from backup
db.restore_backup("/backups/aurora-20241201.sql").await?;

// Point-in-time recovery
db.restore_to_timestamp("2024-12-01 10:30:00").await?;
```

## ⚙️ Configuration API

### Runtime Configuration

```rust
// Get configuration
let config = db.get_configuration().await?;
println!("Max connections: {}", config.get("max_connections").unwrap());

// Set configuration (runtime)
db.set_configuration("jit_enabled", "true").await?;
db.set_configuration("log_level", "debug").await?;

// Reload configuration
db.reload_configuration().await?;
```

### Server Configuration

```rust
// Get server info
let info = db.get_server_info().await?;
println!("Version: {}", info.version);
println!("Uptime: {} seconds", info.uptime_seconds);
println!("Active connections: {}", info.active_connections);

// Get cluster info
let cluster = db.get_cluster_info().await?;
println!("Nodes: {}", cluster.node_count);
println!("Primary: {}", cluster.primary_node);
println!("Replication lag: {}ms", cluster.replication_lag_ms);
```

## 🔍 Error Handling

### Error Types

```rust
#[derive(Debug, thiserror::Error)]
pub enum AuroraError {
    #[error("Connection failed: {0}")]
    ConnectionError(String),

    #[error("Query execution failed: {0}")]
    QueryError(String),

    #[error("Transaction failed: {0}")]
    TransactionError(String),

    #[error("Authentication failed")]
    AuthenticationError,

    #[error("Permission denied: {0}")]
    PermissionDenied(String),

    #[error("Invalid SQL: {0}")]
    InvalidSql(String),

    #[error("Timeout")]
    Timeout,

    #[error("Resource exhausted: {0}")]
    ResourceExhausted(String),
}
```

### Error Handling Examples

```rust
match db.execute_query("SELECT * FROM nonexistent_table").await {
    Ok(result) => println!("Success: {} rows", result.row_count),
    Err(AuroraError::QueryError(msg)) => eprintln!("Query error: {}", msg),
    Err(AuroraError::ConnectionError(msg)) => eprintln!("Connection error: {}", msg),
    Err(e) => eprintln!("Other error: {}", e),
}

// Transaction error handling
let result = db.begin_transaction().await
    .and_then(|mut txn| async move {
        txn.execute("INSERT INTO accounts (balance) VALUES (100)").await?;
        txn.execute("INSERT INTO accounts (balance) VALUES ('invalid')").await?;
        txn.commit().await
    })
    .await;

if let Err(e) = result {
    eprintln!("Transaction failed: {}", e);
    // Transaction automatically rolled back
}
```

## 📈 Performance Tuning API

### JIT Tuning

```rust
// Enable/disable JIT
db.set_jit_enabled(true).await?;

// Set optimization level
db.set_jit_optimization_level(OptimizationLevel::Aggressive).await?;

// Configure cache size
db.set_jit_cache_size_mb(1024).await?;

// Get JIT statistics
let jit_stats = db.get_jit_statistics().await?;
println!("Compilations: {}", jit_stats.total_compilations);
println!("Cache hit rate: {:.2}%", jit_stats.cache_hit_rate * 100.0);
```

### Memory Tuning

```rust
// Adjust buffer pool size
db.set_buffer_pool_size("4GB").await?;

// Configure cache sizes
db.set_query_cache_size_mb(512).await?;
db.set_result_cache_size_mb(256).await?;

// Get memory statistics
let mem_stats = db.get_memory_statistics().await?;
println!("Buffer pool usage: {} MB", mem_stats.buffer_pool_used_mb);
println!("Cache hit rate: {:.2}%", mem_stats.cache_hit_rate * 100.0);
```

### Query Optimization

```rust
// Enable adaptive optimization
db.set_adaptive_optimization_enabled(true).await?;

// Set statistics update interval
db.set_statistics_update_interval_hours(6).await?;

// Force statistics update
db.update_statistics("users").await?;

// Get query plan
let plan = db.explain_query("SELECT * FROM users WHERE age > 21").await?;
println!("Query plan:\n{}", plan);
```

## 🔒 Security API

### Authentication

```rust
// Login
let token = db.authenticate("username", "password").await?;

// Verify token
let claims = db.verify_token(&token).await?;

// Logout
db.logout(&token).await?;
```

### Authorization

```rust
// Check permissions
let has_permission = db.check_permission("user", "read", "users").await?;

// Grant permissions
db.grant_permission("user", "write", "products").await?;

// Revoke permissions
db.revoke_permission("user", "admin", "system").await?;
```

### Audit Logging

```rust
// Enable audit logging
db.set_audit_logging_enabled(true).await?;

// Get audit logs
let logs = db.get_audit_logs("2024-12-01", "2024-12-02").await?;
for log in logs {
    println!("{}: {} performed {} on {}",
        log.timestamp, log.user, log.action, log.resource);
}
```

---

This API reference covers AuroraDB's comprehensive capabilities. For more detailed examples and tutorials, see the [Examples](../examples/) directory.
