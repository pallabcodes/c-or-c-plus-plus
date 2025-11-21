//! Table Catalog Management
//!
//! Stores and manages table metadata including schemas, columns, and constraints.

use std::collections::HashMap;
use std::sync::Arc;
use tokio::sync::RwLock;
use crate::core::{AuroraResult, AuroraError, ErrorCode};
use crate::query::parser::ast::{CreateTableQuery, DropTableQuery, ColumnDefinition, TableConstraint};
use crate::types::DataType;

/// Table metadata stored in the catalog
#[derive(Debug, Clone, serde::Serialize, serde::Deserialize)]
pub struct TableMetadata {
    pub name: String,
    pub columns: Vec<ColumnMetadata>,
    pub constraints: Vec<TableConstraint>,
    pub created_at: chrono::DateTime<chrono::Utc>,
    pub modified_at: chrono::DateTime<chrono::Utc>,
}

/// Column metadata
#[derive(Debug, Clone, serde::Serialize, serde::Deserialize)]
pub struct ColumnMetadata {
    pub name: String,
    pub data_type: DataType,
    pub nullable: bool,
    pub default_value: Option<String>, // Serialized default value
    pub ordinal_position: usize,
}

/// Table catalog that manages all table metadata
pub struct TableCatalog {
    tables: RwLock<HashMap<String, TableMetadata>>,
    storage_path: std::path::PathBuf,
}

impl TableCatalog {
    /// Create a new table catalog
    pub fn new(storage_path: std::path::PathBuf) -> Self {
        Self {
            tables: RwLock::new(HashMap::new()),
            storage_path,
        }
    }

    /// Create a table from DDL
    pub async fn create_table(&self, create_query: &CreateTableQuery) -> AuroraResult<()> {
        let mut tables = self.tables.write().await;

        // Check if table already exists
        if tables.contains_key(&create_query.name) {
            return Err(AuroraError::new(
                ErrorCode::StorageCorruption,
                format!("Table '{}' already exists", create_query.name)
            ));
        }

        // Convert column definitions to metadata
        let columns = create_query.columns.iter().enumerate()
            .map(|(i, col)| ColumnMetadata {
                name: col.name.clone(),
                data_type: col.data_type.clone(),
                nullable: col.nullable,
                default_value: col.default.as_ref()
                    .map(|expr| format!("{:?}", expr)), // Simplified serialization
                ordinal_position: i,
            })
            .collect();

        // Create table metadata
        let metadata = TableMetadata {
            name: create_query.name.clone(),
            columns,
            constraints: create_query.constraints.clone(),
            created_at: chrono::Utc::now(),
            modified_at: chrono::Utc::now(),
        };

        // Store in catalog
        tables.insert(create_query.name.clone(), metadata);

        // Persist to disk
        self.save_catalog().await?;

        log::info!("Created table: {}", create_query.name);
        Ok(())
    }

    /// Drop a table
    pub async fn drop_table(&self, drop_query: &DropTableQuery) -> AuroraResult<()> {
        let mut tables = self.tables.write().await;

        if drop_query.if_exists && !tables.contains_key(&drop_query.name) {
            return Ok(()); // Table doesn't exist, but IF EXISTS was specified
        }

        if !tables.contains_key(&drop_query.name) {
            return Err(AuroraError::new(
                ErrorCode::StorageCorruption,
                format!("Table '{}' does not exist", drop_query.name)
            ));
        }

        // Remove from catalog
        tables.remove(&drop_query.name);

        // Persist to disk
        self.save_catalog().await?;

        log::info!("Dropped table: {}", drop_query.name);
        Ok(())
    }

    /// Get table metadata
    pub async fn get_table(&self, table_name: &str) -> AuroraResult<Option<TableMetadata>> {
        let tables = self.tables.read().await;
        Ok(tables.get(table_name).cloned())
    }

    /// List all tables
    pub async fn list_tables(&self) -> Vec<String> {
        let tables = self.tables.read().await;
        tables.keys().cloned().collect()
    }

    /// Check if table exists
    pub async fn table_exists(&self, table_name: &str) -> bool {
        let tables = self.tables.read().await;
        tables.contains_key(table_name)
    }

    /// Get column metadata for a table
    pub async fn get_columns(&self, table_name: &str) -> AuroraResult<Vec<ColumnMetadata>> {
        let tables = self.tables.read().await;

        match tables.get(table_name) {
            Some(metadata) => Ok(metadata.columns.clone()),
            None => Err(AuroraError::new(
                ErrorCode::StorageCorruption,
                format!("Table '{}' does not exist", table_name)
            )),
        }
    }

    /// Get column by name
    pub async fn get_column(&self, table_name: &str, column_name: &str) -> AuroraResult<Option<ColumnMetadata>> {
        let columns = self.get_columns(table_name).await?;
        Ok(columns.into_iter().find(|col| col.name == column_name))
    }

    /// Validate data against table schema
    pub async fn validate_data(&self, table_name: &str, data: &HashMap<String, serde_json::Value>) -> AuroraResult<()> {
        let columns = self.get_columns(table_name).await?;

        for column in &columns {
            if let Some(value) = data.get(&column.name) {
                // Validate data type
                self.validate_data_type(&column.data_type, value)?;

                // Check NOT NULL constraint
                if !column.nullable && value.is_null() {
                    return Err(AuroraError::new(
                        ErrorCode::ValidationConstraintViolation,
                        format!("Column '{}' cannot be null", column.name)
                    ));
                }
            } else if !column.nullable && column.default_value.is_none() {
                return Err(AuroraError::new(
                    ErrorCode::ValidationRequiredField,
                    format!("Column '{}' is required", column.name)
                ));
            }
        }

        Ok(())
    }

    /// Validate data type
    fn validate_data_type(&self, expected_type: &DataType, value: &serde_json::Value) -> AuroraResult<()> {
        match (expected_type, value) {
            (DataType::Integer, serde_json::Value::Number(n)) if n.is_i64() => Ok(()),
            (DataType::BigInt, serde_json::Value::Number(n)) if n.is_i64() => Ok(()),
            (DataType::Float, serde_json::Value::Number(_)) => Ok(()),
            (DataType::Double, serde_json::Value::Number(_)) => Ok(()),
            (DataType::Text, serde_json::Value::String(_)) => Ok(()),
            (DataType::Boolean, serde_json::Value::Bool(_)) => Ok(()),
            (DataType::Blob, serde_json::Value::String(_)) => Ok(()), // Base64 encoded
            _ => Err(AuroraError::new(
                ErrorCode::ValidationTypeMismatch,
                format!("Data type mismatch: expected {:?}, got {:?}", expected_type, value)
            )),
        }
    }

    /// Load catalog from disk
    pub async fn load_catalog(&self) -> AuroraResult<()> {
        let catalog_path = self.storage_path.join("catalog.json");

        if !catalog_path.exists() {
            return Ok(()); // No catalog file yet
        }

        let content = tokio::fs::read_to_string(&catalog_path).await
            .map_err(|e| AuroraError::new(ErrorCode::StorageUnavailable, format!("Failed to read catalog: {}", e)))?;

        let tables: HashMap<String, TableMetadata> = serde_json::from_str(&content)
            .map_err(|e| AuroraError::new(ErrorCode::StorageCorruption, format!("Failed to parse catalog: {}", e)))?;

        let mut catalog_tables = self.tables.write().await;
        *catalog_tables = tables;

        log::info!("Loaded catalog with {} tables", catalog_tables.len());
        Ok(())
    }

    /// Save catalog to disk
    async fn save_catalog(&self) -> AuroraResult<()> {
        let tables = self.tables.read().await;
        let content = serde_json::to_string_pretty(&*tables)
            .map_err(|e| AuroraError::new(ErrorCode::StorageCorruption, format!("Failed to serialize catalog: {}", e)))?;

        // Ensure directory exists
        if let Some(parent) = self.storage_path.parent() {
            tokio::fs::create_dir_all(parent).await
                .map_err(|e| AuroraError::new(ErrorCode::StorageUnavailable, format!("Failed to create catalog directory: {}", e)))?;
        }

        let catalog_path = self.storage_path.join("catalog.json");
        tokio::fs::write(&catalog_path, content).await
            .map_err(|e| AuroraError::new(ErrorCode::StorageUnavailable, format!("Failed to write catalog: {}", e)))?;

        Ok(())
    }

    /// Get catalog statistics
    pub async fn stats(&self) -> CatalogStats {
        let tables = self.tables.read().await;

        let total_columns = tables.values()
            .map(|t| t.columns.len())
            .sum();

        CatalogStats {
            total_tables: tables.len(),
            total_columns,
            catalog_size_bytes: self.storage_path.join("catalog.json")
                .metadata().ok()
                .map(|m| m.len() as usize)
                .unwrap_or(0),
        }
    }
}

/// Catalog statistics
#[derive(Debug, Clone)]
pub struct CatalogStats {
    pub total_tables: usize,
    pub total_columns: usize,
    pub catalog_size_bytes: usize,
}

#[cfg(test)]
mod tests {
    use super::*;
    use tempfile::tempdir;

    #[tokio::test]
    async fn test_create_and_drop_table() {
        let temp_dir = tempdir().unwrap();
        let catalog = TableCatalog::new(temp_dir.path().join("catalog"));

        // Create a test table
        let create_query = CreateTableQuery {
            name: "users".to_string(),
            columns: vec![
                ColumnDefinition {
                    name: "id".to_string(),
                    data_type: DataType::Integer,
                    nullable: false,
                    default: None,
                },
                ColumnDefinition {
                    name: "name".to_string(),
                    data_type: DataType::Text,
                    nullable: false,
                    default: None,
                },
            ],
            constraints: vec![TableConstraint::PrimaryKey(vec!["id".to_string()])],
        };

        // Create table
        catalog.create_table(&create_query).await.unwrap();

        // Verify table exists
        assert!(catalog.table_exists("users").await);
        let metadata = catalog.get_table("users").await.unwrap().unwrap();
        assert_eq!(metadata.name, "users");
        assert_eq!(metadata.columns.len(), 2);

        // Drop table
        let drop_query = DropTableQuery {
            name: "users".to_string(),
            if_exists: false,
        };

        catalog.drop_table(&drop_query).await.unwrap();

        // Verify table is gone
        assert!(!catalog.table_exists("users").await);
    }

    #[tokio::test]
    async fn test_data_validation() {
        let temp_dir = tempdir().unwrap();
        let catalog = TableCatalog::new(temp_dir.path().join("catalog"));

        // Create table
        let create_query = CreateTableQuery {
            name: "test_table".to_string(),
            columns: vec![
                ColumnDefinition {
                    name: "id".to_string(),
                    data_type: DataType::Integer,
                    nullable: false,
                    default: None,
                },
                ColumnDefinition {
                    name: "name".to_string(),
                    data_type: DataType::Text,
                    nullable: false,
                    default: None,
                },
            ],
            constraints: vec![],
        };

        catalog.create_table(&create_query).await.unwrap();

        // Test valid data
        let valid_data = serde_json::json!({
            "id": 1,
            "name": "Alice"
        });
        let valid_map = serde_json::from_value::<HashMap<String, serde_json::Value>>(valid_data).unwrap();
        assert!(catalog.validate_data("test_table", &valid_map).await.is_ok());

        // Test invalid data type
        let invalid_data = serde_json::json!({
            "id": "not_an_integer",
            "name": "Alice"
        });
        let invalid_map = serde_json::from_value::<HashMap<String, serde_json::Value>>(invalid_data).unwrap();
        assert!(catalog.validate_data("test_table", &invalid_map).await.is_err());

        // Test missing required field
        let missing_data = serde_json::json!({
            "id": 1
            // missing name
        });
        let missing_map = serde_json::from_value::<HashMap<String, serde_json::Value>>(missing_data).unwrap();
        assert!(catalog.validate_data("test_table", &missing_map).await.is_err());
    }
}
