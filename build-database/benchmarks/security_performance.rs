//! Security Performance Benchmarks
//!
//! Measures the performance overhead of AuroraDB's security features:
//! - Authentication overhead
//! - Authorization overhead
//! - Encryption overhead
//! - Audit logging overhead
//! - End-to-end query security overhead
//! - Comparative benchmarks with/without security

use std::sync::Arc;
use std::time::{Duration, Instant};
use criterion::{black_box, criterion_group, criterion_main, Criterion};
use tokio::runtime::Runtime;
use auroradb::config::DatabaseConfig;
use auroradb::engine::AuroraDB;
use auroradb::core::UserContext;

fn benchmark_security_overhead(c: &mut Criterion) {
    let rt = Runtime::new().unwrap();
    let database = rt.block_on(setup_secure_database());
    let user_session = rt.block_on(setup_test_user(&database));

    let user_context = UserContext {
        user_id: "benchmark_user".to_string(),
        session_id: Some(user_session),
        client_ip: Some("127.0.0.1".to_string()),
        user_agent: Some("Benchmark-Client/1.0".to_string()),
    };

    // Benchmark authentication overhead
    c.bench_function("authentication_overhead", |b| {
        b.iter(|| {
            rt.block_on(async {
                black_box(database.auth_manager.authenticate(
                    "benchmark_user",
                    "BenchPass123!",
                    Some("127.0.0.1")
                ).unwrap())
            })
        })
    });

    // Benchmark authorization overhead
    c.bench_function("authorization_overhead", |b| {
        b.iter(|| {
            rt.block_on(async {
                let authz_context = auroradb::security::authorization::AuthzContext {
                    user_id: "benchmark_user".to_string(),
                    session_id: Some(user_context.session_id.clone().unwrap()),
                    client_ip: Some("127.0.0.1".to_string()),
                    user_agent: Some("Benchmark-Client/1.0".to_string()),
                    resource_attributes: std::collections::HashMap::new(),
                    environment_attributes: std::collections::HashMap::new(),
                };

                black_box(database.authz_manager.authorize(
                    &authz_context,
                    &auroradb::security::rbac::Permission::SelectTable("*".to_string())
                ).await.unwrap())
            })
        })
    });

    // Benchmark audit logging overhead
    c.bench_function("audit_logging_overhead", |b| {
        b.iter(|| {
            black_box(database.audit_logger.log_data_access(
                "benchmark_user", "test_table", "SELECT", 1, Some("bench_session")
            ).unwrap())
        })
    });

    // Benchmark encryption overhead
    let test_data = b"This is test data for encryption benchmarking that should be a reasonable size for performance testing.";
    let key_id = "bench_key".to_string();
    rt.block_on(async {
        database.encryption_manager.generate_data_key(key_id.clone()).unwrap();
    });

    c.bench_function("encryption_overhead", |b| {
        b.iter(|| {
            rt.block_on(async {
                black_box(database.encryption_manager.encrypt_data(
                    test_data.as_ref(),
                    &key_id,
                    None
                ).unwrap())
            })
        })
    });

    c.bench_function("decryption_overhead", |b| {
        let encrypted = rt.block_on(async {
            database.encryption_manager.encrypt_data(test_data.as_ref(), &key_id, None).unwrap()
        });

        b.iter(|| {
            rt.block_on(async {
                black_box(database.encryption_manager.decrypt_data(&encrypted).unwrap())
            })
        })
    });

    // Benchmark end-to-end query with security
    c.bench_function("end_to_end_query_security", |b| {
        b.iter(|| {
            rt.block_on(async {
                black_box(database.execute_query(
                    "SELECT * FROM users",
                    &user_context
                ).await.unwrap())
            })
        })
    });
}

fn benchmark_security_throughput(c: &mut Criterion) {
    let rt = Runtime::new().unwrap();
    let database = rt.block_on(setup_secure_database());

    c.bench_function("concurrent_authentication_10", |b| {
        b.iter(|| {
            rt.block_on(async {
                let mut tasks = Vec::new();
                for i in 0..10 {
                    let db_clone = Arc::clone(&database);
                    let task = tokio::spawn(async move {
                        black_box(db_clone.auth_manager.authenticate(
                            &format!("user{}", i),
                            "TestPass123!",
                            Some("127.0.0.1")
                        ).unwrap())
                    });
                    tasks.push(task);
                }

                for task in tasks {
                    black_box(task.await.unwrap());
                }
            })
        })
    });

    c.bench_function("concurrent_secure_queries_50", |b| {
        b.iter(|| {
            rt.block_on(async {
                let user_session = database.auth_manager.authenticate(
                    "benchmark_user", "BenchPass123!", Some("127.0.0.1")
                ).await.unwrap().session_id;

                let user_context = UserContext {
                    user_id: "benchmark_user".to_string(),
                    session_id: Some(user_session),
                    client_ip: Some("127.0.0.1".to_string()),
                    user_agent: Some("Benchmark-Client/1.0".to_string()),
                };

                let mut tasks = Vec::new();
                for _ in 0..50 {
                    let db_clone = Arc::clone(&database);
                    let ctx_clone = user_context.clone();
                    let task = tokio::spawn(async move {
                        black_box(db_clone.execute_query(
                            "SELECT * FROM users",
                            &ctx_clone
                        ).await.unwrap())
                    });
                    tasks.push(task);
                }

                for task in tasks {
                    black_box(task.await.unwrap());
                }
            })
        })
    });
}

async fn setup_secure_database() -> Arc<AuroraDB> {
    let temp_dir = tempfile::tempdir().unwrap();
    let data_dir = temp_dir.path().to_string();

    let db_config = DatabaseConfig {
        data_directory: data_dir,
        ..DatabaseConfig::default()
    };

    Arc::new(AuroraDB::new(db_config).await.unwrap())
}

async fn setup_test_user(database: &AuroraDB) -> String {
    // Register test user
    let user_id = database.auth_manager.register_user(
        "benchmark_user".to_string(),
        "BenchPass123!".to_string(),
        "benchmark@test.com".to_string(),
    ).unwrap();

    // Assign role
    database.rbac_manager.grant_role_to_user(&user_id, "user").unwrap();

    // Authenticate and return session
    database.auth_manager.authenticate(
        "benchmark_user",
        "BenchPass123!",
        Some("127.0.0.1")
    ).await.unwrap().session_id
}

fn benchmark_security_scalability(c: &mut Criterion) {
    let rt = Runtime::new().unwrap();

    c.bench_function("security_user_scale_100", |b| {
        b.iter(|| {
            rt.block_on(async {
                let database = setup_secure_database().await;

                // Create 100 users
                let mut user_ids = Vec::new();
                for i in 0..100 {
                    let user_id = database.auth_manager.register_user(
                        format!("scaleuser{}", i),
                        "ScalePass123!".to_string(),
                        format!("scaleuser{}@test.com", i),
                    ).unwrap();
                    user_ids.push(user_id);
                }

                // Assign roles to all users
                for user_id in &user_ids {
                    black_box(database.rbac_manager.grant_role_to_user(user_id, "user").unwrap());
                }

                // Authenticate all users
                for i in 0..100 {
                    black_box(database.auth_manager.authenticate(
                        &format!("scaleuser{}", i),
                        "ScalePass123!",
                        Some("127.0.0.1")
                    ).await.unwrap());
                }

                black_box(user_ids.len())
            })
        })
    });

    c.bench_function("security_audit_scale_1000", |b| {
        b.iter(|| {
            rt.block_on(async {
                let database = setup_secure_database().await;

                // Generate 1000 audit events
                for i in 0..1000 {
                    black_box(database.audit_logger.log_data_access(
                        &format!("user{}", i % 10),
                        "test_table",
                        "SELECT",
                        1,
                        Some(&format!("session{}", i % 5))
                    ).unwrap());
                }

                let stats = database.audit_logger.get_audit_stats();
                black_box(stats.total_events)
            })
        })
    });
}

fn benchmark_security_memory_overhead(c: &mut Criterion) {
    let rt = Runtime::new().unwrap();

    c.bench_function("security_memory_baseline", |b| {
        b.iter(|| {
            rt.block_on(async {
                // Measure memory usage of database without security operations
                let database = setup_secure_database().await;

                // Small delay to stabilize memory
                tokio::time::sleep(Duration::from_millis(10)).await;

                black_box(database.rbac_manager.list_users().len())
            })
        })
    });

    c.bench_function("security_memory_with_sessions", |b| {
        b.iter(|| {
            rt.block_on(async {
                let database = setup_secure_database().await;

                // Create multiple active sessions
                let mut sessions = Vec::new();
                for i in 0..20 {
                    let session = database.auth_manager.authenticate(
                        "benchmark_user",
                        "BenchPass123!",
                        Some("127.0.0.1")
                    ).await.unwrap();
                    sessions.push(session.session_id);
                }

                // Validate all sessions (memory pressure)
                for session_id in &sessions {
                    black_box(database.auth_manager.validate_session(session_id).unwrap());
                }

                black_box(sessions.len())
            })
        })
    });
}

criterion_group!(
    benches,
    benchmark_security_overhead,
    benchmark_security_throughput,
    benchmark_security_scalability,
    benchmark_security_memory_overhead
);
criterion_main!(benches);
