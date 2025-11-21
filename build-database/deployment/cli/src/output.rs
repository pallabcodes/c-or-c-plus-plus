//! Output Formatting
//!
//! Handles different output formats (table, JSON, CSV) for CLI commands.

use comfy_table::{Table, Cell, Row};
use serde_json::Value;

#[derive(Debug, Clone)]
pub enum OutputFormat {
    Table,
    Json,
    Csv,
}

pub struct TableFormatter {
    table: Table,
}

impl TableFormatter {
    pub fn new(headers: Vec<&str>) -> Self {
        let mut table = Table::new();
        table.set_header(headers);
        Self { table }
    }

    pub fn add_row(&mut self, cells: Vec<String>) {
        let row: Row = cells.into_iter().map(Cell::new).collect();
        self.table.add_row(row);
    }

    pub fn print(self) {
        println!("{}", self.table);
    }
}

pub struct CsvFormatter {
    headers: Vec<String>,
    rows: Vec<Vec<String>>,
}

impl CsvFormatter {
    pub fn new(headers: Vec<String>) -> Self {
        Self {
            headers,
            rows: Vec::new(),
        }
    }

    pub fn add_row(&mut self, cells: Vec<String>) {
        self.rows.push(cells);
    }

    pub fn print(self) {
        // Print headers
        println!("{}", self.headers.join(","));

        // Print rows
        for row in self.rows {
            let csv_row = row.iter()
                .map(|cell| {
                    if cell.contains(',') || cell.contains('"') || cell.contains('\n') {
                        format!("\"{}\"", cell.replace("\"", "\"\""))
                    } else {
                        cell.clone()
                    }
                })
                .collect::<Vec<_>>()
                .join(",");
            println!("{}", csv_row);
        }
    }
}

pub fn format_query_result(result: &crate::client::QueryResult, format: OutputFormat) {
    match format {
        OutputFormat::Table => {
            if result.data.is_empty() {
                println!("No results");
                return;
            }

            let mut formatter = TableFormatter::new(
                result.columns.iter().map(|s| s.as_str()).collect()
            );

            for row_data in &result.data {
                // Parse row data (assuming pipe-separated for now)
                let cells: Vec<String> = row_data.split('|')
                    .map(|s| s.trim().to_string())
                    .collect();
                formatter.add_row(cells);
            }

            formatter.print();

            println!("\n{} rows returned in {:.2}ms",
                result.row_count,
                result.execution_time_ms
            );
        }
        OutputFormat::Json => {
            let json_result = serde_json::json!({
                "columns": result.columns,
                "data": result.data,
                "row_count": result.row_count,
                "execution_time_ms": result.execution_time_ms
            });
            println!("{}", serde_json::to_string_pretty(&json_result).unwrap());
        }
        OutputFormat::Csv => {
            let mut formatter = CsvFormatter::new(result.columns.clone());

            for row_data in &result.data {
                let cells: Vec<String> = row_data.split('|')
                    .map(|s| s.trim().to_string())
                    .collect();
                formatter.add_row(cells);
            }

            formatter.print();
        }
    }
}

pub fn format_metrics(metrics: &std::collections::HashMap<String, Value>, format: OutputFormat) {
    match format {
        OutputFormat::Table => {
            println!("AuroraDB Metrics");
            println!("================");

            let mut sorted_metrics: Vec<_> = metrics.iter().collect();
            sorted_metrics.sort_by(|a, b| a.0.cmp(b.0));

            for (key, value) in sorted_metrics {
                match value {
                    Value::Number(n) => {
                        if let Some(int_val) = n.as_u64() {
                            println!("{:<30}: {}", key, int_val);
                        } else if let Some(float_val) = n.as_f64() {
                            println!("{:<30}: {:.2}", key, float_val);
                        }
                    }
                    Value::Bool(b) => println!("{:<30}: {}", key, b),
                    Value::String(s) => println!("{:<30}: {}", key, s),
                    _ => println!("{:<30}: {:?}", key, value),
                }
            }
        }
        OutputFormat::Json => {
            println!("{}", serde_json::to_string_pretty(metrics).unwrap());
        }
        OutputFormat::Csv => {
            println!("metric,value");
            for (key, value) in metrics {
                let value_str = match value {
                    Value::String(s) => s.clone(),
                    Value::Number(n) => n.to_string(),
                    Value::Bool(b) => b.to_string(),
                    _ => value.to_string(),
                };
                println!("{},{}", key, value_str);
            }
        }
    }
}

pub fn format_cluster_status(status: &Value, format: OutputFormat) {
    match format {
        OutputFormat::Table => {
            println!("Cluster Status");
            println!("==============");

            if let Some(cluster_name) = status.get("cluster_name").and_then(|v| v.as_str()) {
                println!("Cluster Name: {}", cluster_name);
            }

            if let Some(node_count) = status.get("node_count").and_then(|v| v.as_u64()) {
                println!("Node Count: {}", node_count);
            }

            if let Some(primary) = status.get("primary_node").and_then(|v| v.as_str()) {
                println!("Primary Node: {}", primary);
            }

            if let Some(lag) = status.get("replication_lag_ms").and_then(|v| v.as_u64()) {
                println!("Replication Lag: {}ms", lag);
            }

            if let Some(health) = status.get("health").and_then(|v| v.as_str()) {
                println!("Health: {}", health);
            }
        }
        OutputFormat::Json => {
            println!("{}", serde_json::to_string_pretty(status).unwrap());
        }
        OutputFormat::Csv => {
            println!("metric,value");
            if let Some(cluster_name) = status.get("cluster_name").and_then(|v| v.as_str()) {
                println!("cluster_name,{}", cluster_name);
            }
            if let Some(node_count) = status.get("node_count").and_then(|v| v.as_u64()) {
                println!("node_count,{}", node_count);
            }
            if let Some(primary) = status.get("primary_node").and_then(|v| v.as_str()) {
                println!("primary_node,{}", primary);
            }
            if let Some(lag) = status.get("replication_lag_ms").and_then(|v| v.as_u64()) {
                println!("replication_lag_ms,{}", lag);
            }
            if let Some(health) = status.get("health").and_then(|v| v.as_str()) {
                println!("health,{}", health);
            }
        }
    }
}

pub fn print_success(message: &str) {
    println!("✅ {}", message);
}

pub fn print_error(message: &str) {
    eprintln!("❌ {}", message);
}

pub fn print_warning(message: &str) {
    println!("⚠️  {}", message);
}

pub fn print_info(message: &str) {
    println!("ℹ️  {}", message);
}
