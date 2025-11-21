//! Table Storage Layer
//!
//! Provides schema-aware data storage and retrieval for tables.
//! Integrates with the B+ Tree storage engine to provide table-specific operations.

use std::sync::Arc;
use std::collections::HashMap;
use serde::{Serialize, Deserialize};
use crate::core::{AuroraResult, AuroraError, ErrorCode};
use crate::storage::btree::engine::WorkingBTreeEngine;
use crate::storage::wal_logger::{WALLogger, WALRecord};
use crate::catalog::{TableCatalog, ColumnMetadata};
use crate::types::DataValue;
use crate::mvcc::{TransactionManager, TransactionId, TupleVersionChain, VersionedTuple, VisibilityChecker};
use std::collections::HashMap;

/// Row data stored in a table
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TableRow {
    /// Table name this row belongs to
    pub table_name: String,
    /// Primary key value for this row
    pub primary_key: DataValue,
    /// Column data as key-value pairs
    pub data: HashMap<String, DataValue>,
}

/// Table storage manager with MVCC and WAL durability
pub struct TableStorage {
    /// Underlying storage engine
    storage_engine: Arc<WorkingBTreeEngine>,
    /// Catalog for schema information
    catalog: Arc<TableCatalog>,
    /// WAL logger for transaction durability
    wal_logger: Arc<WALLogger>,
    /// Transaction manager for MVCC
    transaction_manager: Arc<TransactionManager>,
}

impl TableStorage {
    /// Create a new table storage manager with MVCC and WAL durability
    pub fn new(storage_engine: Arc<WorkingBTreeEngine>, catalog: Arc<TableCatalog>, wal_logger: Arc<WALLogger>) -> Self {
        Self {
            storage_engine,
            catalog,
            wal_logger,
            transaction_manager: Arc::new(TransactionManager::new()),
        }
    }

    /// Insert a row into a table with MVCC and WAL durability
    pub async fn insert_row(&self, transaction: &crate::mvcc::transaction::Transaction, table_name: &str, row_data: HashMap<String, DataValue>) -> AuroraResult<()> {
        // Verify table exists
        if !self.catalog.table_exists(table_name).await {
            return Err(AuroraError::new(
                ErrorCode::StorageCorruption,
                format!("Table '{}' does not exist", table_name)
            ));
        }

        // Get table schema
        let columns = self.catalog.get_columns(table_name).await?;

        // Validate and prepare row data
        let validated_data = self.validate_and_prepare_data(table_name, &columns, row_data)?;

        // Create new versioned tuple
        let primary_key = self.extract_primary_key(&validated_data, &columns)?;
        let versioned_tuple = VersionedTuple::new(primary_key.clone(), validated_data, transaction.id);

        // Check for existing tuple with same primary key
        let existing_chain = self.get_tuple_chain(table_name, &primary_key).await?;
        if let Some(chain) = existing_chain {
            if chain.is_visible_to(transaction, &self.transaction_manager) {
                return Err(AuroraError::new(
                    ErrorCode::ConstraintViolation,
                    format!("Primary key violation: tuple already exists in table '{}'", table_name)
                ));
            }
        }

        // Create version chain if it doesn't exist
        let mut version_chain = existing_chain.unwrap_or_else(|| TupleVersionChain::new(versioned_tuple.clone()));

        // Serialize version chain
        let serialized_data = bincode::serialize(&version_chain)
            .map_err(|e| AuroraError::new(ErrorCode::StorageUnavailable, format!("Serialization error: {}", e)))?;

        // Generate storage key
        let storage_key = self.generate_tuple_key(table_name, &primary_key);

        // Log the operation to WAL BEFORE storing (write-ahead logging)
        self.wal_logger.log_insert(transaction.id, table_name, &storage_key, &serialized_data).await
            .map_err(|e| AuroraError::new(ErrorCode::StorageUnavailable, format!("WAL logging failed: {}", e)))?;

        // Store in B+ Tree
        self.storage_engine.insert(storage_key, serialized_data).await?;

        log::debug!("Inserted row into table '{}': {:?}", table_name, primary_key);
        Ok(())
    }

    /// Get a row from a table by primary key
    pub async fn get_row(&self, table_name: &str, primary_key: &DataValue) -> AuroraResult<Option<TableRow>> {
        // Generate storage key
        let storage_key = self.generate_key_for_table_row(table_name, primary_key);

        // Retrieve from storage
        match self.storage_engine.get(&storage_key).await? {
            Some(data) => {
                let row: TableRow = bincode::deserialize(&data)
                    .map_err(|e| AuroraError::new(ErrorCode::StorageCorruption, format!("Deserialization error: {}", e)))?;
                Ok(Some(row))
            }
            None => Ok(None),
        }
    }

    /// Scan all visible rows in a table using MVCC
    pub async fn scan_table(&self, transaction: &crate::mvcc::transaction::Transaction, table_name: &str) -> AuroraResult<Vec<HashMap<String, DataValue>>> {
        // Generate table prefix for scanning
        let table_prefix = format!("table:{}:", table_name);

        // Get all keys with this prefix
        let all_data = self.storage_engine.scan_prefix(&table_prefix).await?;

        let mut visible_rows = Vec::new();
        for (_, data) in all_data {
            let version_chain: TupleVersionChain = bincode::deserialize(&data)
                .map_err(|e| AuroraError::new(ErrorCode::StorageCorruption, format!("Deserialization error: {}", e)))?;

            // Get the visible version for this transaction
            if let Some(visible_version) = version_chain.visible_version(transaction, &self.transaction_manager) {
                visible_rows.push(visible_version.data.clone());
            }
        }

        Ok(visible_rows)
    }

    /// Update a row in a table with MVCC versioning
    pub async fn update_row(&self, transaction: &crate::mvcc::transaction::Transaction, table_name: &str, primary_key: &DataValue, new_data: HashMap<String, DataValue>) -> AuroraResult<bool> {
        // Verify table exists
        if !self.catalog.table_exists(table_name).await {
            return Err(AuroraError::new(
                ErrorCode::StorageCorruption,
                format!("Table '{}' does not exist", table_name)
            ));
        }

        // Get table schema for validation
        let columns = self.catalog.get_columns(table_name).await?;

        // Validate the new data
        self.validate_and_prepare_data(table_name, &columns, new_data.clone())?;

        // Get existing version chain
        let mut version_chain = match self.get_tuple_chain(table_name, primary_key).await? {
            Some(chain) => chain,
            None => return Ok(false), // Row doesn't exist
        };

        // Check if the current version is visible to this transaction
        let current_version = match version_chain.visible_version(transaction, &self.transaction_manager) {
            Some(version) => version,
            None => return Ok(false), // No visible version
        };

        // Create new version with updated data
        let new_version = current_version.new_version(new_data, transaction.id);

        // Serialize updated version chain
        let updated_chain = TupleVersionChain::new(new_version); // Simplified - should preserve history
        let serialized_data = bincode::serialize(&updated_chain)
            .map_err(|e| AuroraError::new(ErrorCode::StorageUnavailable, format!("Serialization error: {}", e)))?;

        // Generate storage key
        let storage_key = self.generate_tuple_key(table_name, primary_key);

        // Log the operation to WAL BEFORE storing
        self.wal_logger.log_update(
            transaction.id,
            table_name,
            &storage_key,
            &bincode::serialize(&current_version.data).unwrap_or_default(),
            &bincode::serialize(&new_version.data).unwrap_or_default()
        ).await
            .map_err(|e| AuroraError::new(ErrorCode::StorageUnavailable, format!("WAL logging failed: {}", e)))?;

        // Store the updated version chain
        self.storage_engine.insert(storage_key, serialized_data).await?;

        log::debug!("Updated row in table '{}': {:?}", table_name, primary_key);
        Ok(true)
    }

    /// Update a row in a table
    pub async fn update_row(&self, table_name: &str, primary_key: &DataValue, updates: HashMap<String, DataValue>) -> AuroraResult<bool> {
        // Get existing row
        let mut existing_row = match self.get_row(table_name, primary_key).await? {
            Some(row) => row,
            None => return Ok(false), // Row doesn't exist
        };

        // Get table schema for validation
        let columns = self.catalog.get_columns(table_name).await?;

        // Validate updates
        for (column_name, value) in &updates {
            // Check column exists
            if !columns.iter().any(|c| c.name == *column_name) {
                return Err(AuroraError::new(
                    ErrorCode::ValidationConstraintViolation,
                    format!("Column '{}' does not exist in table '{}'", column_name, table_name)
                ));
            }

            // Validate data type
            self.validate_column_value(&columns, column_name, value)?;
        }

        // Apply updates
        for (column_name, value) in updates {
            existing_row.data.insert(column_name, value);
        }

        // Store updated row
        let storage_key = self.generate_row_key(&existing_row);
        let serialized_data = bincode::serialize(&existing_row)
            .map_err(|e| AuroraError::new(ErrorCode::StorageUnavailable, format!("Serialization error: {}", e)))?;

        self.storage_engine.insert(storage_key, serialized_data).await?;

        log::debug!("Updated row in table '{}': {:?}", table_name, primary_key);
        Ok(true)
    }

    /// Delete a row from a table with MVCC versioning
    pub async fn delete_row(&self, transaction: &crate::mvcc::transaction::Transaction, table_name: &str, primary_key: &DataValue) -> AuroraResult<bool> {
        // Get existing version chain
        let mut version_chain = match self.get_tuple_chain(table_name, primary_key).await? {
            Some(chain) => chain,
            None => return Ok(false), // Row doesn't exist
        };

        // Check if the current version is visible to this transaction
        let current_version = match version_chain.visible_version(transaction, &self.transaction_manager) {
            Some(version) => version,
            None => return Ok(false), // No visible version
        };

        // Mark the current version as deleted by this transaction
        version_chain.delete_current(transaction.id);

        // Serialize updated version chain
        let serialized_data = bincode::serialize(&version_chain)
            .map_err(|e| AuroraError::new(ErrorCode::StorageUnavailable, format!("Serialization error: {}", e)))?;

        // Generate storage key
        let storage_key = self.generate_tuple_key(table_name, primary_key);

        // Log the operation to WAL BEFORE storing
        self.wal_logger.log_delete(
            transaction.id,
            table_name,
            &storage_key,
            &bincode::serialize(&current_version.data).unwrap_or_default()
        ).await
            .map_err(|e| AuroraError::new(ErrorCode::StorageUnavailable, format!("WAL logging failed: {}", e)))?;

        // Store the updated version chain
        self.storage_engine.insert(storage_key, serialized_data).await?;

        log::debug!("Deleted row from table '{}': {:?}", table_name, primary_key);
        Ok(true)
    }

    /// Delete all rows in a table (for DROP TABLE)
    pub async fn delete_table_data(&self, table_name: &str) -> AuroraResult<()> {
        let table_prefix = format!("table:{}:", table_name);

        // Get all keys with this prefix and delete them
        let keys_to_delete = self.storage_engine.scan_prefix_keys_only(&table_prefix).await?;

        for key in keys_to_delete {
            self.storage_engine.delete(&key).await?;
        }

        log::info!("Deleted all data for table '{}'", table_name);
        Ok(())
    }

    /// Get version chain for a tuple
    async fn get_tuple_chain(&self, table_name: &str, primary_key: &DataValue) -> AuroraResult<Option<TupleVersionChain>> {
        let storage_key = self.generate_tuple_key(table_name, primary_key);

        match self.storage_engine.get(&storage_key).await? {
            Some(data) => {
                let chain: TupleVersionChain = bincode::deserialize(&data)
                    .map_err(|e| AuroraError::new(ErrorCode::StorageCorruption, format!("Deserialization error: {}", e)))?;
                Ok(Some(chain))
            }
            None => Ok(None),
        }
    }

    /// Extract primary key from row data
    fn extract_primary_key(&self, data: &HashMap<String, DataValue>, columns: &[ColumnMetadata]) -> AuroraResult<DataValue> {
        // Find primary key column (simplified - assumes first column or 'id' column)
        let pk_column = columns.iter()
            .find(|c| c.name == "id")
            .or_else(|| columns.first())
            .ok_or_else(|| AuroraError::new(
                ErrorCode::StorageCorruption,
                "No primary key column found".to_string()
            ))?;

        data.get(&pk_column.name)
            .cloned()
            .ok_or_else(|| AuroraError::new(
                ErrorCode::ValidationRequiredField,
                format!("Primary key column '{}' is required", pk_column.name)
            ))
    }

    /// Generate storage key for a tuple
    fn generate_tuple_key(&self, table_name: &str, primary_key: &DataValue) -> Vec<u8> {
        format!("table:{}:pk:{:?}", table_name, primary_key).into_bytes()
    }

    /// Validate and prepare row data for storage
    fn validate_and_prepare_row(
        &self,
        table_name: &str,
        columns: &[ColumnMetadata],
        row_data: HashMap<String, DataValue>
    ) -> AuroraResult<TableRow> {
        let mut validated_data = HashMap::new();

        // Find primary key column
        let primary_key_column = columns.iter()
            .find(|c| c.name == "id") // Simple assumption - first column is PK for now
            .or_else(|| columns.first())
            .ok_or_else(|| AuroraError::new(
                ErrorCode::StorageCorruption,
                format!("No primary key column found for table '{}'", table_name)
            ))?;

        // Extract primary key value
        let primary_key = row_data.get(&primary_key_column.name)
            .ok_or_else(|| AuroraError::new(
                ErrorCode::ValidationRequiredField,
                format!("Primary key column '{}' is required", primary_key_column.name)
            ))?
            .clone();

        // Validate each provided value
        for (column_name, value) in row_data {
            // Check column exists and validate type
            self.validate_column_value(columns, &column_name, &value)?;
            validated_data.insert(column_name, value);
        }

        // Ensure NOT NULL columns have values
        for column in columns {
            if !column.nullable && !validated_data.contains_key(&column.name) {
                // Check if there's a default value
                if let Some(default_expr) = &column.default_value {
                    // For now, use a simple default
                    let default_value = match column.data_type {
                        crate::types::DataType::Integer => DataValue::Integer(0),
                        crate::types::DataType::Text => DataValue::Text("".to_string()),
                        crate::types::DataType::Boolean => DataValue::Boolean(false),
                        _ => DataValue::Null,
                    };
                    validated_data.insert(column.name.clone(), default_value);
                } else {
                    return Err(AuroraError::new(
                        ErrorCode::ValidationRequiredField,
                        format!("Column '{}' cannot be null", column.name)
                    ));
                }
            }
        }

        Ok(TableRow {
            table_name: table_name.to_string(),
            primary_key,
            data: validated_data,
        })
    }

    /// Validate a column value against its schema
    fn validate_column_value(&self, columns: &[ColumnMetadata], column_name: &str, value: &DataValue) -> AuroraResult<()> {
        let column = columns.iter().find(|c| c.name == column_name)
            .ok_or_else(|| AuroraError::new(
                ErrorCode::ValidationConstraintViolation,
                format!("Column '{}' does not exist", column_name)
            ))?;

        // Type validation
        match (&column.data_type, value) {
            (crate::types::DataType::Integer, DataValue::Integer(_)) => Ok(()),
            (crate::types::DataType::BigInt, DataValue::BigInt(_)) => Ok(()),
            (crate::types::DataType::Float, DataValue::Float(_)) => Ok(()),
            (crate::types::DataType::Double, DataValue::Double(_)) => Ok(()),
            (crate::types::DataType::Text, DataValue::Text(_)) => Ok(()),
            (crate::types::DataType::Boolean, DataValue::Boolean(_)) => Ok(()),
            (crate::types::DataType::Blob, DataValue::Blob(_)) => Ok(()),
            (_, DataValue::Null) if column.nullable => Ok(()),
            _ => Err(AuroraError::new(
                ErrorCode::ValidationTypeMismatch,
                format!("Type mismatch for column '{}': expected {:?}, got {:?}", column_name, column.data_type, value)
            )),
        }
    }

    /// Generate storage key for a row
    fn generate_row_key(&self, row: &TableRow) -> Vec<u8> {
        format!("table:{}:pk:{:?}", row.table_name, row.primary_key).into_bytes()
    }

    /// Generate storage key for a specific table row
    fn generate_key_for_table_row(&self, table_name: &str, primary_key: &DataValue) -> Vec<u8> {
        format!("table:{}:pk:{:?}", table_name, primary_key).into_bytes()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use tempfile::tempdir;
    use crate::config::DatabaseConfig;

    #[tokio::test]
    async fn test_table_storage_operations() {
        let temp_dir = tempdir().unwrap();
        let config = DatabaseConfig {
            data_directory: temp_dir.path().to_string(),
            ..DatabaseConfig::default()
        };

        // Create storage engine and catalog
        let storage_engine = Arc::new(WorkingBTreeEngine::new(Default::default(), &config.data_directory).await.unwrap());
        let catalog = Arc::new(TableCatalog::new(temp_dir.path().join("catalog")));

        // Create table storage
        let table_storage = TableStorage::new(storage_engine, catalog);

        // Create test table in catalog first
        let create_query = crate::query::parser::ast::CreateTableQuery {
            name: "test_table".to_string(),
            columns: vec![
                crate::query::parser::ast::ColumnDefinition {
                    name: "id".to_string(),
                    data_type: crate::types::DataType::Integer,
                    nullable: false,
                    default: None,
                },
                crate::query::parser::ast::ColumnDefinition {
                    name: "name".to_string(),
                    data_type: crate::types::DataType::Text,
                    nullable: false,
                    default: None,
                },
            ],
            constraints: vec![],
        };

        table_storage.catalog.create_table(&create_query).await.unwrap();

        // Test insert
        let mut row_data = HashMap::new();
        row_data.insert("id".to_string(), DataValue::Integer(1));
        row_data.insert("name".to_string(), DataValue::Text("Alice".to_string()));

        table_storage.insert_row("test_table", row_data).await.unwrap();

        // Test get
        let retrieved = table_storage.get_row("test_table", &DataValue::Integer(1)).await.unwrap();
        assert!(retrieved.is_some());
        let row = retrieved.unwrap();
        assert_eq!(row.table_name, "test_table");
        assert_eq!(row.primary_key, DataValue::Integer(1));
        assert_eq!(row.data.get("name"), Some(&DataValue::Text("Alice".to_string())));

        // Test scan
        let all_rows = table_storage.scan_table("test_table").await.unwrap();
        assert_eq!(all_rows.len(), 1);
        assert_eq!(all_rows[0].primary_key, DataValue::Integer(1));
    }
}
