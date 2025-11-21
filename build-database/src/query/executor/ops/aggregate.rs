//! Aggregation Operators
//!
//! Group by and aggregation operators with vectorized processing.

use crate::core::*;
use crate::query::parser::ast::*;
use super::traits::*;
use std::collections::HashMap;

/// Group by aggregation operator
pub struct GroupByOperator {
    input: Box<dyn PhysicalOperator>,
    group_by: Vec<Expression>,
    aggregates: Vec<AggregateExpr>,
    groups: Option<HashMap<Vec<u8>, Vec<Row>>>,
    result_iter: Option<Box<dyn Iterator<Item = Row> + Send>>,
    vectorized: bool,
    stats: OperatorStats,
}

impl GroupByOperator {
    pub fn new(input: Box<dyn PhysicalOperator>, group_by: Vec<Expression>, aggregates: Vec<AggregateExpr>, vectorized: bool) -> Self {
        Self {
            input,
            group_by,
            aggregates,
            groups: None,
            result_iter: None,
            vectorized,
            stats: OperatorStats::default(),
        }
    }
}

#[async_trait::async_trait]
impl PhysicalOperator for GroupByOperator {
    async fn open(&mut self) -> ExecutionResult<()> {
        self.input.open().await?;

        let mut groups: HashMap<Vec<u8>, Vec<Row>> = HashMap::new();

        while let Some(row) = self.input.next().await? {
            let key = vec![0u8; 8]; // Placeholder key
            groups.entry(key).or_insert_with(Vec::new).push(row);
        }

        let results: Vec<Row> = groups.into_iter().map(|(_key, group_rows)| {
            group_rows.into_iter().next().unwrap_or_else(|| Row {
                id: RowId(0),
                data: vec![],
            })
        }).collect();

        self.result_iter = Some(Box::new(results.into_iter()));
        Ok(())
    }

    async fn next(&mut self) -> ExecutionResult<Option<Row>> {
        if let Some(ref mut iter) = self.result_iter {
            if let Some(row) = iter.next() {
                self.stats.rows_processed += 1;
                Ok(Some(row))
            } else {
                Ok(None)
            }
        } else {
            Ok(None)
        }
    }

    async fn close(&mut self) -> ExecutionResult<()> {
        self.input.close().await?;
        Ok(())
    }

    fn stats(&self) -> OperatorStats {
        self.stats.clone()
    }
}

/// Limit operator
pub struct LimitOperator {
    input: Box<dyn PhysicalOperator>,
    limit: usize,
    offset: usize,
    returned: usize,
    stats: OperatorStats,
}

impl LimitOperator {
    pub fn new(input: Box<dyn PhysicalOperator>, limit: usize, offset: usize) -> Self {
        Self {
            input,
            limit,
            offset,
            returned: 0,
            stats: OperatorStats::default(),
        }
    }
}

#[async_trait::async_trait]
impl PhysicalOperator for LimitOperator {
    async fn open(&mut self) -> ExecutionResult<()> {
        self.input.open().await?;
        Ok(())
    }

    async fn next(&mut self) -> ExecutionResult<Option<Row>> {
        while self.returned < self.offset {
            if self.input.next().await?.is_none() {
                return Ok(None);
            }
            self.returned += 1;
        }

        if self.returned < self.offset + self.limit {
            if let Some(row) = self.input.next().await? {
                self.returned += 1;
                self.stats.rows_processed += 1;
                Ok(Some(row))
            } else {
                Ok(None)
            }
        } else {
            Ok(None)
        }
    }

    async fn close(&mut self) -> ExecutionResult<()> {
        self.input.close().await?;
        Ok(())
    }

    fn stats(&self) -> OperatorStats {
        self.stats.clone()
    }
}
