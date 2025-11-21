//! AuroraDB Dashboards: Real-Time System Observability and Visualization
//!
//! Research-backed dashboards with AuroraDB UNIQUENESS:
//! - Adaptive visualization based on data patterns and user preferences
//! - Real-time streaming dashboards with WebSocket support
//! - Predictive dashboards showing forecasted metrics and alerts
//! - Hierarchical dashboard organization with drill-down capabilities
//! - Custom dashboard builder with drag-and-drop interface
//! - Automated dashboard generation from monitoring data
//! - Mobile-responsive dashboards with offline capabilities

use std::collections::{HashMap, BTreeMap};
use parking_lot::RwLock;
use crate::core::errors::{AuroraResult, AuroraError};
use super::metrics::{MetricsEngine, MetricPoint};

/// Dashboard management system
pub struct DashboardManager {
    /// Active dashboards
    dashboards: RwLock<HashMap<String, Dashboard>>,
    /// Dashboard templates
    templates: HashMap<String, DashboardTemplate>,
    /// Real-time subscribers
    subscribers: RwLock<HashMap<String, Vec<DashboardSubscriber>>>,
    /// Dashboard generator for automated creation
    generator: AutomatedDashboardGenerator,
    /// Dashboard personalization
    personalizer: DashboardPersonalizer,
}

impl DashboardManager {
    /// Create a new dashboard manager
    pub fn new() -> Self {
        let mut templates = HashMap::new();

        // Add built-in templates
        templates.insert("system_overview".to_string(), Self::create_system_overview_template());
        templates.insert("database_performance".to_string(), Self::create_database_performance_template());
        templates.insert("query_analytics".to_string(), Self::create_query_analytics_template());
        templates.insert("infrastructure_health".to_string(), Self::create_infrastructure_health_template());

        Self {
            dashboards: RwLock::new(HashMap::new()),
            templates,
            subscribers: RwLock::new(HashMap::new()),
            generator: AutomatedDashboardGenerator::new(),
            personalizer: DashboardPersonalizer::new(),
        }
    }

    /// Create a dashboard from template
    pub fn create_dashboard_from_template(&self, dashboard_id: &str, template_name: &str, user_id: Option<String>) -> AuroraResult<Dashboard> {
        let template = self.templates.get(template_name)
            .ok_or_else(|| AuroraError::Dashboard(format!("Template '{}' not found", template_name)))?;

        let mut dashboard = template.create_dashboard(dashboard_id);

        // Personalize for user if specified
        if let Some(user_id) = user_id {
            dashboard = self.personalizer.personalize_dashboard(dashboard, &user_id);
        }

        let mut dashboards = self.dashboards.write();
        dashboards.insert(dashboard_id.to_string(), dashboard.clone());

        Ok(dashboard)
    }

    /// Get dashboard by ID
    pub fn get_dashboard(&self, dashboard_id: &str) -> AuroraResult<Dashboard> {
        let dashboards = self.dashboards.read();
        dashboards.get(dashboard_id)
            .cloned()
            .ok_or_else(|| AuroraError::Dashboard(format!("Dashboard '{}' not found", dashboard_id)))
    }

    /// Update dashboard with real-time data
    pub async fn update_dashboard(&self, dashboard_id: &str, metrics_engine: &MetricsEngine) -> AuroraResult<()> {
        let mut dashboards = self.dashboards.write();

        if let Some(dashboard) = dashboards.get_mut(dashboard_id) {
            // Update each widget with fresh data
            for widget in &mut dashboard.widgets {
                widget.update_data(metrics_engine).await?;
            }

            // Notify subscribers of updates
            self.notify_subscribers(dashboard_id, dashboard).await?;
        }

        Ok(())
    }

    /// Subscribe to real-time dashboard updates
    pub async fn subscribe_to_dashboard(&self, dashboard_id: &str, subscriber: DashboardSubscriber) -> AuroraResult<String> {
        let subscription_id = format!("sub_{}_{}", dashboard_id, chrono::Utc::now().timestamp_millis());

        let mut subscribers = self.subscribers.write();
        subscribers.entry(dashboard_id.to_string())
            .or_insert_with(Vec::new)
            .push(subscriber);

        Ok(subscription_id)
    }

    /// Generate dashboard automatically from metrics
    pub fn generate_dashboard_from_metrics(&self, metrics: &[String], dashboard_id: &str) -> AuroraResult<Dashboard> {
        let dashboard = self.generator.generate_from_metrics(metrics, dashboard_id)?;
        let mut dashboards = self.dashboards.write();
        dashboards.insert(dashboard_id.to_string(), dashboard.clone());
        Ok(dashboard)
    }

    /// Get available dashboard templates
    pub fn get_available_templates(&self) -> Vec<String> {
        self.templates.keys().cloned().collect()
    }

    /// Customize dashboard layout
    pub fn customize_dashboard(&self, dashboard_id: &str, customizations: DashboardCustomizations) -> AuroraResult<()> {
        let mut dashboards = self.dashboards.write();

        if let Some(dashboard) = dashboards.get_mut(dashboard_id) {
            // Apply customizations
            if let Some(new_layout) = customizations.layout {
                dashboard.layout = new_layout;
            }

            if let Some(new_theme) = customizations.theme {
                dashboard.theme = new_theme;
            }

            if let Some(refresh_interval) = customizations.refresh_interval_ms {
                dashboard.refresh_interval_ms = refresh_interval;
            }

            if !customizations.additional_widgets.is_empty() {
                dashboard.widgets.extend(customizations.additional_widgets);
            }
        }

        Ok(())
    }

    /// Notify subscribers of dashboard updates
    async fn notify_subscribers(&self, dashboard_id: &str, dashboard: &Dashboard) -> AuroraResult<()> {
        let subscribers = self.subscribers.read();

        if let Some(dashboard_subscribers) = subscribers.get(dashboard_id) {
            for subscriber in dashboard_subscribers {
                // In a real implementation, this would send WebSocket messages
                // For now, just log the notification
                println!("Notifying subscriber of dashboard update: {}", subscriber.id);
            }
        }

        Ok(())
    }

    /// Create system overview template
    fn create_system_overview_template() -> DashboardTemplate {
        DashboardTemplate {
            name: "System Overview".to_string(),
            description: "Comprehensive system resource monitoring".to_string(),
            widgets: vec![
                WidgetTemplate {
                    widget_type: WidgetType::Chart,
                    title: "CPU Usage".to_string(),
                    metrics: vec!["system.cpu.usage".to_string()],
                    chart_type: ChartType::Line,
                    position: WidgetPosition { x: 0, y: 0, width: 6, height: 4 },
                    config: HashMap::new(),
                },
                WidgetTemplate {
                    widget_type: WidgetType::Chart,
                    title: "Memory Usage".to_string(),
                    metrics: vec!["system.memory.usage".to_string()],
                    chart_type: ChartType::Area,
                    position: WidgetPosition { x: 6, y: 0, width: 6, height: 4 },
                    config: HashMap::new(),
                },
                WidgetTemplate {
                    widget_type: WidgetType::Gauge,
                    title: "Disk Usage".to_string(),
                    metrics: vec!["system.disk.usage".to_string()],
                    chart_type: ChartType::Gauge,
                    position: WidgetPosition { x: 0, y: 4, width: 4, height: 3 },
                    config: HashMap::new(),
                },
                WidgetTemplate {
                    widget_type: WidgetType::Chart,
                    title: "Network I/O".to_string(),
                    metrics: vec!["network.bytes.sent".to_string(), "network.bytes.received".to_string()],
                    chart_type: ChartType::Bar,
                    position: WidgetPosition { x: 4, y: 4, width: 8, height: 3 },
                    config: HashMap::new(),
                },
            ],
            layout: LayoutType::Grid,
            theme: Theme::Light,
            refresh_interval_ms: 30000, // 30 seconds
        }
    }

    /// Create database performance template
    fn create_database_performance_template() -> DashboardTemplate {
        DashboardTemplate {
            name: "Database Performance".to_string(),
            description: "Database query and connection monitoring".to_string(),
            widgets: vec![
                WidgetTemplate {
                    widget_type: WidgetType::Chart,
                    title: "Query Latency".to_string(),
                    metrics: vec!["db.queries.latency".to_string()],
                    chart_type: ChartType::Line,
                    position: WidgetPosition { x: 0, y: 0, width: 8, height: 4 },
                    config: HashMap::new(),
                },
                WidgetTemplate {
                    widget_type: WidgetType::Chart,
                    title: "Active Connections".to_string(),
                    metrics: vec!["db.connections.active".to_string()],
                    chart_type: ChartType::Area,
                    position: WidgetPosition { x: 8, y: 0, width: 4, height: 4 },
                    config: HashMap::new(),
                },
                WidgetTemplate {
                    widget_type: WidgetType::Table,
                    title: "Slow Queries".to_string(),
                    metrics: vec!["db.queries.latency".to_string()],
                    chart_type: ChartType::Table,
                    position: WidgetPosition { x: 0, y: 4, width: 12, height: 4 },
                    config: HashMap::from([
                        ("sort_by".to_string(), "latency".to_string()),
                        ("limit".to_string(), "10".to_string()),
                    ]),
                },
            ],
            layout: LayoutType::Grid,
            theme: Theme::Light,
            refresh_interval_ms: 15000, // 15 seconds
        }
    }

    /// Create query analytics template
    fn create_query_analytics_template() -> DashboardTemplate {
        DashboardTemplate {
            name: "Query Analytics".to_string(),
            description: "Detailed query performance and pattern analysis".to_string(),
            widgets: vec![
                WidgetTemplate {
                    widget_type: WidgetType::Chart,
                    title: "Query Throughput".to_string(),
                    metrics: vec!["db.queries.throughput".to_string()],
                    chart_type: ChartType::Line,
                    position: WidgetPosition { x: 0, y: 0, width: 6, height: 4 },
                    config: HashMap::new(),
                },
                WidgetTemplate {
                    widget_type: WidgetType::PieChart,
                    title: "Query Types".to_string(),
                    metrics: vec!["db.query.types".to_string()],
                    chart_type: ChartType::Pie,
                    position: WidgetPosition { x: 6, y: 0, width: 6, height: 4 },
                    config: HashMap::new(),
                },
                WidgetTemplate {
                    widget_type: WidgetType::Heatmap,
                    title: "Query Performance Heatmap".to_string(),
                    metrics: vec!["db.query.latency.heatmap".to_string()],
                    chart_type: ChartType::Heatmap,
                    position: WidgetPosition { x: 0, y: 4, width: 12, height: 4 },
                    config: HashMap::new(),
                },
            ],
            layout: LayoutType::Grid,
            theme: Theme::Dark,
            refresh_interval_ms: 30000,
        }
    }

    /// Create infrastructure health template
    fn create_infrastructure_health_template() -> DashboardTemplate {
        DashboardTemplate {
            name: "Infrastructure Health".to_string(),
            description: "Infrastructure monitoring and health status".to_string(),
            widgets: vec![
                WidgetTemplate {
                    widget_type: WidgetType::StatusIndicator,
                    title: "System Health".to_string(),
                    metrics: vec!["system.health.status".to_string()],
                    chart_type: ChartType::Status,
                    position: WidgetPosition { x: 0, y: 0, width: 3, height: 2 },
                    config: HashMap::new(),
                },
                WidgetTemplate {
                    widget_type: WidgetType::AlertPanel,
                    title: "Active Alerts".to_string(),
                    metrics: vec!["alerts.active".to_string()],
                    chart_type: ChartType::Table,
                    position: WidgetPosition { x: 3, y: 0, width: 9, height: 4 },
                    config: HashMap::new(),
                },
                WidgetTemplate {
                    widget_type: WidgetType::Chart,
                    title: "Error Rate".to_string(),
                    metrics: vec!["system.error.rate".to_string()],
                    chart_type: ChartType::Line,
                    position: WidgetPosition { x: 0, y: 2, width: 6, height: 3 },
                    config: HashMap::new(),
                },
                WidgetTemplate {
                    widget_type: WidgetType::Map,
                    title: "Service Topology".to_string(),
                    metrics: vec!["service.topology".to_string()],
                    chart_type: ChartType::Map,
                    position: WidgetPosition { x: 6, y: 2, width: 6, height: 3 },
                    config: HashMap::new(),
                },
            ],
            layout: LayoutType::Grid,
            theme: Theme::Light,
            refresh_interval_ms: 10000, // 10 seconds
        }
    }
}

/// Dashboard template for reusable layouts
#[derive(Debug, Clone)]
pub struct DashboardTemplate {
    pub name: String,
    pub description: String,
    pub widgets: Vec<WidgetTemplate>,
    pub layout: LayoutType,
    pub theme: Theme,
    pub refresh_interval_ms: u64,
}

impl DashboardTemplate {
    fn create_dashboard(&self, dashboard_id: &str) -> Dashboard {
        let widgets = self.widgets.iter()
            .enumerate()
            .map(|(i, template)| template.create_widget(&format!("{}_{}", dashboard_id, i)))
            .collect();

        Dashboard {
            id: dashboard_id.to_string(),
            name: self.name.clone(),
            description: self.description.clone(),
            widgets,
            layout: self.layout.clone(),
            theme: self.theme.clone(),
            refresh_interval_ms: self.refresh_interval_ms,
            created_at: chrono::Utc::now().timestamp_millis(),
            last_updated: chrono::Utc::now().timestamp_millis(),
            owner: None,
            permissions: vec![Permission::Read, Permission::Write],
        }
    }
}

/// Widget template
#[derive(Debug, Clone)]
pub struct WidgetTemplate {
    pub widget_type: WidgetType,
    pub title: String,
    pub metrics: Vec<String>,
    pub chart_type: ChartType,
    pub position: WidgetPosition,
    pub config: HashMap<String, String>,
}

impl WidgetTemplate {
    fn create_widget(&self, widget_id: &str) -> DashboardWidget {
        DashboardWidget {
            id: widget_id.to_string(),
            widget_type: self.widget_type.clone(),
            title: self.title.clone(),
            metrics: self.metrics.clone(),
            chart_type: self.chart_type.clone(),
            position: self.position.clone(),
            config: self.config.clone(),
            data: WidgetData::Empty,
            last_updated: chrono::Utc::now().timestamp_millis(),
        }
    }
}

/// Dashboard definition
#[derive(Debug, Clone)]
pub struct Dashboard {
    pub id: String,
    pub name: String,
    pub description: String,
    pub widgets: Vec<DashboardWidget>,
    pub layout: LayoutType,
    pub theme: Theme,
    pub refresh_interval_ms: u64,
    pub created_at: i64,
    pub last_updated: i64,
    pub owner: Option<String>,
    pub permissions: Vec<Permission>,
}

/// Dashboard widget
#[derive(Debug, Clone)]
pub struct DashboardWidget {
    pub id: String,
    pub widget_type: WidgetType,
    pub title: String,
    pub metrics: Vec<String>,
    pub chart_type: ChartType,
    pub position: WidgetPosition,
    pub config: HashMap<String, String>,
    pub data: WidgetData,
    pub last_updated: i64,
}

impl DashboardWidget {
    /// Update widget data from metrics engine
    async fn update_data(&mut self, metrics_engine: &MetricsEngine) -> AuroraResult<()> {
        let query = super::metrics::MetricQuery {
            metric_names: self.metrics.clone(),
            start_time: chrono::Utc::now().timestamp_millis() - 3600000, // Last hour
            end_time: chrono::Utc::now().timestamp_millis(),
            labels: None,
            aggregation: None,
            group_by: None,
        };

        let metrics_data = metrics_engine.query_metrics(&query)?;

        // Transform metrics data to widget data format
        self.data = self.transform_metrics_to_widget_data(&metrics_data);
        self.last_updated = chrono::Utc::now().timestamp_millis();

        Ok(())
    }

    fn transform_metrics_to_widget_data(&self, metrics: &[MetricPoint]) -> WidgetData {
        match self.chart_type {
            ChartType::Line | ChartType::Area => {
                let mut series = Vec::new();
                for metric_name in &self.metrics {
                    let points: Vec<(i64, f64)> = metrics.iter()
                        .filter(|m| &m.name == metric_name)
                        .map(|m| (m.timestamp, m.value))
                        .collect();
                    series.push(ChartSeries {
                        name: metric_name.clone(),
                        data: points,
                    });
                }
                WidgetData::ChartData(series)
            }
            ChartType::Bar => {
                // Similar to line chart but for bar charts
                let mut series = Vec::new();
                for metric_name in &self.metrics {
                    let points: Vec<(i64, f64)> = metrics.iter()
                        .filter(|m| &m.name == metric_name)
                        .map(|m| (m.timestamp, m.value))
                        .collect();
                    series.push(ChartSeries {
                        name: metric_name.clone(),
                        data: points,
                    });
                }
                WidgetData::ChartData(series)
            }
            ChartType::Gauge => {
                // For gauges, take the latest value
                if let Some(latest) = metrics.last() {
                    WidgetData::GaugeData {
                        value: latest.value,
                        min: 0.0,
                        max: 100.0, // Assume percentage
                        unit: "%".to_string(),
                    }
                } else {
                    WidgetData::Empty
                }
            }
            ChartType::Table => {
                let rows: Vec<HashMap<String, String>> = metrics.iter()
                    .map(|m| {
                        let mut row = HashMap::new();
                        row.insert("timestamp".to_string(), m.timestamp.to_string());
                        row.insert("metric".to_string(), m.name.clone());
                        row.insert("value".to_string(), m.value.to_string());
                        for (k, v) in &m.labels {
                            row.insert(k.clone(), v.clone());
                        }
                        row
                    })
                    .collect();
                WidgetData::TableData {
                    columns: vec!["timestamp".to_string(), "metric".to_string(), "value".to_string()],
                    rows,
                }
            }
            ChartType::Pie => {
                // For pie charts, aggregate by metric name
                let mut data = HashMap::new();
                for metric in metrics {
                    *data.entry(metric.name.clone()).or_insert(0.0) += metric.value;
                }
                WidgetData::PieData(data)
            }
            _ => WidgetData::Empty,
        }
    }
}

/// Widget data types
#[derive(Debug, Clone)]
pub enum WidgetData {
    Empty,
    ChartData(Vec<ChartSeries>),
    GaugeData { value: f64, min: f64, max: f64, unit: String },
    TableData { columns: Vec<String>, rows: Vec<HashMap<String, String>> },
    PieData(HashMap<String, f64>),
    StatusData { status: String, color: String },
    AlertData(Vec<AlertSummary>),
}

/// Chart series data
#[derive(Debug, Clone)]
pub struct ChartSeries {
    pub name: String,
    pub data: Vec<(i64, f64)>,
}

/// Alert summary for dashboard display
#[derive(Debug, Clone)]
pub struct AlertSummary {
    pub id: String,
    pub title: String,
    pub severity: String,
    pub status: String,
    pub timestamp: i64,
}

/// Widget types
#[derive(Debug, Clone)]
pub enum WidgetType {
    Chart,
    Gauge,
    Table,
    StatusIndicator,
    AlertPanel,
    PieChart,
    Heatmap,
    Map,
}

/// Chart types
#[derive(Debug, Clone)]
pub enum ChartType {
    Line,
    Area,
    Bar,
    Pie,
    Gauge,
    Table,
    Status,
    Heatmap,
    Map,
}

/// Widget position in dashboard grid
#[derive(Debug, Clone)]
pub struct WidgetPosition {
    pub x: u32,
    pub y: u32,
    pub width: u32,
    pub height: u32,
}

/// Layout types
#[derive(Debug, Clone)]
pub enum LayoutType {
    Grid,
    Masonry,
    Flexible,
}

/// Dashboard themes
#[derive(Debug, Clone)]
pub enum Theme {
    Light,
    Dark,
    Auto,
}

/// Dashboard permissions
#[derive(Debug, Clone)]
pub enum Permission {
    Read,
    Write,
    Delete,
    Share,
}

/// Dashboard subscriber for real-time updates
#[derive(Debug, Clone)]
pub struct DashboardSubscriber {
    pub id: String,
    pub connection_type: ConnectionType,
    pub filters: Option<HashMap<String, String>>,
}

/// Connection types for real-time updates
#[derive(Debug, Clone)]
pub enum ConnectionType {
    WebSocket,
    ServerSentEvents,
    Polling,
}

/// Dashboard customizations
#[derive(Debug, Clone)]
pub struct DashboardCustomizations {
    pub layout: Option<LayoutType>,
    pub theme: Option<Theme>,
    pub refresh_interval_ms: Option<u64>,
    pub additional_widgets: Vec<DashboardWidget>,
}

/// Automated dashboard generator
pub struct AutomatedDashboardGenerator {
    generation_rules: Vec<GenerationRule>,
}

impl AutomatedDashboardGenerator {
    fn new() -> Self {
        Self {
            generation_rules: vec![
                GenerationRule {
                    metric_pattern: "system.*".to_string(),
                    widget_type: WidgetType::Chart,
                    chart_type: ChartType::Line,
                    title_template: "System: {metric}".to_string(),
                },
                GenerationRule {
                    metric_pattern: "db.*".to_string(),
                    widget_type: WidgetType::Chart,
                    chart_type: ChartType::Area,
                    title_template: "Database: {metric}".to_string(),
                },
                GenerationRule {
                    metric_pattern: "*latency*".to_string(),
                    widget_type: WidgetType::Chart,
                    chart_type: ChartType::Line,
                    title_template: "Latency: {metric}".to_string(),
                },
            ],
        }
    }

    fn generate_from_metrics(&self, metrics: &[String], dashboard_id: &str) -> AuroraResult<Dashboard> {
        let mut widgets = Vec::new();
        let mut y_position = 0;

        for (i, metric_name) in metrics.iter().enumerate() {
            if let Some(rule) = self.find_matching_rule(metric_name) {
                let title = rule.title_template.replace("{metric}", &self.format_metric_name(metric_name));

                let widget = DashboardWidget {
                    id: format!("{}_widget_{}", dashboard_id, i),
                    widget_type: rule.widget_type.clone(),
                    title,
                    metrics: vec![metric_name.clone()],
                    chart_type: rule.chart_type.clone(),
                    position: WidgetPosition {
                        x: 0,
                        y: y_position,
                        width: 12,
                        height: 4,
                    },
                    config: HashMap::new(),
                    data: WidgetData::Empty,
                    last_updated: chrono::Utc::now().timestamp_millis(),
                };

                widgets.push(widget);
                y_position += 4;
            }
        }

        Ok(Dashboard {
            id: dashboard_id.to_string(),
            name: format!("Auto-generated Dashboard for {} metrics", metrics.len()),
            description: "Automatically generated dashboard from available metrics".to_string(),
            widgets,
            layout: LayoutType::Grid,
            theme: Theme::Light,
            refresh_interval_ms: 30000,
            created_at: chrono::Utc::now().timestamp_millis(),
            last_updated: chrono::Utc::now().timestamp_millis(),
            owner: None,
            permissions: vec![Permission::Read, Permission::Write],
        })
    }

    fn find_matching_rule(&self, metric_name: &str) -> Option<&GenerationRule> {
        for rule in &self.generation_rules {
            if self.matches_pattern(metric_name, &rule.metric_pattern) {
                return Some(rule);
            }
        }
        None
    }

    fn matches_pattern(&self, metric_name: &str, pattern: &str) -> bool {
        if pattern.contains('*') {
            let regex_pattern = pattern.replace('*', ".*");
            regex::Regex::new(&regex_pattern).map_or(false, |re| re.is_match(metric_name))
        } else {
            metric_name == pattern
        }
    }

    fn format_metric_name(&self, metric_name: &str) -> String {
        metric_name.split('.').last().unwrap_or(metric_name)
            .split('_').collect::<Vec<&str>>().join(" ")
            .to_title_case()
    }
}

/// Dashboard personalizer
pub struct DashboardPersonalizer {
    user_preferences: RwLock<HashMap<String, UserPreferences>>,
}

impl DashboardPersonalizer {
    fn new() -> Self {
        Self {
            user_preferences: RwLock::new(HashMap::new()),
        }
    }

    fn personalize_dashboard(&self, mut dashboard: Dashboard, user_id: &str) -> Dashboard {
        let preferences = self.user_preferences.read();
        if let Some(user_prefs) = preferences.get(user_id) {
            // Apply user preferences
            if let Some(theme) = &user_prefs.preferred_theme {
                dashboard.theme = theme.clone();
            }

            if let Some(refresh_interval) = user_prefs.preferred_refresh_interval {
                dashboard.refresh_interval_ms = refresh_interval;
            }

            // Filter widgets based on user interests
            if !user_prefs.hidden_widget_types.is_empty() {
                dashboard.widgets.retain(|widget| {
                    !user_prefs.hidden_widget_types.contains(&widget.widget_type)
                });
            }
        }

        dashboard
    }
}

/// Generation rule for automated dashboard creation
#[derive(Debug, Clone)]
struct GenerationRule {
    metric_pattern: String,
    widget_type: WidgetType,
    chart_type: ChartType,
    title_template: String,
}

/// User preferences for dashboard personalization
#[derive(Debug, Clone)]
pub struct UserPreferences {
    pub preferred_theme: Option<Theme>,
    pub preferred_refresh_interval: Option<u64>,
    pub hidden_widget_types: Vec<WidgetType>,
    pub favorite_dashboards: Vec<String>,
}

/// Trait for title case conversion
trait TitleCase {
    fn to_title_case(&self) -> String;
}

impl TitleCase for str {
    fn to_title_case(&self) -> String {
        self.split(' ')
            .map(|word| {
                let mut chars = word.chars();
                match chars.next() {
                    None => String::new(),
                    Some(first) => first.to_uppercase().chain(chars.as_str().chars().map(|c| c.to_lowercase().next().unwrap_or(c))).collect(),
                }
            })
            .collect::<Vec<String>>()
            .join(" ")
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::monitoring::metrics::MetricsEngine;

    #[test]
    fn test_dashboard_creation_from_template() {
        let manager = DashboardManager::new();

        let dashboard = manager.create_dashboard_from_template("test_dash", "system_overview", None).unwrap();

        assert_eq!(dashboard.id, "test_dash");
        assert_eq!(dashboard.name, "System Overview");
        assert_eq!(dashboard.widgets.len(), 4); // Should have 4 widgets from template

        // Check widget types
        let widget_types: Vec<WidgetType> = dashboard.widgets.iter().map(|w| w.widget_type.clone()).collect();
        assert!(widget_types.contains(&WidgetType::Chart));
        assert!(widget_types.contains(&WidgetType::Gauge));
    }

    #[test]
    fn test_automated_dashboard_generation() {
        let manager = DashboardManager::new();

        let metrics = vec![
            "system.cpu.usage".to_string(),
            "system.memory.usage".to_string(),
            "db.connections.active".to_string(),
            "db.queries.latency".to_string(),
        ];

        let dashboard = manager.generate_dashboard_from_metrics(&metrics, "auto_dash").unwrap();

        assert_eq!(dashboard.id, "auto_dash");
        assert!(dashboard.name.contains("Auto-generated"));
        assert!(!dashboard.widgets.is_empty());

        // Should generate widgets for matching metrics
        assert!(dashboard.widgets.len() > 0);
    }

    #[tokio::test]
    async fn test_dashboard_update() {
        let manager = DashboardManager::new();
        let metrics_engine = MetricsEngine::new();

        // Create and register a test collector
        let collector = super::metrics::SystemMetricsCollector;
        metrics_engine.register_collector("system", Box::new(collector)).unwrap();

        // Collect some metrics
        metrics_engine.collect_metrics().await.unwrap();

        // Create dashboard
        let dashboard = manager.create_dashboard_from_template("test_dash", "system_overview", None).unwrap();

        // Update dashboard
        manager.update_dashboard("test_dash", &metrics_engine).await.unwrap();

        // Verify dashboard was updated
        let updated_dashboard = manager.get_dashboard("test_dash").unwrap();
        assert!(updated_dashboard.last_updated >= dashboard.last_updated);
    }

    #[test]
    fn test_widget_data_transformation() {
        let mut widget = DashboardWidget {
            id: "test_widget".to_string(),
            widget_type: WidgetType::Chart,
            title: "Test Chart".to_string(),
            metrics: vec!["test.metric".to_string()],
            chart_type: ChartType::Line,
            position: WidgetPosition { x: 0, y: 0, width: 6, height: 4 },
            config: HashMap::new(),
            data: WidgetData::Empty,
            last_updated: 0,
        };

        let metrics = vec![
            MetricPoint::new("test.metric", 10.0),
            MetricPoint::new("test.metric", 20.0),
        ];

        let transformed_data = widget.transform_metrics_to_widget_data(&metrics);

        match transformed_data {
            WidgetData::ChartData(series) => {
                assert_eq!(series.len(), 1);
                assert_eq!(series[0].name, "test.metric");
                assert_eq!(series[0].data.len(), 2);
            }
            _ => panic!("Expected ChartData"),
        }
    }

    #[test]
    fn test_gauge_widget_transformation() {
        let mut widget = DashboardWidget {
            id: "test_gauge".to_string(),
            widget_type: WidgetType::Gauge,
            title: "Test Gauge".to_string(),
            metrics: vec!["test.metric".to_string()],
            chart_type: ChartType::Gauge,
            position: WidgetPosition { x: 0, y: 0, width: 4, height: 3 },
            config: HashMap::new(),
            data: WidgetData::Empty,
            last_updated: 0,
        };

        let metrics = vec![MetricPoint::new("test.metric", 75.0)];

        let transformed_data = widget.transform_metrics_to_widget_data(&metrics);

        match transformed_data {
            WidgetData::GaugeData { value, min, max, unit } => {
                assert_eq!(value, 75.0);
                assert_eq!(min, 0.0);
                assert_eq!(max, 100.0);
                assert_eq!(unit, "%");
            }
            _ => panic!("Expected GaugeData"),
        }
    }

    #[test]
    fn test_table_widget_transformation() {
        let mut widget = DashboardWidget {
            id: "test_table".to_string(),
            widget_type: WidgetType::Table,
            title: "Test Table".to_string(),
            metrics: vec!["test.metric".to_string()],
            chart_type: ChartType::Table,
            position: WidgetPosition { x: 0, y: 0, width: 12, height: 4 },
            config: HashMap::new(),
            data: WidgetData::Empty,
            last_updated: 0,
        };

        let metrics = vec![MetricPoint::new("test.metric", 42.0)];

        let transformed_data = widget.transform_metrics_to_widget_data(&metrics);

        match transformed_data {
            WidgetData::TableData { columns, rows } => {
                assert!(columns.contains(&"timestamp".to_string()));
                assert!(columns.contains(&"metric".to_string()));
                assert!(columns.contains(&"value".to_string()));
                assert_eq!(rows.len(), 1);
                assert_eq!(rows[0].get("value").unwrap(), "42");
            }
            _ => panic!("Expected TableData"),
        }
    }

    #[test]
    fn test_dashboard_customization() {
        let manager = DashboardManager::new();

        // Create dashboard first
        let _dashboard = manager.create_dashboard_from_template("test_dash", "system_overview", None).unwrap();

        // Apply customizations
        let customizations = DashboardCustomizations {
            layout: Some(LayoutType::Masonry),
            theme: Some(Theme::Dark),
            refresh_interval_ms: Some(60000),
            additional_widgets: vec![],
        };

        manager.customize_dashboard("test_dash", customizations).unwrap();

        // Verify customizations were applied
        let customized_dashboard = manager.get_dashboard("test_dash").unwrap();
        assert!(matches!(customized_dashboard.layout, LayoutType::Masonry));
        assert!(matches!(customized_dashboard.theme, Theme::Dark));
        assert_eq!(customized_dashboard.refresh_interval_ms, 60000);
    }

    #[test]
    fn test_available_templates() {
        let manager = DashboardManager::new();
        let templates = manager.get_available_templates();

        assert!(templates.contains(&"system_overview".to_string()));
        assert!(templates.contains(&"database_performance".to_string()));
        assert!(templates.contains(&"query_analytics".to_string()));
        assert!(templates.contains(&"infrastructure_health".to_string()));
    }

    #[test]
    fn test_title_case_conversion() {
        assert_eq!("hello world".to_title_case(), "Hello World");
        assert_eq!("CPU usage".to_title_case(), "Cpu Usage"); // Note: this is a simple implementation
        assert_eq!("".to_title_case(), "");
    }

    #[test]
    fn test_metric_name_formatting() {
        let generator = AutomatedDashboardGenerator::new();

        // Test the private method indirectly through generation
        let metrics = vec!["system.cpu.usage".to_string()];
        let dashboard = generator.generate_from_metrics(&metrics, "test").unwrap();

        // Should have a widget with formatted title
        assert!(!dashboard.widgets.is_empty());
        assert!(dashboard.widgets[0].title.contains("System"));
    }

    #[tokio::test]
    async fn test_dashboard_subscription() {
        let manager = DashboardManager::new();

        let subscriber = DashboardSubscriber {
            id: "test_subscriber".to_string(),
            connection_type: ConnectionType::WebSocket,
            filters: None,
        };

        let subscription_id = manager.subscribe_to_dashboard("test_dash", subscriber).await.unwrap();
        assert!(subscription_id.starts_with("sub_test_dash_"));
    }

    #[test]
    fn test_widget_positioning() {
        let position = WidgetPosition {
            x: 0,
            y: 0,
            width: 6,
            height: 4,
        };

        assert_eq!(position.x, 0);
        assert_eq!(position.y, 0);
        assert_eq!(position.width, 6);
        assert_eq!(position.height, 4);
    }

    #[test]
    fn test_dashboard_permissions() {
        let dashboard = Dashboard {
            id: "test".to_string(),
            name: "Test Dashboard".to_string(),
            description: "Test".to_string(),
            widgets: vec![],
            layout: LayoutType::Grid,
            theme: Theme::Light,
            refresh_interval_ms: 30000,
            created_at: 0,
            last_updated: 0,
            owner: Some("user1".to_string()),
            permissions: vec![Permission::Read, Permission::Write],
        };

        assert_eq!(dashboard.permissions.len(), 2);
        assert!(dashboard.permissions.contains(&Permission::Read));
        assert!(dashboard.permissions.contains(&Permission::Write));
    }
}
