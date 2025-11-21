//! Join Operators
//!
//! Nested loop join and hash join implementations with vectorized processing.

use crate::core::*;
use crate::query::parser::ast::*;
use super::traits::*;
use std::collections::HashMap;

/// Nested loop join operator
pub struct NestedLoopJoinOperator {
    left: Box<dyn PhysicalOperator>,
    right: Box<dyn PhysicalOperator>,
    condition: Expression,
    left_tuple: Option<Row>,
    stats: OperatorStats,
}

impl NestedLoopJoinOperator {
    pub fn new(left: Box<dyn PhysicalOperator>, right: Box<dyn PhysicalOperator>, condition: Expression) -> Self {
        Self {
            left,
            right,
            condition,
            left_tuple: None,
            stats: OperatorStats::default(),
        }
    }
}

#[async_trait::async_trait]
impl PhysicalOperator for NestedLoopJoinOperator {
    async fn open(&mut self) -> ExecutionResult<()> {
        self.left.open().await?;
        self.right.open().await?;
        Ok(())
    }

    async fn next(&mut self) -> ExecutionResult<Option<Row>> {
        loop {
            if self.left_tuple.is_none() {
                self.left_tuple = self.left.next().await?;
                if self.left_tuple.is_none() {
                    return Ok(None);
                }
                self.right.close().await?;
                self.right.open().await?;
            }

            if let Some(right_tuple) = self.right.next().await? {
                let left_tuple = self.left_tuple.as_ref().unwrap();
                let joined_row = Row {
                    id: left_tuple.id,
                    data: [left_tuple.data.clone(), right_tuple.data.clone()].concat(),
                };

                self.stats.rows_processed += 1;
                return Ok(Some(joined_row));
            } else {
                self.left_tuple = None;
            }
        }
    }

    async fn close(&mut self) -> ExecutionResult<()> {
        self.left.close().await?;
        self.right.close().await?;
        Ok(())
    }

    fn stats(&self) -> OperatorStats {
        self.stats.clone()
    }
}

/// Hash join operator with vectorized processing
pub struct HashJoinOperator {
    left: Box<dyn PhysicalOperator>,
    right: Box<dyn PhysicalOperator>,
    condition: Expression,
    hash_table: HashMap<Vec<u8>, Vec<Row>>,
    right_iter: Option<Box<dyn Iterator<Item = Row> + Send>>,
    vectorized: bool,
    stats: OperatorStats,
}

impl HashJoinOperator {
    pub fn new(left: Box<dyn PhysicalOperator>, right: Box<dyn PhysicalOperator>, condition: Expression, vectorized: bool) -> Self {
        Self {
            left,
            right,
            condition,
            hash_table: HashMap::new(),
            right_iter: None,
            vectorized,
            stats: OperatorStats::default(),
        }
    }
}

#[async_trait::async_trait]
impl PhysicalOperator for HashJoinOperator {
    async fn open(&mut self) -> ExecutionResult<()> {
        self.left.open().await?;
        self.right.open().await?;

        // Build hash table from left side
        while let Some(row) = self.left.next().await? {
            let key = vec![0u8; 8]; // Placeholder key
            self.hash_table.entry(key).or_insert_with(Vec::new).push(row);
        }

        // Prepare right side iterator
        let mut right_rows = Vec::new();
        while let Some(row) = self.right.next().await? {
            right_rows.push(row);
        }
        self.right_iter = Some(Box::new(right_rows.into_iter()));

        Ok(())
    }

    async fn next(&mut self) -> ExecutionResult<Option<Row>> {
        if let Some(ref mut iter) = self.right_iter {
            while let Some(right_row) = iter.next() {
                let key = vec![0u8; 8]; // Placeholder key

                if let Some(left_rows) = self.hash_table.get(&key) {
                    for left_row in left_rows {
                        let joined_row = Row {
                            id: left_row.id,
                            data: [left_row.data.clone(), right_row.data.clone()].concat(),
                        };

                        self.stats.rows_processed += 1;
                        return Ok(Some(joined_row));
                    }
                }
            }
        }

        Ok(None)
    }

    async fn close(&mut self) -> ExecutionResult<()> {
        self.left.close().await?;
        self.right.close().await?;
        self.hash_table.clear();
        Ok(())
    }

    fn stats(&self) -> OperatorStats {
        self.stats.clone()
    }
}
