use crate::errors::{AuroraError, AuroraResult};
use crate::types::{ColumnId, TableId};

/// Column definition in a table schema
#[derive(Debug, Clone)]
pub struct Column {
    pub id: ColumnId,
    pub name: String,
    pub data_type: crate::data::DataType,
    pub nullable: bool,
    pub default_value: Option<Vec<u8>>,
}

/// Table schema definition
#[derive(Debug, Clone)]
pub struct TableSchema {
    pub id: TableId,
    pub name: String,
    pub columns: Vec<Column>,
    pub primary_key: Vec<ColumnId>,
}

impl TableSchema {
    /// Get column by ID
    pub fn get_column(&self, column_id: ColumnId) -> Option<&Column> {
        self.columns.iter().find(|col| col.id == column_id)
    }

    /// Get column by name
    pub fn get_column_by_name(&self, name: &str) -> Option<&Column> {
        self.columns.iter().find(|col| col.name == name)
    }

    /// Validate that primary key columns exist
    pub fn validate_primary_key(&self) -> AuroraResult<()> {
        for &pk_col in &self.primary_key {
            if self.get_column(pk_col).is_none() {
                return Err(AuroraError::InvalidArgument(
                    format!("Primary key column {:?} not found in table {}", pk_col, self.name)
                ));
            }
        }
        Ok(())
    }
}
