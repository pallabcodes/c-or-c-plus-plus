//! Scan Operators
//!
//! Table scan and index scan operators for data access.

use crate::core::*;
use crate::storage::engine::*;
use crate::query::parser::ast::*;
use super::traits::*;

/// Sequential scan operator
pub struct SeqScanOperator<'a> {
    table: String,
    filter: Option<Expression>,
    storage: &'a dyn StorageEngine,
    iterator: Option<Box<dyn Iterator<Item = (Vec<u8>, Vec<u8>)> + Send>>,
    stats: OperatorStats,
}

impl<'a> SeqScanOperator<'a> {
    pub fn new(table: String, filter: Option<Expression>, storage: &'a dyn StorageEngine) -> Self {
        Self {
            table,
            filter,
            storage,
            iterator: None,
            stats: OperatorStats::default(),
        }
    }
}

#[async_trait::async_trait]
impl<'a> PhysicalOperator for SeqScanOperator<'a> {
    async fn open(&mut self) -> ExecutionResult<()> {
        // Create a simple range iterator
        self.iterator = Some(Box::new(std::iter::empty()));
        Ok(())
    }

    async fn next(&mut self) -> ExecutionResult<Option<Row>> {
        if let Some(ref mut iter) = self.iterator {
            if let Some((key, value)) = iter.next() {
                self.stats.rows_processed += 1;
                let row = Row {
                    id: RowId(u64::from_be_bytes(key[..8].try_into().unwrap_or([0; 8]))),
                    data: vec![Some(value)],
                };
                Ok(Some(row))
            } else {
                Ok(None)
            }
        } else {
            Ok(None)
        }
    }

    async fn close(&mut self) -> ExecutionResult<()> {
        self.iterator = None;
        Ok(())
    }

    fn stats(&self) -> OperatorStats {
        self.stats.clone()
    }
}

/// Index scan operator
pub struct IndexScanOperator<'a> {
    table: String,
    index: String,
    filter: Option<Expression>,
    storage: &'a dyn StorageEngine,
    iterator: Option<Box<dyn Iterator<Item = (Vec<u8>, Vec<u8>)> + Send>>,
    stats: OperatorStats,
}

impl<'a> IndexScanOperator<'a> {
    pub fn new(table: String, index: String, filter: Option<Expression>, storage: &'a dyn StorageEngine) -> Self {
        Self {
            table,
            index,
            filter,
            storage,
            iterator: None,
            stats: OperatorStats::default(),
        }
    }
}

#[async_trait::async_trait]
impl<'a> PhysicalOperator for IndexScanOperator<'a> {
    async fn open(&mut self) -> ExecutionResult<()> {
        self.iterator = Some(Box::new(std::iter::empty()));
        Ok(())
    }

    async fn next(&mut self) -> ExecutionResult<Option<Row>> {
        if let Some(ref mut iter) = self.iterator {
            if let Some((key, value)) = iter.next() {
                self.stats.rows_processed += 1;
                let row = Row {
                    id: RowId(u64::from_be_bytes(key[..8].try_into().unwrap_or([0; 8]))),
                    data: vec![Some(value)],
                };
                Ok(Some(row))
            } else {
                Ok(None)
            }
        } else {
            Ok(None)
        }
    }

    async fn close(&mut self) -> ExecutionResult<()> {
        self.iterator = None;
        Ok(())
    }

    fn stats(&self) -> OperatorStats {
        self.stats.clone()
    }
}

/// Vector search operator
pub struct VectorSearchOperator<'a> {
    vector_expr: Expression,
    distance_metric: DistanceMetric,
    k: usize,
    filter: Option<Box<LogicalPlan>>,
    storage: &'a dyn StorageEngine,
    results: Vec<Row>,
    current_index: usize,
    stats: OperatorStats,
}

impl<'a> VectorSearchOperator<'a> {
    pub fn new(vector_expr: Expression, distance_metric: DistanceMetric, k: usize, filter: Option<Box<LogicalPlan>>, storage: &'a dyn StorageEngine) -> Self {
        Self {
            vector_expr,
            distance_metric,
            k,
            filter,
            storage,
            results: Vec::new(),
            current_index: 0,
            stats: OperatorStats::default(),
        }
    }
}

#[async_trait::async_trait]
impl<'a> PhysicalOperator for VectorSearchOperator<'a> {
    async fn open(&mut self) -> ExecutionResult<()> {
        self.results = Vec::new();
        Ok(())
    }

    async fn next(&mut self) -> ExecutionResult<Option<Row>> {
        if self.current_index < self.results.len() {
            let row = self.results[self.current_index].clone();
            self.current_index += 1;
            self.stats.rows_processed += 1;
            Ok(Some(row))
        } else {
            Ok(None)
        }
    }

    async fn close(&mut self) -> ExecutionResult<()> {
        self.results.clear();
        self.current_index = 0;
        Ok(())
    }

    fn stats(&self) -> OperatorStats {
        self.stats.clone()
    }
}
