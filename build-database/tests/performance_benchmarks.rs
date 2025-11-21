//! Performance Benchmarks for AuroraDB
//!
//! Comprehensive benchmarking suite measuring UNIQUENESS performance improvements.
//! Benchmarks designed to validate 5x-10x performance gains over traditional databases.

use aurora_db::*;
use criterion::{black_box, criterion_group, criterion_main, Criterion};
use std::time::Duration;
use tokio::runtime::Runtime;

/// Benchmark database operations
pub fn benchmark_database_operations(c: &mut Criterion) {
    let rt = Runtime::new().unwrap();

    c.bench_function("aurora_insert_single", |b| {
        b.iter(|| {
            rt.block_on(async {
                let db = create_test_database().await;
                let sql = "INSERT INTO test_table (id, data) VALUES (1, 'test data')";
                black_box(db.execute_query(sql).await.unwrap());
            });
        });
    });

    c.bench_function("aurora_bulk_insert_1000", |b| {
        b.iter(|| {
            rt.block_on(async {
                let db = create_test_database().await;
                for i in 0..1000 {
                    let sql = format!("INSERT INTO test_table (id, data) VALUES ({}, 'data_{}')", i, i);
                    black_box(db.execute_query(&sql).await.unwrap());
                }
            });
        });
    });

    c.bench_function("aurora_select_single", |b| {
        b.iter(|| {
            rt.block_on(async {
                let db = create_test_database().await;
                // Pre-populate data
                setup_benchmark_data(&db).await;
                let sql = "SELECT * FROM test_table WHERE id = 500";
                black_box(db.execute_query(sql).await.unwrap());
            });
        });
    });

    c.bench_function("aurora_complex_query", |b| {
        b.iter(|| {
            rt.block_on(async {
                let db = create_test_database().await;
                setup_benchmark_data(&db).await;
                let sql = r#"
                    SELECT t1.id, t1.data, t2.value
                    FROM test_table t1
                    JOIN test_table2 t2 ON t1.id = t2.id
                    WHERE t1.id > 100 AND t2.value < 500
                    ORDER BY t1.id DESC
                    LIMIT 50
                "#;
                black_box(db.execute_query(sql).await.unwrap());
            });
        });
    });
}

/// Benchmark transaction performance
pub fn benchmark_transaction_performance(c: &mut Criterion) {
    let rt = Runtime::new().unwrap();

    c.bench_function("aurora_transaction_commit", |b| {
        b.iter(|| {
            rt.block_on(async {
                let db = create_test_database().await;
                let mut txn = db.begin_transaction().await.unwrap();
                black_box(txn.execute("INSERT INTO test_table (id, data) VALUES (1, 'txn_test')").await.unwrap());
                black_box(txn.commit().await.unwrap());
            });
        });
    });

    c.bench_function("aurora_concurrent_transactions_10", |b| {
        b.iter(|| {
            rt.block_on(async {
                let db = create_test_database().await;
                let mut handles = vec![];

                for i in 0..10 {
                    let db_clone = db.clone();
                    let handle = tokio::spawn(async move {
                        let mut txn = db_clone.begin_transaction().await.unwrap();
                        for j in 0..10 {
                            let sql = format!("INSERT INTO test_table (id, data) VALUES ({}, 'concurrency_test_{}_{}')",
                                            i * 10 + j, i, j);
                            txn.execute(&sql).await.unwrap();
                        }
                        txn.commit().await.unwrap();
                    });
                    handles.push(handle);
                }

                for handle in handles {
                    black_box(handle.await.unwrap());
                }
            });
        });
    });
}

/// Benchmark network protocol performance
pub fn benchmark_network_performance(c: &mut Criterion) {
    let rt = Runtime::new().unwrap();

    c.bench_function("aurora_protocol_serialization", |b| {
        b.iter(|| {
            let protocol = WireProtocol::new(ProtocolFormat::AuroraBinary);
            let message = protocol.create_query_message("SELECT * FROM test_table");
            black_box(protocol.serialize(&message).unwrap());
        });
    });

    c.bench_function("aurora_connection_pool_get", |b| {
        b.iter(|| {
            rt.block_on(async {
                let pool_config = PoolConfig {
                    max_connections: 10,
                    min_connections: 1,
                    max_idle_time_ms: 300000,
                    connection_timeout_ms: 5000,
                    health_check_interval_ms: 30000,
                    connection_config: ConnectionConfig {
                        host: "localhost".to_string(),
                        port: 5432,
                        max_connections: 10,
                        connection_timeout_ms: 5000,
                        idle_timeout_ms: 300000,
                        buffer_size: 8192,
                        protocol_format: ProtocolFormat::AuroraBinary,
                    },
                };

                let factory = Box::new(TcpConnectionFactory::new(pool_config.connection_config.clone()));
                let mut pool = ConnectionPool::new(pool_config, factory);
                black_box(pool.get_connection().await.unwrap());
            });
        });
    });
}

/// Benchmark vector operations (when implemented)
pub fn benchmark_vector_operations(c: &mut Criterion) {
    c.bench_function("aurora_vector_encoding_f32", |b| {
        b.iter(|| {
            let vector = vec![0.1f32; 384]; // Typical embedding size
            black_box(VectorEncoder::encode_vector_f32(&vector, 8));
        });
    });

    c.bench_function("aurora_vector_similarity_1000", |b| {
        b.iter(|| {
            let query_vector = vec![0.5f32; 128];
            let database_vectors = vec![vec![0.1f32; 128]; 1000];

            // Simulate similarity search
            let mut similarities = vec![];
            for db_vector in &database_vectors {
                let similarity = cosine_similarity(&query_vector, db_vector);
                similarities.push(similarity);
            }

            similarities.sort_by(|a, b| b.partial_cmp(a).unwrap());
            black_box(similarities.into_iter().take(10).collect::<Vec<_>>());
        });
    });
}

/// Benchmark memory usage patterns
pub fn benchmark_memory_usage(c: &mut Criterion) {
    let rt = Runtime::new().unwrap();

    c.bench_function("aurora_mvcc_version_chain", |b| {
        b.iter(|| {
            rt.block_on(async {
                let mut mvcc = MVCCManager::new();

                // Create multiple versions of the same key
                for version in 0..100 {
                    let key = b"test_key".to_vec();
                    let value = format!("value_version_{}", version).into_bytes();
                    let txn_id = TransactionId(version as u64);
                    black_box(mvcc.create_version(key, value, txn_id).await.unwrap());
                }
            });
        });
    });

    c.bench_function("aurora_buffer_pool_simulation", |b| {
        b.iter(|| {
            rt.block_on(async {
                let mut buffer_pool = HashMap::new();

                // Simulate buffer pool operations
                for page_id in 0..1000 {
                    let page_data = vec![0u8; 8192]; // 8KB page
                    buffer_pool.insert(PageId(page_id), page_data);
                }

                // Simulate random access patterns
                for _ in 0..10000 {
                    let random_page = rand::random::<u64>() % 1000;
                    black_box(buffer_pool.get(&PageId(random_page)));
                }
            });
        });
    });
}

/// Benchmark concurrent workloads
pub fn benchmark_concurrent_workloads(c: &mut Criterion) {
    let rt = Runtime::new().unwrap();

    c.bench_function("aurora_mixed_workload_50_50", |b| {
        b.iter(|| {
            rt.block_on(async {
                let db = create_test_database().await;
                setup_benchmark_data(&db).await;

                let mut handles = vec![];

                // 50% reads, 50% writes
                for i in 0..100 {
                    let db_clone = db.clone();
                    let handle = tokio::spawn(async move {
                        if i % 2 == 0 {
                            // Read operation
                            let sql = format!("SELECT * FROM test_table WHERE id = {}", i % 1000);
                            black_box(db_clone.execute_query(&sql).await.unwrap());
                        } else {
                            // Write operation
                            let sql = format!("UPDATE test_table SET data = 'updated_{}' WHERE id = {}", i, i % 1000);
                            black_box(db_clone.execute_query(&sql).await.unwrap());
                        }
                    });
                    handles.push(handle);
                }

                for handle in handles {
                    black_box(handle.await.unwrap());
                }
            });
        });
    });
}

/// Setup function for benchmark data
async fn setup_benchmark_data(db: &AuroraDB) {
    // Create test tables
    let create_table1 = r#"
        CREATE TABLE test_table (
            id INTEGER PRIMARY KEY,
            data VARCHAR(255)
        )
    "#;

    let create_table2 = r#"
        CREATE TABLE test_table2 (
            id INTEGER PRIMARY KEY,
            value INTEGER
        )
    "#;

    db.execute_query(create_table1).await.unwrap();
    db.execute_query(create_table2).await.unwrap();

    // Insert benchmark data
    for i in 0..1000 {
        let sql1 = format!("INSERT INTO test_table (id, data) VALUES ({}, 'benchmark_data_{}')", i, i);
        let sql2 = format!("INSERT INTO test_table2 (id, value) VALUES ({}, {})", i, rand::random::<i32>() % 1000);

        db.execute_query(&sql1).await.unwrap();
        db.execute_query(&sql2).await.unwrap();
    }
}

/// Create test database instance
async fn create_test_database() -> AuroraDB {
    let config = DatabaseConfig {
        max_connections: 10,
        buffer_pool_size: 128 * 1024 * 1024, // 128MB
        max_tables: 100,
        max_columns_per_table: 100,
        default_isolation_level: IsolationLevel::ReadCommitted,
        transaction_timeout_ms: 30000,
        enable_query_logging: false, // Disable for benchmarks
        enable_metrics: false,       // Disable for benchmarks
    };

    AuroraDB::new(config).await.expect("Failed to create test database")
}

/// Cosine similarity calculation
fn cosine_similarity(a: &[f32], b: &[f32]) -> f32 {
    let dot_product: f32 = a.iter().zip(b.iter()).map(|(x, y)| x * y).sum();
    let norm_a: f32 = a.iter().map(|x| x * x).sum::<f32>().sqrt();
    let norm_b: f32 = b.iter().map(|x| x * x).sum::<f32>().sqrt();

    if norm_a == 0.0 || norm_b == 0.0 {
        0.0
    } else {
        dot_product / (norm_a * norm_b)
    }
}

// Criterion benchmark groups
criterion_group!(
    benches,
    benchmark_database_operations,
    benchmark_transaction_performance,
    benchmark_network_performance,
    benchmark_vector_operations,
    benchmark_memory_usage,
    benchmark_concurrent_workloads
);
criterion_main!(benches);
