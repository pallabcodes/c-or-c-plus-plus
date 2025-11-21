//! Grafana Dashboard Templates
//!
//! Pre-configured Grafana dashboards for AuroraDB monitoring:
//! - Database overview dashboard
//! - Query performance dashboard
//! - System resources dashboard
//! - Connection pool dashboard
//! - Alerting dashboard

use serde::{Serialize, Deserialize};
use std::collections::HashMap;

/// Grafana dashboard configuration
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct GrafanaDashboard {
    pub dashboard: DashboardConfig,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct DashboardConfig {
    pub id: Option<u64>,
    pub title: String,
    pub description: String,
    pub tags: Vec<String>,
    pub timezone: String,
    pub panels: Vec<Panel>,
    pub time: TimeRange,
    pub refresh: String,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Panel {
    pub id: u64,
    pub title: String,
    pub panel_type: String,
    pub targets: Vec<Target>,
    pub grid_pos: GridPos,
    pub options: HashMap<String, serde_json::Value>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Target {
    pub expr: String,
    pub legend_format: String,
    pub ref_id: String,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct GridPos {
    pub h: u8,
    pub w: u8,
    pub x: u8,
    pub y: u8,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TimeRange {
    pub from: String,
    pub to: String,
}

/// Create AuroraDB overview dashboard
pub fn create_overview_dashboard() -> GrafanaDashboard {
    GrafanaDashboard {
        dashboard: DashboardConfig {
            id: None,
            title: "AuroraDB Overview".to_string(),
            description: "Comprehensive AuroraDB monitoring dashboard".to_string(),
            tags: vec!["auroradb".to_string(), "database".to_string()],
            timezone: "UTC".to_string(),
            refresh: "30s".to_string(),
            time: TimeRange {
                from: "now-1h".to_string(),
                to: "now".to_string(),
            },
            panels: vec![
                // Active Connections
                Panel {
                    id: 1,
                    title: "Active Connections".to_string(),
                    panel_type: "stat".to_string(),
                    targets: vec![Target {
                        expr: "aurora_active_connections".to_string(),
                        legend_format: "Connections".to_string(),
                        ref_id: "A".to_string(),
                    }],
                    grid_pos: GridPos { h: 8, w: 12, x: 0, y: 0 },
                    options: HashMap::new(),
                },

                // Query Rate
                Panel {
                    id: 2,
                    title: "Query Rate (per second)".to_string(),
                    panel_type: "graph".to_string(),
                    targets: vec![Target {
                        expr: "rate(aurora_queries_total[5m])".to_string(),
                        legend_format: "QPS".to_string(),
                        ref_id: "A".to_string(),
                    }],
                    grid_pos: GridPos { h: 8, w: 12, x: 12, y: 0 },
                    options: HashMap::new(),
                },

                // Query Latency
                Panel {
                    id: 3,
                    title: "Query Latency".to_string(),
                    panel_type: "graph".to_string(),
                    targets: vec![Target {
                        expr: "aurora_query_duration_seconds".to_string(),
                        legend_format: "Latency (s)".to_string(),
                        ref_id: "A".to_string(),
                    }],
                    grid_pos: GridPos { h: 8, w: 12, x: 0, y: 8 },
                    options: HashMap::new(),
                },

                // Storage Usage
                Panel {
                    id: 4,
                    title: "Storage Usage".to_string(),
                    panel_type: "bargauge".to_string(),
                    targets: vec![Target {
                        expr: "aurora_storage_used_bytes".to_string(),
                        legend_format: "Used".to_string(),
                        ref_id: "A".to_string(),
                    }],
                    grid_pos: GridPos { h: 8, w: 12, x: 12, y: 8 },
                    options: HashMap::new(),
                },

                // Active Transactions
                Panel {
                    id: 5,
                    title: "Active Transactions".to_string(),
                    panel_type: "stat".to_string(),
                    targets: vec![Target {
                        expr: "aurora_active_transactions".to_string(),
                        legend_format: "Active TX".to_string(),
                        ref_id: "A".to_string(),
                    }],
                    grid_pos: GridPos { h: 8, w: 6, x: 0, y: 16 },
                    options: HashMap::new(),
                },

                // Total Tables
                Panel {
                    id: 6,
                    title: "Total Tables".to_string(),
                    panel_type: "stat".to_string(),
                    targets: vec![Target {
                        expr: "aurora_tables_total".to_string(),
                        legend_format: "Tables".to_string(),
                        ref_id: "A".to_string(),
                    }],
                    grid_pos: GridPos { h: 8, w: 6, x: 6, y: 16 },
                    options: HashMap::new(),
                },

                // Memory Usage
                Panel {
                    id: 7,
                    title: "Memory Usage".to_string(),
                    panel_type: "graph".to_string(),
                    targets: vec![Target {
                        expr: "aurora_memory_used_bytes".to_string(),
                        legend_format: "Memory (bytes)".to_string(),
                        ref_id: "A".to_string(),
                    }],
                    grid_pos: GridPos { h: 8, w: 12, x: 12, y: 16 },
                    options: HashMap::new(),
                },
            ],
        }
    }
}

/// Create query performance dashboard
pub fn create_query_performance_dashboard() -> GrafanaDashboard {
    GrafanaDashboard {
        dashboard: DashboardConfig {
            id: None,
            title: "AuroraDB Query Performance".to_string(),
            description: "Detailed query performance monitoring".to_string(),
            tags: vec!["auroradb".to_string(), "queries".to_string(), "performance".to_string()],
            timezone: "UTC".to_string(),
            refresh: "30s".to_string(),
            time: TimeRange {
                from: "now-1h".to_string(),
                to: "now".to_string(),
            },
            panels: vec![
                // Total Queries
                Panel {
                    id: 1,
                    title: "Total Queries Executed".to_string(),
                    panel_type: "stat".to_string(),
                    targets: vec![Target {
                        expr: "aurora_queries_total".to_string(),
                        legend_format: "Queries".to_string(),
                        ref_id: "A".to_string(),
                    }],
                    grid_pos: GridPos { h: 8, w: 8, x: 0, y: 0 },
                    options: HashMap::new(),
                },

                // Query Rate Over Time
                Panel {
                    id: 2,
                    title: "Query Rate Timeline".to_string(),
                    panel_type: "graph".to_string(),
                    targets: vec![Target {
                        expr: "rate(aurora_queries_total[5m])".to_string(),
                        legend_format: "QPS".to_string(),
                        ref_id: "A".to_string(),
                    }],
                    grid_pos: GridPos { h: 8, w: 16, x: 8, y: 0 },
                    options: HashMap::new(),
                },

                // Average Query Latency
                Panel {
                    id: 3,
                    title: "Average Query Latency".to_string(),
                    panel_type: "graph".to_string(),
                    targets: vec![Target {
                        expr: "aurora_query_duration_seconds".to_string(),
                        legend_format: "Latency (s)".to_string(),
                        ref_id: "A".to_string(),
                    }],
                    grid_pos: GridPos { h: 8, w: 12, x: 0, y: 8 },
                    options: HashMap::new(),
                },

                // Query Latency Distribution
                Panel {
                    id: 4,
                    title: "Query Latency Percentiles".to_string(),
                    panel_type: "heatmap".to_string(),
                    targets: vec![Target {
                        expr: "histogram_quantile(0.95, rate(aurora_query_duration_seconds_bucket[5m]))".to_string(),
                        legend_format: "95th percentile".to_string(),
                        ref_id: "A".to_string(),
                    }],
                    grid_pos: GridPos { h: 8, w: 12, x: 12, y: 8 },
                    options: HashMap::new(),
                },

                // Slow Queries Alert
                Panel {
                    id: 5,
                    title: "Slow Queries (>1s)".to_string(),
                    panel_type: "table".to_string(),
                    targets: vec![Target {
                        expr: "increase(aurora_query_duration_seconds{quantile=\"0.95\"}[5m])".to_string(),
                        legend_format: "Slow queries".to_string(),
                        ref_id: "A".to_string(),
                    }],
                    grid_pos: GridPos { h: 8, w: 12, x: 0, y: 16 },
                    options: HashMap::new(),
                },

                // Query Type Distribution
                Panel {
                    id: 6,
                    title: "Query Types".to_string(),
                    panel_type: "piechart".to_string(),
                    targets: vec![Target {
                        expr: "sum(rate(aurora_queries_total[5m])) by (query_type)".to_string(),
                        legend_format: "{{query_type}}".to_string(),
                        ref_id: "A".to_string(),
                    }],
                    grid_pos: GridPos { h: 8, w: 12, x: 12, y: 16 },
                    options: HashMap::new(),
                },
            ],
        }
    }
}

/// Create system resources dashboard
pub fn create_system_resources_dashboard() -> GrafanaDashboard {
    GrafanaDashboard {
        dashboard: DashboardConfig {
            id: None,
            title: "AuroraDB System Resources".to_string(),
            description: "System resource monitoring for AuroraDB".to_string(),
            tags: vec!["auroradb".to_string(), "system".to_string(), "resources".to_string()],
            timezone: "UTC".to_string(),
            refresh: "30s".to_string(),
            time: TimeRange {
                from: "now-1h".to_string(),
                to: "now".to_string(),
            },
            panels: vec![
                // CPU Usage
                Panel {
                    id: 1,
                    title: "CPU Usage".to_string(),
                    panel_type: "graph".to_string(),
                    targets: vec![Target {
                        expr: "100 - (avg by (instance) (rate(node_cpu_seconds_total{mode=\"idle\"}[5m])) * 100)".to_string(),
                        legend_format: "CPU %".to_string(),
                        ref_id: "A".to_string(),
                    }],
                    grid_pos: GridPos { h: 8, w: 12, x: 0, y: 0 },
                    options: HashMap::new(),
                },

                // Memory Usage
                Panel {
                    id: 2,
                    title: "Memory Usage".to_string(),
                    panel_type: "graph".to_string(),
                    targets: vec![Target {
                        expr: "aurora_memory_used_bytes".to_string(),
                        legend_format: "AuroraDB Memory".to_string(),
                        ref_id: "A".to_string(),
                    }],
                    grid_pos: GridPos { h: 8, w: 12, x: 12, y: 0 },
                    options: HashMap::new(),
                },

                // Disk I/O
                Panel {
                    id: 3,
                    title: "Disk I/O Operations".to_string(),
                    panel_type: "graph".to_string(),
                    targets: vec![Target {
                        expr: "rate(node_disk_reads_completed_total[5m])".to_string(),
                        legend_format: "Read IOPS".to_string(),
                        ref_id: "A".to_string(),
                    }, Target {
                        expr: "rate(node_disk_writes_completed_total[5m])".to_string(),
                        legend_format: "Write IOPS".to_string(),
                        ref_id: "B".to_string(),
                    }],
                    grid_pos: GridPos { h: 8, w: 12, x: 0, y: 8 },
                    options: HashMap::new(),
                },

                // Network Traffic
                Panel {
                    id: 4,
                    title: "Network Traffic".to_string(),
                    panel_type: "graph".to_string(),
                    targets: vec![Target {
                        expr: "rate(node_network_receive_bytes_total[5m])".to_string(),
                        legend_format: "RX bytes/s".to_string(),
                        ref_id: "A".to_string(),
                    }, Target {
                        expr: "rate(node_network_transmit_bytes_total[5m])".to_string(),
                        legend_format: "TX bytes/s".to_string(),
                        ref_id: "B".to_string(),
                    }],
                    grid_pos: GridPos { h: 8, w: 12, x: 12, y: 8 },
                    options: HashMap::new(),
                },

                // Database Uptime
                Panel {
                    id: 5,
                    title: "Database Uptime".to_string(),
                    panel_type: "stat".to_string(),
                    targets: vec![Target {
                        expr: "aurora_uptime_seconds".to_string(),
                        legend_format: "Uptime (s)".to_string(),
                        ref_id: "A".to_string(),
                    }],
                    grid_pos: GridPos { h: 8, w: 8, x: 0, y: 16 },
                    options: HashMap::new(),
                },

                // Error Rate
                Panel {
                    id: 6,
                    title: "Error Rate".to_string(),
                    panel_type: "graph".to_string(),
                    targets: vec![Target {
                        expr: "rate(aurora_errors_total[5m])".to_string(),
                        legend_format: "Errors/s".to_string(),
                        ref_id: "A".to_string(),
                    }],
                    grid_pos: GridPos { h: 8, w: 16, x: 8, y: 16 },
                    options: HashMap::new(),
                },
            ],
        }
    }
}

/// Create alerting dashboard
pub fn create_alerting_dashboard() -> GrafanaDashboard {
    GrafanaDashboard {
        dashboard: DashboardConfig {
            id: None,
            title: "AuroraDB Alerts & Incidents".to_string(),
            description: "Alerting and incident monitoring for AuroraDB".to_string(),
            tags: vec!["auroradb".to_string(), "alerts".to_string(), "monitoring".to_string()],
            timezone: "UTC".to_string(),
            refresh: "30s".to_string(),
            time: TimeRange {
                from: "now-24h".to_string(),
                to: "now".to_string(),
            },
            panels: vec![
                // Connection Pool Alert
                Panel {
                    id: 1,
                    title: "Connection Pool Usage Alert".to_string(),
                    panel_type: "stat".to_string(),
                    targets: vec![Target {
                        expr: "(aurora_connection_pool_size - aurora_active_connections) / aurora_connection_pool_size * 100 < 20".to_string(),
                        legend_format: "Pool Usage %".to_string(),
                        ref_id: "A".to_string(),
                    }],
                    grid_pos: GridPos { h: 8, w: 8, x: 0, y: 0 },
                    options: HashMap::new(),
                },

                // High Query Latency Alert
                Panel {
                    id: 2,
                    title: "High Query Latency Alert".to_string(),
                    panel_type: "stat".to_string(),
                    targets: vec![Target {
                        expr: "aurora_query_duration_seconds > 1".to_string(),
                        legend_format: "Slow Queries".to_string(),
                        ref_id: "A".to_string(),
                    }],
                    grid_pos: GridPos { h: 8, w: 8, x: 8, y: 0 },
                    options: HashMap::new(),
                },

                // Storage Space Alert
                Panel {
                    id: 3,
                    title: "Storage Space Alert".to_string(),
                    panel_type: "stat".to_string(),
                    targets: vec![Target {
                        expr: "aurora_storage_used_bytes / 1073741824 > 100".to_string(), // > 100GB
                        legend_format: "Storage (GB)".to_string(),
                        ref_id: "A".to_string(),
                    }],
                    grid_pos: GridPos { h: 8, w: 8, x: 16, y: 0 },
                    options: HashMap::new(),
                },

                // Transaction Lock Alert
                Panel {
                    id: 4,
                    title: "Transaction Lock Alert".to_string(),
                    panel_type: "stat".to_string(),
                    targets: vec![Target {
                        expr: "aurora_active_transactions > 10".to_string(),
                        legend_format: "Long TX".to_string(),
                        ref_id: "A".to_string(),
                    }],
                    grid_pos: GridPos { h: 8, w: 12, x: 0, y: 8 },
                    options: HashMap::new(),
                },

                // Alert Timeline
                Panel {
                    id: 5,
                    title: "Alert Timeline".to_string(),
                    panel_type: "graph".to_string(),
                    targets: vec![Target {
                        expr: "ALERTS{alertname=~\"AuroraDB.*\"}".to_string(),
                        legend_format: "{{alertname}}".to_string(),
                        ref_id: "A".to_string(),
                    }],
                    grid_pos: GridPos { h: 8, w: 12, x: 12, y: 8 },
                    options: HashMap::new(),
                },

                // Recent Alerts Table
                Panel {
                    id: 6,
                    title: "Recent Alerts".to_string(),
                    panel_type: "table".to_string(),
                    targets: vec![Target {
                        expr: "ALERTS{alertname=~\"AuroraDB.*\"}[1h]".to_string(),
                        legend_format: "{{alertname}}".to_string(),
                        ref_id: "A".to_string(),
                    }],
                    grid_pos: GridPos { h: 8, w: 24, x: 0, y: 16 },
                    options: HashMap::new(),
                },
            ],
        }
    }
}

/// Export all dashboards as JSON
pub fn export_dashboards() -> Vec<(String, GrafanaDashboard)> {
    vec![
        ("auroradb-overview.json".to_string(), create_overview_dashboard()),
        ("auroradb-query-performance.json".to_string(), create_query_performance_dashboard()),
        ("auroradb-system-resources.json".to_string(), create_system_resources_dashboard()),
        ("auroradb-alerts.json".to_string(), create_alerting_dashboard()),
    ]
}
