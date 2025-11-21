//! Sort and Ordering Operators
//!
//! Sorting operators with vectorized processing support.

use crate::core::*;
use crate::query::parser::ast::*;
use super::traits::*;

/// Sort operator with vectorized sorting
pub struct SortOperator {
    input: Box<dyn PhysicalOperator>,
    order_by: Vec<OrderByItem>,
    sorted_rows: Option<Vec<Row>>,
    current_index: usize,
    vectorized: bool,
    stats: OperatorStats,
}

impl SortOperator {
    pub fn new(input: Box<dyn PhysicalOperator>, order_by: Vec<OrderByItem>, vectorized: bool) -> Self {
        Self {
            input,
            order_by,
            sorted_rows: None,
            current_index: 0,
            vectorized,
            stats: OperatorStats::default(),
        }
    }
}

#[async_trait::async_trait]
impl PhysicalOperator for SortOperator {
    async fn open(&mut self) -> ExecutionResult<()> {
        self.input.open().await?;

        let mut rows = Vec::new();
        while let Some(row) = self.input.next().await? {
            rows.push(row);
        }

        // TODO: Implement sorting based on order_by
        self.sorted_rows = Some(rows);
        self.current_index = 0;

        Ok(())
    }

    async fn next(&mut self) -> ExecutionResult<Option<Row>> {
        if let Some(ref rows) = self.sorted_rows {
            if self.current_index < rows.len() {
                let row = rows[self.current_index].clone();
                self.current_index += 1;
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
        self.sorted_rows = None;
        Ok(())
    }

    fn stats(&self) -> OperatorStats {
        self.stats.clone()
    }
}
