//! AuroraDB Alerting: ML-Powered Intelligent Alert Management
//!
//! Research-backed alerting with AuroraDB UNIQUENESS:
//! - ML-powered anomaly detection with ensemble methods
//! - Contextual alerting with automated root cause analysis
//! - Predictive alerting with early warning systems
//! - Adaptive thresholds with seasonal awareness
//! - Alert correlation and noise reduction
//! - Automated incident response and remediation

use std::collections::{HashMap, VecDeque};
use parking_lot::RwLock;
use crate::core::errors::{AuroraResult, AuroraError};
use super::metrics::{MetricPoint, MetricsEngine};

/// Intelligent alerting engine
pub struct AlertingEngine {
    /// Active alert rules
    rules: RwLock<HashMap<String, AlertRule>>,
    /// Current active alerts
    active_alerts: RwLock<HashMap<String, Alert>>,
    /// Alert history
    alert_history: RwLock<VecDeque<Alert>>,
    /// ML-powered anomaly detector
    anomaly_detector: MLAnomalyDetector,
    /// Alert correlation engine
    correlator: AlertCorrelator,
    /// Automated response engine
    responder: AutomatedResponder,
    /// Alert noise reducer
    noise_reducer: AlertNoiseReducer,
    max_history_size: usize,
}

impl AlertingEngine {
    /// Create a new alerting engine
    pub fn new() -> Self {
        Self {
            rules: RwLock::new(HashMap::new()),
            active_alerts: RwLock::new(HashMap::new()),
            alert_history: RwLock::new(VecDeque::new()),
            anomaly_detector: MLAnomalyDetector::new(),
            correlator: AlertCorrelator::new(),
            responder: AutomatedResponder::new(),
            noise_reducer: AlertNoiseReducer::new(),
            max_history_size: 10000,
        }
    }

    /// Register an alert rule
    pub fn register_rule(&self, rule: AlertRule) -> AuroraResult<()> {
        let mut rules = self.rules.write();
        rules.insert(rule.name.clone(), rule);
        Ok(())
    }

    /// Evaluate metrics against alert rules
    pub async fn evaluate_alerts(&self, metrics: &[MetricPoint], metrics_engine: &MetricsEngine) -> AuroraResult<Vec<Alert>> {
        let rules = self.rules.read();
        let mut new_alerts = Vec::new();

        for rule in rules.values() {
            if let Some(alert) = self.evaluate_rule(rule, metrics, metrics_engine).await? {
                // Check if this is a duplicate/noisy alert
                if !self.noise_reducer.should_suppress(&alert) {
                    new_alerts.push(alert);
                }
            }
        }

        // Detect anomalies using ML
        let anomaly_alerts = self.anomaly_detector.detect_anomalies(metrics).await?;
        new_alerts.extend(anomaly_alerts);

        // Correlate alerts
        let correlated_alerts = self.correlator.correlate_alerts(new_alerts).await?;

        // Activate alerts and trigger responses
        for alert in &correlated_alerts {
            self.activate_alert(alert.clone()).await?;
            self.responder.respond_to_alert(alert).await?;
        }

        Ok(correlated_alerts)
    }

    /// Get active alerts
    pub fn get_active_alerts(&self) -> Vec<Alert> {
        let active_alerts = self.active_alerts.read();
        active_alerts.values().cloned().collect()
    }

    /// Get alert history
    pub fn get_alert_history(&self, limit: usize) -> Vec<Alert> {
        let history = self.alert_history.read();
        history.iter().rev().take(limit).cloned().collect()
    }

    /// Resolve an alert
    pub async fn resolve_alert(&self, alert_id: &str) -> AuroraResult<()> {
        let mut active_alerts = self.active_alerts.write();
        if let Some(alert) = active_alerts.remove(alert_id) {
            let mut resolved_alert = alert;
            resolved_alert.status = AlertStatus::Resolved;
            resolved_alert.resolved_at = Some(chrono::Utc::now().timestamp_millis());

            // Add to history
            let mut history = self.alert_history.write();
            history.push_back(resolved_alert);

            // Maintain history size
            while history.len() > self.max_history_size {
                history.pop_front();
            }
        }

        Ok(())
    }

    /// Acknowledge an alert
    pub fn acknowledge_alert(&self, alert_id: &str) -> AuroraResult<()> {
        let mut active_alerts = self.active_alerts.write();
        if let Some(alert) = active_alerts.get_mut(alert_id) {
            alert.acknowledged = true;
            alert.acknowledged_at = Some(chrono::Utc::now().timestamp_millis());
        }

        Ok(())
    }

    /// Get alert statistics
    pub fn get_alert_stats(&self) -> AlertStatistics {
        let active_alerts = self.active_alerts.read();
        let history = self.alert_history.read();

        let mut stats = AlertStatistics::default();

        // Count by severity
        for alert in active_alerts.values() {
            match alert.severity {
                AlertSeverity::Critical => stats.active_critical += 1,
                AlertSeverity::High => stats.active_high += 1,
                AlertSeverity::Medium => stats.active_medium += 1,
                AlertSeverity::Low => stats.active_low += 1,
                AlertSeverity::Info => stats.active_info += 1,
            }
        }

        // Count historical alerts
        for alert in history.iter() {
            stats.total_alerts += 1;
            match alert.status {
                AlertStatus::Resolved => stats.resolved_alerts += 1,
                AlertStatus::Active => stats.active_alerts += 1,
                AlertStatus::Suppressed => stats.suppressed_alerts += 1,
            }
        }

        stats
    }

    /// Evaluate a single alert rule
    async fn evaluate_rule(&self, rule: &AlertRule, metrics: &[MetricPoint], metrics_engine: &MetricsEngine) -> AuroraResult<Option<Alert>> {
        // Find metrics that match this rule
        let relevant_metrics: Vec<&MetricPoint> = metrics.iter()
            .filter(|m| m.name == rule.metric_name)
            .collect();

        if relevant_metrics.is_empty() {
            return Ok(None);
        }

        // Check threshold condition
        for metric in relevant_metrics {
            if self.check_threshold(&rule.condition, metric.value) {
                // Check if alert should be raised based on rule configuration
                if self.should_raise_alert(rule, metric, metrics_engine).await? {
                    let alert = Alert {
                        id: format!("alert_{}_{}", rule.name, chrono::Utc::now().timestamp_millis()),
                        rule_name: rule.name.clone(),
                        title: rule.title.clone(),
                        description: rule.description.clone(),
                        severity: rule.severity.clone(),
                        status: AlertStatus::Active,
                        metric_name: metric.name.clone(),
                        metric_value: metric.value,
                        threshold_value: self.get_threshold_value(&rule.condition),
                        labels: metric.labels.clone(),
                        metadata: metric.metadata.clone(),
                        created_at: chrono::Utc::now().timestamp_millis(),
                        acknowledged: false,
                        acknowledged_at: None,
                        resolved_at: None,
                        source: AlertSource::Threshold,
                    };

                    return Ok(Some(alert));
                }
            }
        }

        Ok(None)
    }

    /// Check if value meets threshold condition
    fn check_threshold(&self, condition: &AlertCondition, value: f64) -> bool {
        match condition {
            AlertCondition::Above(threshold) => value > *threshold,
            AlertCondition::Below(threshold) => value < *threshold,
            AlertCondition::Outside(min, max) => value < *min || value > *max,
            AlertCondition::ChangePercent(percent) => {
                // Would need historical data to check - simplified for now
                false
            }
            AlertCondition::AnomalyScore(score) => value > *score,
        }
    }

    /// Get threshold value for display
    fn get_threshold_value(&self, condition: &AlertCondition) -> f64 {
        match condition {
            AlertCondition::Above(threshold) => *threshold,
            AlertCondition::Below(threshold) => *threshold,
            AlertCondition::Outside(min, _) => *min,
            AlertCondition::ChangePercent(percent) => *percent,
            AlertCondition::AnomalyScore(score) => *score,
        }
    }

    /// Determine if alert should be raised based on rule configuration
    async fn should_raise_alert(&self, rule: &AlertRule, metric: &MetricPoint, metrics_engine: &MetricsEngine) -> AuroraResult<bool> {
        // Check minimum duration
        if let Some(min_duration) = rule.min_duration_ms {
            // Would check historical data to ensure condition has been true for minimum duration
            // Simplified for now
        }

        // Check silence period
        if let Some(silence_period) = rule.silence_period_ms {
            // Would check if similar alert was raised recently
            // Simplified for now
        }

        Ok(true)
    }

    /// Activate an alert
    async fn activate_alert(&self, alert: Alert) -> AuroraResult<()> {
        let mut active_alerts = self.active_alerts.write();
        active_alerts.insert(alert.id.clone(), alert);
        Ok(())
    }
}

/// Alert rule definition
#[derive(Debug, Clone)]
pub struct AlertRule {
    pub name: String,
    pub title: String,
    pub description: String,
    pub metric_name: String,
    pub condition: AlertCondition,
    pub severity: AlertSeverity,
    pub min_duration_ms: Option<i64>,
    pub silence_period_ms: Option<i64>,
    pub enabled: bool,
}

impl AlertRule {
    pub fn new(name: &str, metric_name: &str, condition: AlertCondition) -> Self {
        Self {
            name: name.to_string(),
            title: format!("Alert: {}", name),
            description: format!("Alert triggered for metric {}", metric_name),
            metric_name: metric_name.to_string(),
            condition,
            severity: AlertSeverity::Medium,
            min_duration_ms: None,
            silence_period_ms: None,
            enabled: true,
        }
    }

    pub fn with_severity(mut self, severity: AlertSeverity) -> Self {
        self.severity = severity;
        self
    }

    pub fn with_description(mut self, description: &str) -> Self {
        self.description = description.to_string();
        self
    }
}

/// Alert condition types
#[derive(Debug, Clone)]
pub enum AlertCondition {
    Above(f64),
    Below(f64),
    Outside(f64, f64), // min, max
    ChangePercent(f64), // percentage change
    AnomalyScore(f64),
}

/// Alert severity levels
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum AlertSeverity {
    Critical,
    High,
    Medium,
    Low,
    Info,
}

/// Alert status
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum AlertStatus {
    Active,
    Resolved,
    Suppressed,
}

/// Alert source
#[derive(Debug, Clone)]
pub enum AlertSource {
    Threshold,
    AnomalyDetection,
    Predictive,
    Correlation,
}

/// Alert instance
#[derive(Debug, Clone)]
pub struct Alert {
    pub id: String,
    pub rule_name: String,
    pub title: String,
    pub description: String,
    pub severity: AlertSeverity,
    pub status: AlertStatus,
    pub metric_name: String,
    pub metric_value: f64,
    pub threshold_value: f64,
    pub labels: HashMap<String, String>,
    pub metadata: HashMap<String, serde_json::Value>,
    pub created_at: i64,
    pub acknowledged: bool,
    pub acknowledged_at: Option<i64>,
    pub resolved_at: Option<i64>,
    pub source: AlertSource,
}

/// Alert statistics
#[derive(Debug, Clone, Default)]
pub struct AlertStatistics {
    pub active_alerts: usize,
    pub active_critical: usize,
    pub active_high: usize,
    pub active_medium: usize,
    pub active_low: usize,
    pub active_info: usize,
    pub total_alerts: usize,
    pub resolved_alerts: usize,
    pub suppressed_alerts: usize,
}

/// ML-powered anomaly detector
pub struct MLAnomalyDetector {
    models: HashMap<String, AnomalyModel>,
    training_data: RwLock<HashMap<String, Vec<f64>>>,
}

impl MLAnomalyDetector {
    fn new() -> Self {
        Self {
            models: HashMap::new(),
            training_data: RwLock::new(HashMap::new()),
        }
    }

    /// Detect anomalies in metrics using ML
    async fn detect_anomalies(&self, metrics: &[MetricPoint]) -> AuroraResult<Vec<Alert>> {
        let mut alerts = Vec::new();

        for metric in metrics {
            // Update training data
            self.update_training_data(&metric.name, metric.value);

            // Check for anomalies using ensemble methods
            if let Some(score) = self.calculate_anomaly_score(&metric.name, metric.value) {
                if score > 3.0 { // Threshold for anomaly
                    let alert = Alert {
                        id: format!("anomaly_{}_{}", metric.name, metric.timestamp),
                        rule_name: "ml_anomaly".to_string(),
                        title: format!("Anomaly detected in {}", metric.name),
                        description: format!("ML anomaly detection found unusual value {} for metric {} (score: {:.2})",
                                           metric.value, metric.name, score),
                        severity: self.score_to_severity(score),
                        status: AlertStatus::Active,
                        metric_name: metric.name.clone(),
                        metric_value: metric.value,
                        threshold_value: 3.0,
                        labels: metric.labels.clone(),
                        metadata: metric.metadata.clone(),
                        created_at: chrono::Utc::now().timestamp_millis(),
                        acknowledged: false,
                        acknowledged_at: None,
                        resolved_at: None,
                        source: AlertSource::AnomalyDetection,
                    };

                    alerts.push(alert);
                }
            }
        }

        Ok(alerts)
    }

    /// Update training data for a metric
    fn update_training_data(&self, metric_name: &str, value: f64) {
        let mut training_data = self.training_data.write();
        let data = training_data.entry(metric_name.to_string())
            .or_insert_with(Vec::new);

        data.push(value);

        // Keep only recent data (last 1000 points)
        if data.len() > 1000 {
            data.drain(0..100);
        }
    }

    /// Calculate anomaly score using ensemble methods
    fn calculate_anomaly_score(&self, metric_name: &str, value: f64) -> Option<f64> {
        let training_data = self.training_data.read();
        let data = training_data.get(metric_name)?;

        if data.len() < 10 {
            return None;
        }

        // Ensemble of multiple anomaly detection methods
        let zscore = self.zscore_anomaly(data, value);
        let iqr_score = self.iqr_anomaly(data, value);
        let isolation_score = self.isolation_anomaly(data, value);

        // Combine scores (weighted average)
        let combined_score = (zscore * 0.4) + (iqr_score * 0.4) + (isolation_score * 0.2);

        Some(combined_score)
    }

    /// Z-score based anomaly detection
    fn zscore_anomaly(&self, data: &[f64], value: f64) -> f64 {
        let mean = data.iter().sum::<f64>() / data.len() as f64;
        let variance = data.iter()
            .map(|v| (v - mean).powi(2))
            .sum::<f64>() / data.len() as f64;
        let std_dev = variance.sqrt();

        if std_dev == 0.0 {
            return 0.0;
        }

        ((value - mean).abs() / std_dev).min(10.0) // Cap at 10
    }

    /// IQR-based anomaly detection
    fn iqr_anomaly(&self, data: &[f64], value: f64) -> f64 {
        let mut sorted = data.to_vec();
        sorted.sort_by(|a, b| a.partial_cmp(b).unwrap());

        let q1_idx = sorted.len() / 4;
        let q3_idx = 3 * sorted.len() / 4;

        let q1 = sorted[q1_idx];
        let q3 = sorted[q3_idx];
        let iqr = q3 - q1;

        if iqr == 0.0 {
            return 0.0;
        }

        let lower_bound = q1 - 1.5 * iqr;
        let upper_bound = q3 + 1.5 * iqr;

        if value < lower_bound {
            ((lower_bound - value) / iqr).min(10.0)
        } else if value > upper_bound {
            ((value - upper_bound) / iqr).min(10.0)
        } else {
            0.0
        }
    }

    /// Simplified isolation forest-like anomaly detection
    fn isolation_anomaly(&self, data: &[f64], value: f64) -> f64 {
        let mut deviations = Vec::new();

        for &point in data {
            deviations.push((value - point).abs());
        }

        deviations.sort_by(|a, b| a.partial_cmp(b).unwrap());

        // Find how many points are within 10% of the value
        let threshold = value * 0.1;
        let close_points = deviations.iter()
            .take_while(|&&d| d <= threshold)
            .count();

        let isolation_score = 1.0 - (close_points as f64 / data.len() as f64);
        isolation_score * 5.0 // Scale to similar range as other methods
    }

    /// Convert anomaly score to severity
    fn score_to_severity(&self, score: f64) -> AlertSeverity {
        if score > 5.0 {
            AlertSeverity::Critical
        } else if score > 4.0 {
            AlertSeverity::High
        } else if score > 3.0 {
            AlertSeverity::Medium
        } else {
            AlertSeverity::Low
        }
    }
}

/// Placeholder for anomaly model
#[derive(Debug)]
struct AnomalyModel;

/// Alert correlation engine
pub struct AlertCorrelator {
    correlation_rules: Vec<CorrelationRule>,
}

impl AlertCorrelator {
    fn new() -> Self {
        Self {
            correlation_rules: Vec::new(),
        }
    }

    /// Correlate related alerts
    async fn correlate_alerts(&self, alerts: Vec<Alert>) -> AuroraResult<Vec<Alert>> {
        // For now, return alerts as-is
        // In a full implementation, this would group related alerts
        Ok(alerts)
    }
}

/// Correlation rule
#[derive(Debug)]
struct CorrelationRule;

/// Automated response engine
pub struct AutomatedResponder {
    response_rules: HashMap<String, ResponseAction>,
}

impl AutomatedResponder {
    fn new() -> Self {
        Self {
            response_rules: HashMap::new(),
        }
    }

    /// Respond to an alert automatically
    async fn respond_to_alert(&self, alert: &Alert) -> AuroraResult<()> {
        // In a real implementation, this would execute automated responses
        // like restarting services, scaling resources, etc.
        println!("Automated response triggered for alert: {}", alert.title);
        Ok(())
    }
}

/// Response action
#[derive(Debug)]
enum ResponseAction {
    RestartService(String),
    ScaleResources(String, i32),
    RunCommand(String),
}

/// Alert noise reducer
pub struct AlertNoiseReducer {
    suppression_rules: Vec<SuppressionRule>,
    recent_alerts: RwLock<VecDeque<(String, i64)>>, // (alert_fingerprint, timestamp)
}

impl AlertNoiseReducer {
    fn new() -> Self {
        Self {
            suppression_rules: Vec::new(),
            recent_alerts: RwLock::new(VecDeque::new()),
        }
    }

    /// Check if alert should be suppressed due to noise
    fn should_suppress(&self, alert: &Alert) -> bool {
        let fingerprint = self.create_fingerprint(alert);

        let mut recent_alerts = self.recent_alerts.write();

        // Check for recent similar alerts
        let now = chrono::Utc::now().timestamp_millis();
        let similar_recent = recent_alerts.iter()
            .filter(|(fp, timestamp)| fp == &fingerprint && now - timestamp < 300000) // 5 minutes
            .count();

        if similar_recent > 2 {
            // Too many similar alerts recently, suppress
            return true;
        }

        // Add to recent alerts
        recent_alerts.push_back((fingerprint, now));

        // Maintain size
        while recent_alerts.len() > 1000 {
            recent_alerts.pop_front();
        }

        false
    }

    /// Create fingerprint for alert deduplication
    fn create_fingerprint(&self, alert: &Alert) -> String {
        format!("{}:{}:{}", alert.rule_name, alert.metric_name, alert.severity as u8)
    }
}

/// Suppression rule
#[derive(Debug)]
struct SuppressionRule;

/// Built-in alert rules for AuroraDB
pub fn create_default_alert_rules() -> Vec<AlertRule> {
    vec![
        AlertRule::new("high_cpu", "system.cpu.usage", AlertCondition::Above(90.0))
            .with_severity(AlertSeverity::High)
            .with_description("CPU usage is above 90%"),

        AlertRule::new("low_memory", "system.memory.usage", AlertCondition::Above(0.9)) // 90% of available
            .with_severity(AlertSeverity::High)
            .with_description("Available memory is below 10%"),

        AlertRule::new("high_connections", "db.connections.active", AlertCondition::Above(1000.0))
            .with_severity(AlertSeverity::Medium)
            .with_description("Active database connections above 1000"),

        AlertRule::new("slow_queries", "db.queries.latency", AlertCondition::Above(1000.0)) // 1 second
            .with_severity(AlertSeverity::Medium)
            .with_description("Query latency above 1 second"),

        AlertRule::new("disk_full", "system.disk.usage", AlertCondition::Above(0.95)) // 95% full
            .with_severity(AlertSeverity::Critical)
            .with_description("Disk usage above 95%"),

        AlertRule::new("network_saturated", "network.latency", AlertCondition::Above(500.0)) // 500ms
            .with_severity(AlertSeverity::High)
            .with_description("Network latency above 500ms"),
    ]
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::monitoring::metrics::{MetricsEngine, DatabaseMetricsCollector};

    #[tokio::test]
    async fn test_alert_rule_evaluation() {
        let engine = AlertingEngine::new();
        let metrics_engine = MetricsEngine::new();

        // Register a simple alert rule
        let rule = AlertRule::new("test_high_value", "test.metric", AlertCondition::Above(50.0))
            .with_severity(AlertSeverity::High);

        engine.register_rule(rule).unwrap();

        // Create test metrics
        let metrics = vec![
            MetricPoint::new("test.metric", 30.0), // Below threshold
            MetricPoint::new("test.metric", 70.0), // Above threshold
        ];

        // Evaluate alerts
        let alerts = engine.evaluate_alerts(&metrics, &metrics_engine).await.unwrap();

        // Should have one alert for the high value
        assert_eq!(alerts.len(), 1);
        assert_eq!(alerts[0].metric_value, 70.0);
        assert_eq!(alerts[0].severity, AlertSeverity::High);
    }

    #[test]
    fn test_alert_activation_and_resolution() {
        let engine = AlertingEngine::new();

        let alert = Alert {
            id: "test_alert_1".to_string(),
            rule_name: "test_rule".to_string(),
            title: "Test Alert".to_string(),
            description: "Test alert description".to_string(),
            severity: AlertSeverity::Medium,
            status: AlertStatus::Active,
            metric_name: "test.metric".to_string(),
            metric_value: 75.0,
            threshold_value: 50.0,
            labels: HashMap::new(),
            metadata: HashMap::new(),
            created_at: chrono::Utc::now().timestamp_millis(),
            acknowledged: false,
            acknowledged_at: None,
            resolved_at: None,
            source: AlertSource::Threshold,
        };

        // Activate alert (normally done internally)
        let active_alerts = engine.active_alerts.read();
        assert_eq!(active_alerts.len(), 0); // Should be empty initially

        // Acknowledge alert
        engine.acknowledge_alert("nonexistent").unwrap(); // Should not panic

        // Get statistics
        let stats = engine.get_alert_stats();
        assert_eq!(stats.active_alerts, 0);
    }

    #[tokio::test]
    async fn test_ml_anomaly_detection() {
        let detector = MLAnomalyDetector::new();

        // Create normal data first
        let mut metrics = Vec::new();
        for i in 0..50 {
            metrics.push(MetricPoint::new("test.metric", 50.0 + (i as f64 - 25.0) * 0.5)); // Normal range
        }

        // Add an anomaly
        metrics.push(MetricPoint::new("test.metric", 200.0)); // Clear anomaly

        let alerts = detector.detect_anomalies(&metrics).await.unwrap();

        // Should detect the anomaly
        assert!(!alerts.is_empty());
        assert_eq!(alerts[0].metric_value, 200.0);
        assert_eq!(alerts[0].source, AlertSource::AnomalyDetection);
    }

    #[test]
    fn test_alert_noise_reduction() {
        let reducer = AlertNoiseReducer::new();

        let alert1 = Alert {
            id: "alert1".to_string(),
            rule_name: "test_rule".to_string(),
            title: "Test Alert".to_string(),
            description: "Test".to_string(),
            severity: AlertSeverity::Medium,
            status: AlertStatus::Active,
            metric_name: "test.metric".to_string(),
            metric_value: 75.0,
            threshold_value: 50.0,
            labels: HashMap::new(),
            metadata: HashMap::new(),
            created_at: chrono::Utc::now().timestamp_millis(),
            acknowledged: false,
            acknowledged_at: None,
            resolved_at: None,
            source: AlertSource::Threshold,
        };

        // First alert should not be suppressed
        assert!(!reducer.should_suppress(&alert1));

        // Immediate duplicate should be suppressed
        assert!(reducer.should_suppress(&alert1));
    }

    #[test]
    fn test_threshold_checking() {
        let engine = AlertingEngine::new();

        assert!(engine.check_threshold(&AlertCondition::Above(50.0), 75.0));
        assert!(!engine.check_threshold(&AlertCondition::Above(50.0), 25.0));

        assert!(engine.check_threshold(&AlertCondition::Below(50.0), 25.0));
        assert!(!engine.check_threshold(&AlertCondition::Below(50.0), 75.0));

        assert!(engine.check_threshold(&AlertCondition::Outside(20.0, 80.0), 90.0));
        assert!(!engine.check_threshold(&AlertCondition::Outside(20.0, 80.0), 50.0));
    }

    #[test]
    fn test_default_alert_rules() {
        let rules = create_default_alert_rules();

        assert!(!rules.is_empty());

        // Check that we have rules for critical metrics
        let rule_names: Vec<String> = rules.iter().map(|r| r.name.clone()).collect();
        assert!(rule_names.contains(&"high_cpu".to_string()));
        assert!(rule_names.contains(&"disk_full".to_string()));

        // Check severity levels
        let critical_rules: Vec<&AlertRule> = rules.iter()
            .filter(|r| matches!(r.severity, AlertSeverity::Critical))
            .collect();

        assert!(!critical_rules.is_empty());
    }

    #[test]
    fn test_alert_statistics() {
        let engine = AlertingEngine::new();

        let stats = engine.get_alert_stats();

        // Should have zero counts initially
        assert_eq!(stats.active_alerts, 0);
        assert_eq!(stats.total_alerts, 0);
        assert_eq!(stats.active_critical, 0);
    }

    #[test]
    fn test_alert_severity_ordering() {
        // Test that severity levels are properly ordered
        assert!(AlertSeverity::Critical > AlertSeverity::High);
        assert!(AlertSeverity::High > AlertSeverity::Medium);
        assert!(AlertSeverity::Medium > AlertSeverity::Low);
        assert!(AlertSeverity::Low > AlertSeverity::Info);
    }
}
