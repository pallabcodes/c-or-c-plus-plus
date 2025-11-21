//! AuroraDB HTTP Client
//!
//! HTTP client for communicating with AuroraDB's REST API.
//! Used by the CLI tool for database administration.

use reqwest::{Client, Response};
use serde_json::{Value, json};
use std::collections::HashMap;

pub struct AuroraClient {
    client: Client,
    base_url: String,
    auth_token: Option<String>,
}

#[derive(Debug)]
pub struct QueryResult {
    pub data: Vec<String>,
    pub row_count: usize,
    pub execution_time_ms: f64,
    pub columns: Vec<String>,
}

#[derive(Debug)]
pub struct TableSchema {
    pub columns: Vec<ColumnInfo>,
    pub primary_key: Vec<String>,
}

#[derive(Debug)]
pub struct ColumnInfo {
    pub name: String,
    pub data_type: String,
    pub nullable: bool,
}

impl AuroraClient {
    pub fn new(host: &str, port: &str, user: &str, password: &str, database: &str) -> Result<Self, Box<dyn std::error::Error>> {
        let base_url = format!("http://{}:{}", host, port);
        let client = Client::new();

        let mut aurora_client = Self {
            client,
            base_url,
            auth_token: None,
        };

        // Authenticate
        aurora_client.authenticate(user, password, database)?;

        Ok(aurora_client)
    }

    fn authenticate(&mut self, user: &str, password: &str, database: &str) -> Result<(), Box<dyn std::error::Error>> {
        let auth_data = json!({
            "username": user,
            "password": password,
            "database": database
        });

        let response = self.client
            .post(&format!("{}/auth/login", self.base_url))
            .json(&auth_data)
            .send()?;

        if !response.status().is_success() {
            return Err(format!("Authentication failed: {}", response.status()).into());
        }

        let auth_response: Value = response.json()?;
        if let Some(token) = auth_response.get("token").and_then(|t| t.as_str()) {
            self.auth_token = Some(token.to_string());
        }

        Ok(())
    }

    fn make_request(&self, method: &str, endpoint: &str, body: Option<Value>) -> Result<Response, Box<dyn std::error::Error>> {
        let url = format!("{}{}", self.base_url, endpoint);

        let mut request = match method {
            "GET" => self.client.get(&url),
            "POST" => self.client.post(&url),
            "PUT" => self.client.put(&url),
            "DELETE" => self.client.delete(&url),
            _ => return Err("Invalid HTTP method".into()),
        };

        // Add authentication header
        if let Some(token) = &self.auth_token {
            request = request.header("Authorization", format!("Bearer {}", token));
        }

        // Add JSON body if provided
        if let Some(body) = body {
            request = request.json(&body);
        }

        let response = request.send()?;
        Ok(response)
    }

    pub fn execute_query(&self, sql: &str) -> Result<QueryResult, Box<dyn std::error::Error>> {
        let query_data = json!({
            "query": sql
        });

        let response = self.make_request("POST", "/api/query", Some(query_data))?;

        if !response.status().is_success() {
            let error: Value = response.json()?;
            return Err(format!("Query failed: {}", error.get("error").unwrap_or(&Value::String("Unknown error".to_string()))).into());
        }

        let result: Value = response.json()?;

        let data = result.get("data")
            .and_then(|d| d.as_array())
            .unwrap_or(&vec![])
            .iter()
            .filter_map(|row| row.as_str().map(|s| s.to_string()))
            .collect();

        let row_count = result.get("row_count").and_then(|rc| rc.as_u64()).unwrap_or(0) as usize;
        let execution_time_ms = result.get("execution_time_ms").and_then(|et| et.as_f64()).unwrap_or(0.0);

        let columns = result.get("columns")
            .and_then(|c| c.as_array())
            .unwrap_or(&vec![])
            .iter()
            .filter_map(|col| col.as_str().map(|s| s.to_string()))
            .collect();

        Ok(QueryResult {
            data,
            row_count,
            execution_time_ms,
            columns,
        })
    }

    pub fn get_status(&self) -> Result<Value, Box<dyn std::error::Error>> {
        let response = self.make_request("GET", "/api/status", None)?;

        if !response.status().is_success() {
            return Err(format!("Status request failed: {}", response.status()).into());
        }

        let status: Value = response.json()?;
        Ok(status)
    }

    pub fn list_tables(&self) -> Result<Vec<String>, Box<dyn std::error::Error>> {
        let response = self.make_request("GET", "/api/tables", None)?;

        if !response.status().is_success() {
            return Err(format!("List tables failed: {}", response.status()).into());
        }

        let tables: Value = response.json()?;
        let table_list = tables.get("tables")
            .and_then(|t| t.as_array())
            .unwrap_or(&vec![])
            .iter()
            .filter_map(|table| table.as_str().map(|s| s.to_string()))
            .collect();

        Ok(table_list)
    }

    pub fn get_table_schema(&self, table: &str) -> Result<TableSchema, Box<dyn std::error::Error>> {
        let response = self.make_request("GET", &format!("/api/tables/{}/schema", table), None)?;

        if !response.status().is_success() {
            return Err(format!("Get schema failed: {}", response.status()).into());
        }

        let schema_data: Value = response.json()?;

        let columns = schema_data.get("columns")
            .and_then(|c| c.as_array())
            .unwrap_or(&vec![])
            .iter()
            .filter_map(|col| {
                Some(ColumnInfo {
                    name: col.get("name")?.as_str()?.to_string(),
                    data_type: col.get("type")?.as_str()?.to_string(),
                    nullable: col.get("nullable")?.as_bool().unwrap_or(true),
                })
            })
            .collect();

        let primary_key = schema_data.get("primary_key")
            .and_then(|pk| pk.as_array())
            .unwrap_or(&vec![])
            .iter()
            .filter_map(|key| key.as_str().map(|s| s.to_string()))
            .collect();

        Ok(TableSchema {
            columns,
            primary_key,
        })
    }

    pub fn get_metrics(&self) -> Result<HashMap<String, Value>, Box<dyn std::error::Error>> {
        let response = self.make_request("GET", "/api/metrics", None)?;

        if !response.status().is_success() {
            return Err(format!("Metrics request failed: {}", response.status()).into());
        }

        let metrics: Value = response.json()?;
        let metrics_map = metrics.get("metrics")
            .and_then(|m| m.as_object())
            .map(|obj| obj.clone())
            .unwrap_or_default();

        Ok(metrics_map)
    }

    pub fn create_backup(&self, output_path: &str) -> Result<(), Box<dyn std::error::Error>> {
        let backup_data = json!({
            "output_path": output_path
        });

        let response = self.make_request("POST", "/api/backup", Some(backup_data))?;

        if !response.status().is_success() {
            let error: Value = response.json()?;
            return Err(format!("Backup failed: {}", error.get("error").unwrap_or(&Value::String("Unknown error".to_string()))).into());
        }

        Ok(())
    }

    pub fn restore_backup(&self, input_path: &str) -> Result<(), Box<dyn std::error::Error>> {
        let restore_data = json!({
            "input_path": input_path
        });

        let response = self.make_request("POST", "/api/restore", Some(restore_data))?;

        if !response.status().is_success() {
            let error: Value = response.json()?;
            return Err(format!("Restore failed: {}", error.get("error").unwrap_or(&Value::String("Unknown error".to_string()))).into());
        }

        Ok(())
    }

    pub fn list_users(&self) -> Result<Vec<String>, Box<dyn std::error::Error>> {
        let response = self.make_request("GET", "/api/users", None)?;

        if !response.status().is_success() {
            return Err(format!("List users failed: {}", response.status()).into());
        }

        let users: Value = response.json()?;
        let user_list = users.get("users")
            .and_then(|u| u.as_array())
            .unwrap_or(&vec![])
            .iter()
            .filter_map(|user| user.as_str().map(|s| s.to_string()))
            .collect();

        Ok(user_list)
    }

    pub fn create_user(&self, username: &str, password: &str) -> Result<(), Box<dyn std::error::Error>> {
        let user_data = json!({
            "username": username,
            "password": password
        });

        let response = self.make_request("POST", "/api/users", Some(user_data))?;

        if !response.status().is_success() {
            let error: Value = response.json()?;
            return Err(format!("Create user failed: {}", error.get("error").unwrap_or(&Value::String("Unknown error".to_string()))).into());
        }

        Ok(())
    }

    pub fn drop_user(&self, username: &str) -> Result<(), Box<dyn std::error::Error>> {
        let response = self.make_request("DELETE", &format!("/api/users/{}", username), None)?;

        if !response.status().is_success() {
            let error: Value = response.json()?;
            return Err(format!("Drop user failed: {}", error.get("error").unwrap_or(&Value::String("Unknown error".to_string()))).into());
        }

        Ok(())
    }

    pub fn get_cluster_status(&self) -> Result<Value, Box<dyn std::error::Error>> {
        let response = self.make_request("GET", "/api/cluster/status", None)?;

        if !response.status().is_success() {
            return Err(format!("Cluster status failed: {}", response.status()).into());
        }

        let status: Value = response.json()?;
        Ok(status)
    }

    pub fn list_cluster_nodes(&self) -> Result<Vec<Value>, Box<dyn std::error::Error>> {
        let response = self.make_request("GET", "/api/cluster/nodes", None)?;

        if !response.status().is_success() {
            return Err(format!("List nodes failed: {}", response.status()).into());
        }

        let nodes: Value = response.json()?;
        let node_list = nodes.get("nodes")
            .and_then(|n| n.as_array())
            .cloned()
            .unwrap_or(vec![]);

        Ok(node_list)
    }

    pub fn join_cluster(&self, node_address: &str) -> Result<(), Box<dyn std::error::Error>> {
        let join_data = json!({
            "node_address": node_address
        });

        let response = self.make_request("POST", "/api/cluster/join", Some(join_data))?;

        if !response.status().is_success() {
            let error: Value = response.json()?;
            return Err(format!("Join cluster failed: {}", error.get("error").unwrap_or(&Value::String("Unknown error".to_string()))).into());
        }

        Ok(())
    }

    pub fn get_jit_status(&self) -> Result<Value, Box<dyn std::error::Error>> {
        let response = self.make_request("GET", "/api/jit/status", None)?;

        if !response.status().is_success() {
            return Err(format!("JIT status failed: {}", response.status()).into());
        }

        let status: Value = response.json()?;
        Ok(status)
    }

    pub fn get_jit_cache_stats(&self) -> Result<Value, Box<dyn std::error::Error>> {
        let response = self.make_request("GET", "/api/jit/cache/stats", None)?;

        if !response.status().is_success() {
            return Err(format!("JIT cache stats failed: {}", response.status()).into());
        }

        let stats: Value = response.json()?;
        Ok(stats)
    }

    pub fn clear_jit_cache(&self) -> Result<(), Box<dyn std::error::Error>> {
        let response = self.make_request("POST", "/api/jit/cache/clear", None)?;

        if !response.status().is_success() {
            let error: Value = response.json()?;
            return Err(format!("Clear JIT cache failed: {}", error.get("error").unwrap_or(&Value::String("Unknown error".to_string()))).into());
        }

        Ok(())
    }

    pub fn run_vacuum(&self) -> Result<(), Box<dyn std::error::Error>> {
        let response = self.make_request("POST", "/api/maintenance/vacuum", None)?;

        if !response.status().is_success() {
            let error: Value = response.json()?;
            return Err(format!("Vacuum failed: {}", error.get("error").unwrap_or(&Value::String("Unknown error".to_string()))).into());
        }

        Ok(())
    }

    pub fn run_analyze(&self) -> Result<(), Box<dyn std::error::Error>> {
        let response = self.make_request("POST", "/api/maintenance/analyze", None)?;

        if !response.status().is_success() {
            let error: Value = response.json()?;
            return Err(format!("Analyze failed: {}", error.get("error").unwrap_or(&Value::String("Unknown error".to_string()))).into());
        }

        Ok(())
    }

    pub fn reindex_table(&self, table: &str) -> Result<(), Box<dyn std::error::Error>> {
        let response = self.make_request("POST", &format!("/api/maintenance/reindex/{}", table), None)?;

        if !response.status().is_success() {
            let error: Value = response.json()?;
            return Err(format!("Reindex table failed: {}", error.get("error").unwrap_or(&Value::String("Unknown error".to_string()))).into());
        }

        Ok(())
    }

    pub fn reindex_all_tables(&self) -> Result<(), Box<dyn std::error::Error>> {
        let response = self.make_request("POST", "/api/maintenance/reindex", None)?;

        if !response.status().is_success() {
            let error: Value = response.json()?;
            return Err(format!("Reindex all failed: {}", error.get("error").unwrap_or(&Value::String("Unknown error".to_string()))).into());
        }

        Ok(())
    }
}
