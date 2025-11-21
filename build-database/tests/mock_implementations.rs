//! Mock Implementations for Testing
//!
//! Provides mock implementations of database components for isolated unit testing.
//! Enables testing of individual components without full system dependencies.

use aurora_db::*;
use std::collections::HashMap;
use std::sync::Arc;
use parking_lot::RwLock;

/// Mock storage engine for testing
pub struct MockStorageEngine {
    data: Arc<RwLock<HashMap<Vec<u8>, Vec<u8>>>>,
}

impl MockStorageEngine {
    pub fn new() -> Self {
        Self {
            data: Arc::new(RwLock::new(HashMap::new())),
        }
    }

    pub fn with_data(initial_data: HashMap<Vec<u8>, Vec<u8>>) -> Self {
        Self {
            data: Arc::new(RwLock::new(initial_data)),
        }
    }
}

#[async_trait::async_trait]
impl StorageEngine for MockStorageEngine {
    async fn get(&self, key: &[u8]) -> Result<Option<Vec<u8>>, StorageError> {
        Ok(self.data.read().get(key).cloned())
    }

    async fn put(&mut self, key: Vec<u8>, value: Vec<u8>) -> Result<(), StorageError> {
        self.data.write().insert(key, value);
        Ok(())
    }

    async fn delete(&mut self, key: &[u8]) -> Result<(), StorageError> {
        self.data.write().remove(key);
        Ok(())
    }

    async fn range_scan(&self, start: &[u8], end: &[u8]) -> Result<Vec<(Vec<u8>, Vec<u8>)>, StorageError> {
        let data = self.data.read();
        let mut results = vec![];

        for (key, value) in data.iter() {
            if key >= &start.to_vec() && key <= &end.to_vec() {
                results.push((key.clone(), value.clone()));
            }
        }

        results.sort_by(|a, b| a.0.cmp(&b.0));
        Ok(results)
    }

    async fn batch_write(&mut self, operations: Vec<StorageOperation>) -> Result<(), StorageError> {
        let mut data = self.data.write();

        for op in operations {
            match op {
                StorageOperation::Put { key, value } => {
                    data.insert(key, value);
                }
                StorageOperation::Delete { key } => {
                    data.remove(&key);
                }
            }
        }

        Ok(())
    }

    fn stats(&self) -> StorageStats {
        let data = self.data.read();
        StorageStats {
            total_keys: data.len() as u64,
            total_size_bytes: data.values().map(|v| v.len()).sum::<usize>() as u64,
            read_operations: 0, // Not tracked in mock
            write_operations: 0, // Not tracked in mock
        }
    }
}

/// Mock network connection for testing
pub struct MockConnection {
    pub id: u64,
    pub sent_messages: Arc<RwLock<Vec<AuroraMessage>>>,
    pub received_messages: Arc<RwLock<Vec<AuroraMessage>>>,
    pub should_fail: bool,
}

impl MockConnection {
    pub fn new(id: u64) -> Self {
        Self {
            id,
            sent_messages: Arc::new(RwLock::new(vec![])),
            received_messages: Arc::new(RwLock::new(vec![])),
            should_fail: false,
        }
    }

    pub fn failing(id: u64) -> Self {
        Self {
            id,
            sent_messages: Arc::new(RwLock::new(vec![])),
            received_messages: Arc::new(RwLock::new(vec![])),
            should_fail: true,
        }
    }

    pub fn queue_message(&mut self, message: AuroraMessage) {
        self.received_messages.write().push(message);
    }

    pub fn sent_messages(&self) -> Vec<AuroraMessage> {
        self.sent_messages.read().clone()
    }
}

#[async_trait::async_trait]
impl Connection for MockConnection {
    async fn handshake(&mut self) -> Result<(), ConnectionError> {
        if self.should_fail {
            return Err(ConnectionError::AuthenticationFailed("Mock failure".to_string()));
        }
        Ok(())
    }

    async fn send_message(&mut self, message: &AuroraMessage) -> Result<(), ConnectionError> {
        if self.should_fail {
            return Err(ConnectionError::ConnectionClosed);
        }
        self.sent_messages.write().push(message.clone());
        Ok(())
    }

    async fn receive_message(&mut self) -> Result<AuroraMessage, ConnectionError> {
        if self.should_fail {
            return Err(ConnectionError::ConnectionClosed);
        }

        let mut received = self.received_messages.write();
        received.pop().ok_or(ConnectionError::ConnectionClosed)
    }

    fn is_idle(&self) -> bool {
        false // Mock connections are never idle
    }

    fn stats(&self) -> &ConnectionStats {
        // Return a dummy stats object
        Box::leak(Box::new(ConnectionStats::default()))
    }

    fn state(&self) -> &ConnectionState {
        &ConnectionState::Ready
    }

    async fn close(&mut self) -> Result<(), ConnectionError> {
        Ok(())
    }
}

/// Mock query executor for testing query processing
pub struct MockQueryExecutor {
    pub executed_queries: Arc<RwLock<Vec<String>>>,
    pub mock_results: HashMap<String, QueryResult>,
}

impl MockQueryExecutor {
    pub fn new() -> Self {
        Self {
            executed_queries: Arc::new(RwLock::new(vec![])),
            mock_results: HashMap::new(),
        }
    }

    pub fn with_result(mut self, query: &str, result: QueryResult) -> Self {
        self.mock_results.insert(query.to_string(), result);
        self
    }

    pub async fn execute(&mut self, query: &str) -> Result<QueryResult, QueryError> {
        self.executed_queries.write().push(query.to_string());

        if let Some(result) = self.mock_results.get(query) {
            Ok(result.clone())
        } else {
            // Return a default successful result
            Ok(QueryResult {
                data: vec![format!("Mock result for: {}", query)],
                row_count: 1,
                execution_time_ms: 1.0,
                columns: vec!["result".to_string()],
            })
        }
    }

    pub fn executed_queries(&self) -> Vec<String> {
        self.executed_queries.read().clone()
    }
}

/// Mock transaction manager for testing transaction logic
pub struct MockTransactionManager {
    pub active_transactions: Arc<RwLock<HashMap<TransactionId, TransactionStatus>>>,
    pub committed_transactions: Arc<RwLock<Vec<TransactionId>>>,
    pub rolled_back_transactions: Arc<RwLock<Vec<TransactionId>>>,
    pub should_fail: bool,
}

impl MockTransactionManager {
    pub fn new() -> Self {
        Self {
            active_transactions: Arc::new(RwLock::new(HashMap::new())),
            committed_transactions: Arc::new(RwLock::new(vec![])),
            rolled_back_transactions: Arc::new(RwLock::new(vec![])),
            should_fail: false,
        }
    }

    pub fn failing() -> Self {
        Self {
            active_transactions: Arc::new(RwLock::new(HashMap::new())),
            committed_transactions: Arc::new(RwLock::new(vec![])),
            rolled_back_transactions: Arc::new(RwLock::new(vec![])),
            should_fail: true,
        }
    }

    pub async fn begin_transaction(&mut self) -> Result<TransactionId, TransactionError> {
        if self.should_fail {
            return Err(TransactionError::TransactionNotFound(TransactionId(0)));
        }

        let txn_id = TransactionId(rand::random::<u64>());
        self.active_transactions.write().insert(txn_id, TransactionStatus::Active);
        Ok(txn_id)
    }

    pub async fn commit_transaction(&mut self, txn_id: TransactionId) -> Result<(), TransactionError> {
        if self.should_fail {
            return Err(TransactionError::InvalidTransactionState(TransactionStatus::Active));
        }

        let mut active = self.active_transactions.write();
        if active.remove(&txn_id).is_some() {
            self.committed_transactions.write().push(txn_id);
            Ok(())
        } else {
            Err(TransactionError::TransactionNotFound(txn_id))
        }
    }

    pub async fn rollback_transaction(&mut self, txn_id: TransactionId) -> Result<(), TransactionError> {
        if self.should_fail {
            return Err(TransactionError::InvalidTransactionState(TransactionStatus::Active));
        }

        let mut active = self.active_transactions.write();
        if active.remove(&txn_id).is_some() {
            self.rolled_back_transactions.write().push(txn_id);
            Ok(())
        } else {
            Err(TransactionError::TransactionNotFound(txn_id))
        }
    }

    pub fn is_committed(&self, txn_id: TransactionId) -> bool {
        self.committed_transactions.read().contains(&txn_id)
    }

    pub fn is_rolled_back(&self, txn_id: TransactionId) -> bool {
        self.rolled_back_transactions.read().contains(&txn_id)
    }

    pub fn is_active(&self, txn_id: TransactionId) -> bool {
        self.active_transactions.read().contains_key(&txn_id)
    }
}

/// Mock clock for testing time-dependent behavior
pub struct MockClock {
    pub current_time: Arc<RwLock<u64>>,
}

impl MockClock {
    pub fn new() -> Self {
        Self {
            current_time: Arc::new(RwLock::new(0)),
        }
    }

    pub fn advance(&mut self, milliseconds: u64) {
        *self.current_time.write() += milliseconds;
    }

    pub fn set_time(&mut self, time: u64) {
        *self.current_time.write() = time;
    }

    pub fn current_time(&self) -> u64 {
        *self.current_time.read()
    }
}

/// Mock metrics collector for testing monitoring
pub struct MockMetricsCollector {
    pub metrics: Arc<RwLock<HashMap<String, f64>>>,
}

impl MockMetricsCollector {
    pub fn new() -> Self {
        Self {
            metrics: Arc::new(RwLock::new(HashMap::new())),
        }
    }

    pub fn record(&mut self, name: &str, value: f64) {
        self.metrics.write().insert(name.to_string(), value);
    }

    pub fn get(&self, name: &str) -> Option<f64> {
        self.metrics.read().get(name).cloned()
    }

    pub fn increment(&mut self, name: &str) {
        let mut metrics = self.metrics.write();
        let current = metrics.get(name).unwrap_or(&0.0);
        metrics.insert(name.to_string(), current + 1.0);
    }
}

/// Test data generators
pub struct TestDataGenerator;

impl TestDataGenerator {
    /// Generate test table with specified number of rows
    pub fn generate_test_table(rows: usize) -> Vec<(i64, String, String)> {
        (0..rows).map(|i| {
            (i as i64,
             format!("Name{}", i),
             format!("user{}@example.com", i))
        }).collect()
    }

    /// Generate random SQL queries for testing
    pub fn generate_random_queries(count: usize) -> Vec<String> {
        use rand::seq::SliceRandom;

        let templates = vec![
            "SELECT * FROM users WHERE id = {}",
            "INSERT INTO users (name, email) VALUES ('{}', '{}')",
            "UPDATE users SET name = '{}' WHERE id = {}",
            "DELETE FROM users WHERE id = {}",
        ];

        let mut rng = rand::thread_rng();
        (0..count).map(|i| {
            let template = templates.choose(&mut rng).unwrap();
            match template.matches("{}").count() {
                1 => format!(template, i),
                2 => format!(template, format!("Value{}", i), i),
                _ => template.to_string(),
            }
        }).collect()
    }

    /// Generate test workload patterns
    pub fn generate_workload(read_ratio: f64, total_operations: usize) -> Vec<String> {
        let reads = (total_operations as f64 * read_ratio) as usize;
        let writes = total_operations - reads;

        let mut queries = vec![];

        // Generate read queries
        for i in 0..reads {
            queries.push(format!("SELECT * FROM test_table WHERE id = {}", i % 1000));
        }

        // Generate write queries
        for i in 0..writes {
            queries.push(format!("INSERT INTO test_table (id, data) VALUES ({}, 'write_{}')", i, i));
        }

        // Shuffle to simulate realistic workload
        use rand::seq::SliceRandom;
        let mut rng = rand::thread_rng();
        queries.shuffle(&mut rng);

        queries
    }
}
