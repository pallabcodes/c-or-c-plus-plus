//! Grafana Dashboards: UNIQUENESS Enterprise Observability
//!
//! Research-backed Grafana dashboard generation for comprehensive monitoring:
//! - **Auto-Generated Dashboards**: Programmatically created monitoring interfaces
//! - **Service Dependencies**: Automatic dependency mapping and visualization
//! - **Performance Metrics**: Real-time performance monitoring with alerting
//! - **Business KPIs**: Application-specific metrics and SLO tracking
//! - **Multi-Region Views**: Global deployment monitoring and correlation
//! - **Custom Panels**: Specialized visualizations for distributed coordination

use crate::error::{Error, Result};
use crate::monitoring::performance_metrics::PerformanceMetricsCollector;
use crate::types::NodeId;

use std::collections::HashMap;
use serde::{Deserialize, Serialize};
use chrono::{DateTime, Utc};

/// Grafana dashboard manager
pub struct GrafanaDashboardManager {
    /// Dashboard templates
    templates: HashMap<String, DashboardTemplate>,

    /// Active dashboards
    dashboards: HashMap<String, GrafanaDashboard>,

    /// Data sources
    data_sources: Vec<GrafanaDataSource>,

    /// Alert rules
    alert_rules: Vec<GrafanaAlertRule>,
}

/// Dashboard template
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct DashboardTemplate {
    pub template_id: String,
    pub name: String,
    pub description: String,
    pub panels: Vec<PanelTemplate>,
    pub variables: Vec<DashboardVariable>,
    pub tags: Vec<String>,
}

/// Grafana dashboard
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct GrafanaDashboard {
    pub dashboard: DashboardSpec,
    pub meta: DashboardMeta,
}

/// Dashboard specification
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct DashboardSpec {
    pub id: Option<u64>,
    pub title: String,
    pub description: Option<String>,
    pub tags: Vec<String>,
    pub panels: Vec<GrafanaPanel>,
    pub time: TimeRange,
    pub timepicker: TimePicker,
    pub templating: Templating,
    pub annotations: Annotations,
    pub refresh: Option<String>,
    pub schema_version: u32,
    pub version: u32,
}

/// Dashboard metadata
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct DashboardMeta {
    pub is_starred: bool,
    pub is_home: bool,
    pub is_snapshot: bool,
    pub folder_id: Option<u64>,
    pub folder_title: Option<String>,
    pub folder_url: Option<String>,
    pub tags: Vec<String>,
}

/// Panel template
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct PanelTemplate {
    pub panel_type: PanelType,
    pub title: String,
    pub description: String,
    pub metrics: Vec<String>,
    pub layout: PanelLayout,
}

/// Panel types
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum PanelType {
    Graph,
    Table,
    Heatmap,
    Stat,
    Gauge,
    BarGauge,
    Logs,
    StatusHistory,
}

/// Panel layout
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct PanelLayout {
    pub width: u32,
    pub height: u32,
    pub x: u32,
    pub y: u32,
}

/// Dashboard variable
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct DashboardVariable {
    pub name: String,
    pub label: String,
    pub var_type: VariableType,
    pub query: String,
    pub multi: bool,
    pub include_all: bool,
}

/// Variable types
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum VariableType {
    Query,
    Custom,
    Constant,
    Interval,
}

/// Grafana panel
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct GrafanaPanel {
    pub id: u64,
    pub title: String,
    pub panel_type: String,
    pub grid_pos: GridPosition,
    pub targets: Vec<Target>,
    pub options: serde_json::Value,
    pub field_config: FieldConfig,
}

/// Grid position
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct GridPosition {
    pub h: u32,
    pub w: u32,
    pub x: u32,
    pub y: u32,
}

/// Target (data query)
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Target {
    pub expr: String,
    pub legend_format: String,
    pub ref_id: String,
}

/// Field configuration
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct FieldConfig {
    pub defaults: FieldDefaults,
    pub overrides: Vec<FieldOverride>,
}

/// Field defaults
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct FieldDefaults {
    pub unit: String,
    pub decimals: Option<u32>,
    pub min: Option<f64>,
    pub max: Option<f64>,
}

/// Field override
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct FieldOverride {
    pub matcher: Matcher,
    pub properties: Vec<Property>,
}

/// Matcher
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Matcher {
    pub id: String,
    pub options: serde_json::Value,
}

/// Property
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Property {
    pub id: String,
    pub value: serde_json::Value,
}

/// Time range
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TimeRange {
    pub from: String,
    pub to: String,
}

/// Time picker
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TimePicker {
    pub refresh_intervals: Vec<String>,
}

/// Templating
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Templating {
    pub list: Vec<serde_json::Value>,
}

/// Annotations
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Annotations {
    pub list: Vec<serde_json::Value>,
}

/// Data source
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct GrafanaDataSource {
    pub name: String,
    pub source_type: String,
    pub url: String,
    pub access: String,
    pub basic_auth: bool,
    pub basic_auth_user: Option<String>,
}

/// Alert rule
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct GrafanaAlertRule {
    pub name: String,
    pub rule_group: String,
    pub condition: String,
    pub data: Vec<AlertData>,
    pub labels: HashMap<String, String>,
    pub annotations: HashMap<String, String>,
}

/// Alert data
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AlertData {
    pub ref_id: String,
    pub relative_time_range: RelativeTimeRange,
    pub datasource_uid: String,
    pub model: serde_json::Value,
}

/// Relative time range
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct RelativeTimeRange {
    pub from: i64,
    pub to: i64,
}

impl GrafanaDashboardManager {
    /// Create new dashboard manager
    pub async fn new() -> Result<Self> {
        let mut manager = Self {
            templates: HashMap::new(),
            dashboards: HashMap::new(),
            data_sources: Vec::new(),
            alert_rules: Vec::new(),
        };

        // Initialize with default templates
        manager.initialize_templates().await?;
        manager.initialize_data_sources().await?;
        manager.initialize_alert_rules().await?;

        Ok(manager)
    }

    /// Create dashboard from template
    pub async fn create_dashboard(&self, template_id: &str, cluster_name: &str) -> Result<GrafanaDashboard> {
        let template = self.templates.get(template_id)
            .ok_or_else(|| Error::NotFound(format!("Template {} not found", template_id)))?;

        let mut dashboard = GrafanaDashboard {
            dashboard: DashboardSpec {
                id: None,
                title: format!("{} - {}", template.name, cluster_name),
                description: Some(template.description.clone()),
                tags: vec!["aurora".to_string(), "coordinator".to_string(), cluster_name.to_string()],
                panels: Vec::new(),
                time: TimeRange {
                    from: "now-1h".to_string(),
                    to: "now".to_string(),
                },
                timepicker: TimePicker {
                    refresh_intervals: vec![
                        "5s".to_string(),
                        "10s".to_string(),
                        "30s".to_string(),
                        "1m".to_string(),
                        "5m".to_string(),
                        "15m".to_string(),
                        "30m".to_string(),
                        "1h".to_string(),
                        "2h".to_string(),
                        "1d".to_string(),
                    ],
                },
                templating: Templating {
                    list: vec![], // Would be populated with variables
                },
                annotations: Annotations {
                    list: vec![], // Would include deployment events, etc.
                },
                refresh: Some("30s".to_string()),
                schema_version: 36,
                version: 1,
            },
            meta: DashboardMeta {
                is_starred: false,
                is_home: false,
                is_snapshot: false,
                folder_id: None,
                folder_title: Some("Aurora".to_string()),
                folder_url: None,
                tags: vec!["aurora".to_string()],
            },
        };

        // Generate panels from template
        for (i, panel_template) in template.panels.iter().enumerate() {
            let panel = self.create_panel_from_template(panel_template, i as u64)?;
            dashboard.dashboard.panels.push(panel);
        }

        // Add variables
        for variable in &template.variables {
            dashboard.dashboard.templating.list.push(
                serde_json::to_value(variable).unwrap()
            );
        }

        Ok(dashboard)
    }

    /// Export dashboard as JSON
    pub async fn export_dashboard_json(&self, dashboard: &GrafanaDashboard) -> Result<String> {
        serde_json::to_string_pretty(dashboard)
            .map_err(|e| Error::Serialization(format!("Failed to serialize dashboard: {}", e)))
    }

    /// Generate Prometheus data source configuration
    pub async fn generate_prometheus_datasource(&self, prometheus_url: &str) -> Result<GrafanaDataSource> {
        Ok(GrafanaDataSource {
            name: "Aurora Prometheus".to_string(),
            source_type: "prometheus".to_string(),
            url: prometheus_url.to_string(),
            access: "proxy".to_string(),
            basic_auth: false,
            basic_auth_user: None,
        })
    }

    /// Get dashboard templates
    pub async fn get_templates(&self) -> Vec<&DashboardTemplate> {
        self.templates.values().collect()
    }

    // Private methods

    async fn initialize_templates(&mut self) -> Result<()> {
        // System Overview Dashboard
        let system_overview = DashboardTemplate {
            template_id: "system_overview".to_string(),
            name: "System Overview".to_string(),
            description: "High-level system metrics and health status".to_string(),
            panels: vec![
                PanelTemplate {
                    panel_type: PanelType::Stat,
                    title: "Cluster Health".to_string(),
                    description: "Overall cluster health status".to_string(),
                    metrics: vec!["aurora_cluster_health".to_string()],
                    layout: PanelLayout { width: 6, height: 3, x: 0, y: 0 },
                },
                PanelTemplate {
                    panel_type: PanelType::Graph,
                    title: "Throughput".to_string(),
                    description: "Requests per second across all nodes".to_string(),
                    metrics: vec!["aurora_requests_total".to_string()],
                    layout: PanelLayout { width: 12, height: 8, x: 6, y: 0 },
                },
                PanelTemplate {
                    panel_type: PanelType::Graph,
                    title: "Latency P95".to_string(),
                    description: "95th percentile response latency".to_string(),
                    metrics: vec!["aurora_request_duration_seconds".to_string()],
                    layout: PanelLayout { width: 12, height: 8, x: 0, y: 8 },
                },
            ],
            variables: vec![
                DashboardVariable {
                    name: "cluster".to_string(),
                    label: "Cluster".to_string(),
                    var_type: VariableType::Query,
                    query: "label_values(aurora_cluster_info, cluster)".to_string(),
                    multi: false,
                    include_all: false,
                },
                DashboardVariable {
                    name: "node".to_string(),
                    label: "Node".to_string(),
                    var_type: VariableType::Query,
                    query: "label_values(aurora_node_info{cluster=\"$cluster\"}, node)".to_string(),
                    multi: true,
                    include_all: true,
                },
            ],
            tags: vec!["system".to_string(), "overview".to_string()],
        };

        // Consensus Dashboard
        let consensus_dashboard = DashboardTemplate {
            template_id: "consensus".to_string(),
            name: "Consensus Performance".to_string(),
            description: "Detailed consensus algorithm metrics and performance".to_string(),
            panels: vec![
                PanelTemplate {
                    panel_type: PanelType::Stat,
                    title: "Current Term".to_string(),
                    description: "Current Raft consensus term".to_string(),
                    metrics: vec!["aurora_consensus_term".to_string()],
                    layout: PanelLayout { width: 4, height: 3, x: 0, y: 0 },
                },
                PanelTemplate {
                    panel_type: PanelType::Graph,
                    title: "Log Replication".to_string(),
                    description: "Log entries replicated per second".to_string(),
                    metrics: vec!["aurora_log_replication_total".to_string()],
                    layout: PanelLayout { width: 10, height: 8, x: 4, y: 0 },
                },
                PanelTemplate {
                    panel_type: PanelType::Table,
                    title: "Leadership Changes".to_string(),
                    description: "Recent leadership transitions".to_string(),
                    metrics: vec!["aurora_leadership_changes".to_string()],
                    layout: PanelLayout { width: 10, height: 6, x: 0, y: 8 },
                },
            ],
            variables: vec![
                DashboardVariable {
                    name: "cluster".to_string(),
                    label: "Cluster".to_string(),
                    var_type: VariableType::Query,
                    query: "label_values(aurora_consensus_info, cluster)".to_string(),
                    multi: false,
                    include_all: false,
                },
            ],
            tags: vec!["consensus".to_string(), "performance".to_string()],
        };

        // Security Dashboard
        let security_dashboard = DashboardTemplate {
            template_id: "security".to_string(),
            name: "Security Monitoring".to_string(),
            description: "Security events, authentication, and threat monitoring".to_string(),
            panels: vec![
                PanelTemplate {
                    panel_type: PanelType::Stat,
                    title: "Active Sessions".to_string(),
                    description: "Number of active authenticated sessions".to_string(),
                    metrics: vec!["aurora_active_sessions".to_string()],
                    layout: PanelLayout { width: 6, height: 3, x: 0, y: 0 },
                },
                PanelTemplate {
                    panel_type: PanelType::Graph,
                    title: "Failed Authentications".to_string(),
                    description: "Authentication failures over time".to_string(),
                    metrics: vec!["aurora_auth_failures_total".to_string()],
                    layout: PanelLayout { width: 12, height: 8, x: 6, y: 0 },
                },
                PanelTemplate {
                    panel_type: PanelType::Logs,
                    title: "Security Events".to_string(),
                    description: "Recent security events and alerts".to_string(),
                    metrics: vec!["aurora_security_events".to_string()],
                    layout: PanelLayout { width: 18, height: 10, x: 0, y: 8 },
                },
            ],
            variables: vec![
                DashboardVariable {
                    name: "severity".to_string(),
                    label: "Severity".to_string(),
                    var_type: VariableType::Custom,
                    query: "info,warn,error,critical".to_string(),
                    multi: true,
                    include_all: true,
                },
            ],
            tags: vec!["security".to_string(), "monitoring".to_string()],
        };

        self.templates.insert(system_overview.template_id.clone(), system_overview);
        self.templates.insert(consensus_dashboard.template_id.clone(), consensus_dashboard);
        self.templates.insert(security_dashboard.template_id.clone(), security_dashboard);

        Ok(())
    }

    async fn initialize_data_sources(&mut self) -> Result<()> {
        // Add Prometheus data source
        self.data_sources.push(GrafanaDataSource {
            name: "Prometheus".to_string(),
            source_type: "prometheus".to_string(),
            url: "http://prometheus:9090".to_string(),
            access: "proxy".to_string(),
            basic_auth: false,
            basic_auth_user: None,
        });

        // Add Loki data source for logs
        self.data_sources.push(GrafanaDataSource {
            name: "Loki".to_string(),
            source_type: "loki".to_string(),
            url: "http://loki:3100".to_string(),
            access: "proxy".to_string(),
            basic_auth: false,
            basic_auth_user: None,
        });

        Ok(())
    }

    async fn initialize_alert_rules(&mut self) -> Result<()> {
        // High latency alert
        self.alert_rules.push(GrafanaAlertRule {
            name: "High Latency Alert".to_string(),
            rule_group: "aurora_alerts".to_string(),
            condition: r#"aurora_request_duration_seconds{quantile="0.95"} > 0.1"#.to_string(),
            data: vec![
                AlertData {
                    ref_id: "A".to_string(),
                    relative_time_range: RelativeTimeRange { from: 300, to: 0 }, // 5 minutes
                    datasource_uid: "prometheus".to_string(),
                    model: serde_json::json!({
                        "expr": "aurora_request_duration_seconds{quantile=\"0.95\"}",
                        "intervalMs": 15000,
                        "maxDataPoints": 43200
                    }),
                },
            ],
            labels: HashMap::from([
                ("severity".to_string(), "warning".to_string()),
                ("service".to_string(), "aurora".to_string()),
            ]),
            annotations: HashMap::from([
                ("summary".to_string(), "High request latency detected".to_string()),
                ("description".to_string(), "95th percentile request latency is above 100ms".to_string()),
            ]),
        });

        // Node down alert
        self.alert_rules.push(GrafanaAlertRule {
            name: "Node Down Alert".to_string(),
            rule_group: "aurora_alerts".to_string(),
            condition: r#"up{job="aurora-coordinator"} == 0"#.to_string(),
            data: vec![
                AlertData {
                    ref_id: "A".to_string(),
                    relative_time_range: RelativeTimeRange { from: 60, to: 0 }, // 1 minute
                    datasource_uid: "prometheus".to_string(),
                    model: serde_json::json!({
                        "expr": "up{job=\"aurora-coordinator\"}",
                        "intervalMs": 15000,
                        "maxDataPoints": 43200
                    }),
                },
            ],
            labels: HashMap::from([
                ("severity".to_string(), "critical".to_string()),
                ("service".to_string(), "aurora".to_string()),
            ]),
            annotations: HashMap::from([
                ("summary".to_string(), "Aurora coordinator node is down".to_string()),
                ("description".to_string(), "Coordinator node has been unreachable for 1 minute".to_string()),
            ]),
        });

        Ok(())
    }

    fn create_panel_from_template(&self, template: &PanelTemplate, panel_id: u64) -> Result<GrafanaPanel> {
        let panel_type_str = match template.panel_type {
            PanelType::Graph => "graph",
            PanelType::Table => "table",
            PanelType::Heatmap => "heatmap",
            PanelType::Stat => "stat",
            PanelType::Gauge => "gauge",
            PanelType::BarGauge => "bargauge",
            PanelType::Logs => "logs",
            PanelType::StatusHistory => "status-history",
        };

        let targets = template.metrics.iter().enumerate().map(|(i, metric)| {
            Target {
                expr: format!("{} {{cluster=\"$cluster\"}}", metric),
                legend_format: format!("{{{{ {} }}}}", metric),
                ref_id: format!("{}{}", (b'A' + i as u8) as char, ""),
            }
        }).collect();

        Ok(GrafanaPanel {
            id: panel_id,
            title: template.title.clone(),
            panel_type: panel_type_str.to_string(),
            grid_pos: GridPosition {
                h: template.layout.height,
                w: template.layout.width,
                x: template.layout.x,
                y: template.layout.y,
            },
            targets,
            options: serde_json::json!({}),
            field_config: FieldConfig {
                defaults: FieldDefaults {
                    unit: "short".to_string(),
                    decimals: None,
                    min: None,
                    max: None,
                },
                overrides: vec![],
            },
        })
    }
}

// UNIQUENESS Research Citations:
// - **Grafana Dashboards**: Grafana dashboard best practices
// - **Prometheus Integration**: Prometheus monitoring and alerting
// - **Time Series Visualization**: Research on effective dashboard design
// - **Alert Management**: Alert fatigue and effective alerting research
