//! Performance Monitor: Trigger Performance Tracking and Optimization
//!
//! Comprehensive performance monitoring for triggers with metrics collection,
//! alerting, and optimization recommendations.

use std::collections::HashMap;
use parking_lot::RwLock;
use chrono::{DateTime, Utc, Duration};
use crate::core::errors::{AuroraResult, AuroraError};
use super::trigger_manager::TriggerStats;

/// Performance metrics for triggers
#[derive(Debug, Clone)]
pub struct PerformanceMetrics {
    pub execution_time_ms: f64,
    pub memory_used_mb: f64,
    pub cpu_utilization: f64,
    pub timestamp: DateTime<Utc>,
}

/// Performance alert for triggers
#[derive(Debug, Clone)]
pub struct PerformanceAlert {
    pub trigger_name: String,
    pub alert_type: AlertType,
    pub severity: AlertSeverity,
    pub message: String,
    pub timestamp: DateTime<Utc>,
    pub metrics: HashMap<String, f64>,
    pub recommendations: Vec<String>,
}

/// Alert types for triggers
#[derive(Debug, Clone, PartialEq)]
pub enum AlertType {
    SlowExecution,
    HighMemoryUsage,
    TriggerTimeout,
    CascadeFailure,
    PerformanceDegradation,
    ResourceExhaustion,
}

/// Alert severity levels
#[derive(Debug, Clone, PartialEq, PartialOrd)]
pub enum AlertSeverity {
    Low,
    Medium,
    High,
    Critical,
}

/// Performance threshold for triggers
#[derive(Debug, Clone)]
pub struct PerformanceThreshold {
    pub metric: String,
    pub operator: ThresholdOperator,
    pub value: f64,
    pub severity: AlertSeverity,
    pub consecutive_occurrences: u32,
}

/// Threshold operators
#[derive(Debug, Clone, PartialEq)]
pub enum ThresholdOperator {
    GreaterThan,
    LessThan,
    Equals,
}

/// Optimization recommendation
#[derive(Debug, Clone)]
pub struct OptimizationRecommendation {
    pub trigger_name: String,
    pub recommendation_type: RecommendationType,
    pub description: String,
    pub expected_improvement: f64,
    pub implementation_effort: EffortLevel,
    pub priority: Priority,
}

/// Recommendation types
#[derive(Debug, Clone, PartialEq)]
pub enum RecommendationType {
    ChangeExecutionMode,
    AddConditions,
    OptimizeCode,
    UseDeferredExecution,
    ImplementCaching,
    AddResourceLimits,
}

/// Effort levels for implementation
#[derive(Debug, Clone, PartialEq, PartialOrd)]
pub enum EffortLevel {
    Low,
    Medium,
    High,
}

/// Priority levels
#[derive(Debug, Clone, PartialEq, PartialOrd)]
pub enum Priority {
    Low,
    Medium,
    High,
    Critical,
}

/// Performance report
#[derive(Debug)]
pub struct PerformanceReport {
    pub trigger_name: String,
    pub period_start: DateTime<Utc>,
    pub period_end: DateTime<Utc>,
    pub total_executions: u64,
    pub avg_execution_time_ms: f64,
    pub p95_execution_time_ms: f64,
    pub p99_execution_time_ms: f64,
    pub error_rate: f64,
    pub alerts_count: u64,
    pub recommendations: Vec<OptimizationRecommendation>,
}

/// Trigger performance monitor
pub struct TriggerPerformanceMonitor {
    metrics_history: RwLock<HashMap<String, Vec<PerformanceMetrics>>>,
    alerts: RwLock<Vec<PerformanceAlert>>,
    thresholds: HashMap<String, PerformanceThreshold>,
    alert_counters: RwLock<HashMap<String, u32>>, // Track consecutive occurrences
}

impl TriggerPerformanceMonitor {
    pub fn new() -> Self {
        let mut thresholds = HashMap::new();

        // Default performance thresholds
        thresholds.insert("slow_execution".to_string(), PerformanceThreshold {
            metric: "execution_time_ms".to_string(),
            operator: ThresholdOperator::GreaterThan,
            value: 1000.0, // 1 second
            severity: AlertSeverity::Medium,
            consecutive_occurrences: 3,
        });

        thresholds.insert("high_memory".to_string(), PerformanceThreshold {
            metric: "memory_used_mb".to_string(),
            operator: ThresholdOperator::GreaterThan,
            value: 500.0, // 500MB
            severity: AlertSeverity::High,
            consecutive_occurrences: 2,
        });

        thresholds.insert("high_cpu".to_string(), PerformanceThreshold {
            metric: "cpu_utilization".to_string(),
            operator: ThresholdOperator::GreaterThan,
            value: 80.0, // 80%
            severity: AlertSeverity::Medium,
            consecutive_occurrences: 5,
        });

        Self {
            metrics_history: RwLock::new(HashMap::new()),
            alerts: RwLock::new(Vec::new()),
            thresholds,
            alert_counters: RwLock::new(HashMap::new()),
        }
    }

    /// Register a trigger for performance monitoring
    pub async fn register_trigger(&self, trigger_name: &str) -> AuroraResult<()> {
        let mut metrics_history = self.metrics_history.write();
        metrics_history.entry(trigger_name.to_string()).or_insert_with(Vec::new);
        Ok(())
    }

    /// Record performance metrics for a trigger execution
    pub async fn record_execution(&self, trigger_name: &str, metrics: &PerformanceMetrics) -> AuroraResult<()> {
        // Store metrics
        {
            let mut metrics_history = self.metrics_history.write();
            let trigger_metrics = metrics_history.entry(trigger_name.to_string()).or_insert_with(Vec::new);
            trigger_metrics.push(metrics.clone());

            // Keep only last 1000 measurements per trigger
            if trigger_metrics.len() > 1000 {
                trigger_metrics.remove(0);
            }
        }

        // Check thresholds and generate alerts
        self.check_thresholds(trigger_name, metrics).await?;

        Ok(())
    }

    /// Get performance statistics for a trigger
    pub async fn get_trigger_stats(&self, trigger_name: &str) -> TriggerStats {
        let metrics_history = self.metrics_history.read();

        if let Some(metrics) = metrics_history.get(trigger_name) {
            if metrics.is_empty() {
                return TriggerStats::default();
            }

            let total_executions = metrics.len() as u64;
            let successful_executions = total_executions; // Simplified - would track errors separately
            let failed_executions = 0; // Simplified

            let execution_times: Vec<f64> = metrics.iter().map(|m| m.execution_time_ms).collect();
            let avg_execution_time = execution_times.iter().sum::<f64>() / execution_times.len() as f64;
            let max_execution_time = execution_times.iter().fold(0.0, |max, &val| if val > max { val } else { max });
            let min_execution_time = execution_times.iter().fold(f64::INFINITY, |min, &val| if val < min { val } else { min });

            let total_memory = metrics.iter().map(|m| m.memory_used_mb).sum::<f64>();
            let last_executed = metrics.last().map(|m| m.timestamp);

            // Calculate percentiles
            let mut sorted_times = execution_times.clone();
            sorted_times.sort_by(|a, b| a.partial_cmp(b).unwrap());
            let p95_index = (sorted_times.len() as f64 * 0.95) as usize;
            let p99_index = (sorted_times.len() as f64 * 0.99) as usize;

            let p95_execution_time = sorted_times.get(p95_index).copied().unwrap_or(max_execution_time);
            let p99_execution_time = sorted_times.get(p99_index).copied().unwrap_or(max_execution_time);

            TriggerStats {
                total_executions,
                successful_executions,
                failed_executions,
                avg_execution_time_ms: avg_execution_time,
                max_execution_time_ms: max_execution_time,
                min_execution_time_ms: min_execution_time,
                last_executed,
            }
        } else {
            TriggerStats::default()
        }
    }

    /// Remove trigger from monitoring
    pub async fn remove_trigger(&self, trigger_name: &str) -> AuroraResult<()> {
        let mut metrics_history = self.metrics_history.write();
        metrics_history.remove(trigger_name);

        let mut alert_counters = self.alert_counters.write();
        alert_counters.remove(trigger_name);

        Ok(())
    }

    /// Get performance alerts
    pub async fn get_alerts(&self, trigger_name: Option<&str>, limit: usize) -> Vec<PerformanceAlert> {
        let alerts = self.alerts.read();
        let filtered: Vec<&PerformanceAlert> = alerts.iter()
            .filter(|alert| {
                trigger_name.map_or(true, |name| alert.trigger_name == name)
            })
            .collect();

        filtered.into_iter().rev().take(limit).map(|a| a.clone()).collect()
    }

    /// Generate performance report
    pub async fn generate_report(&self, trigger_name: &str) -> AuroraResult<PerformanceReport> {
        let stats = self.get_trigger_stats(trigger_name).await;
        let alerts = self.get_alerts(Some(trigger_name), 100).await;
        let recommendations = self.generate_recommendations(trigger_name, &stats).await?;

        let period_end = Utc::now();
        let period_start = period_end - Duration::hours(24); // Last 24 hours

        Ok(PerformanceReport {
            trigger_name: trigger_name.to_string(),
            period_start,
            period_end,
            total_executions: stats.total_executions,
            avg_execution_time_ms: stats.avg_execution_time_ms,
            p95_execution_time_ms: 0.0, // Would calculate from metrics
            p99_execution_time_ms: 0.0, // Would calculate from metrics
            error_rate: stats.failed_executions as f64 / stats.total_executions as f64,
            alerts_count: alerts.len() as u64,
            recommendations,
        })
    }

    /// Get optimization recommendations
    pub async fn generate_recommendations(&self, trigger_name: &str, stats: &TriggerStats) -> AuroraResult<Vec<OptimizationRecommendation>> {
        let mut recommendations = Vec::new();

        // Slow execution recommendation
        if stats.avg_execution_time_ms > 500.0 {
            recommendations.push(OptimizationRecommendation {
                trigger_name: trigger_name.to_string(),
                recommendation_type: RecommendationType::ChangeExecutionMode,
                description: "Consider using asynchronous execution mode to avoid blocking operations".to_string(),
                expected_improvement: 0.3, // 30% improvement
                implementation_effort: EffortLevel::Low,
                priority: Priority::Medium,
            });
        }

        // High variance recommendation
        if stats.max_execution_time_ms > stats.avg_execution_time_ms * 3.0 {
            recommendations.push(OptimizationRecommendation {
                trigger_name: trigger_name.to_string(),
                recommendation_type: RecommendationType::AddConditions,
                description: "Add conditions to filter out unnecessary trigger executions".to_string(),
                expected_improvement: 0.4, // 40% improvement
                implementation_effort: EffortLevel::Medium,
                priority: Priority::High,
            });
        }

        // High execution frequency recommendation
        if stats.total_executions > 1000 && stats.avg_execution_time_ms > 100.0 {
            recommendations.push(OptimizationRecommendation {
                trigger_name: trigger_name.to_string(),
                recommendation_type: RecommendationType::UseDeferredExecution,
                description: "Use deferred execution to batch trigger processing".to_string(),
                expected_improvement: 0.5, // 50% improvement
                implementation_effort: EffortLevel::High,
                priority: Priority::High,
            });
        }

        // Error rate recommendation
        let error_rate = stats.failed_executions as f64 / stats.total_executions as f64;
        if error_rate > 0.1 {
            recommendations.push(OptimizationRecommendation {
                trigger_name: trigger_name.to_string(),
                recommendation_type: RecommendationType::OptimizeCode,
                description: "Review and optimize trigger code to reduce error rate".to_string(),
                expected_improvement: 0.2, // 20% improvement
                implementation_effort: EffortLevel::High,
                priority: Priority::Critical,
            });
        }

        Ok(recommendations)
    }

    /// Add custom performance threshold
    pub fn add_threshold(&mut self, name: String, threshold: PerformanceThreshold) {
        self.thresholds.insert(name, threshold);
    }

    /// Get performance metrics history
    pub fn get_metrics_history(&self, trigger_name: &str, limit: usize) -> Vec<PerformanceMetrics> {
        let metrics_history = self.metrics_history.read();
        if let Some(metrics) = metrics_history.get(trigger_name) {
            metrics.iter().rev().take(limit).cloned().collect()
        } else {
            vec![]
        }
    }

    /// Clear old metrics (older than specified duration)
    pub fn clear_old_metrics(&self, max_age: Duration) {
        let cutoff = Utc::now() - max_age;
        let mut metrics_history = self.metrics_history.write();

        for metrics in metrics_history.values_mut() {
            metrics.retain(|m| m.timestamp > cutoff);
        }
    }

    // Private methods

    async fn check_thresholds(&self, trigger_name: &str, metrics: &PerformanceMetrics) -> AuroraResult<()> {
        for (threshold_name, threshold) in &self.thresholds {
            let metric_value = match metrics.get_value(&threshold.metric) {
                Some(value) => value,
                None => continue,
            };

            let threshold_breached = match threshold.operator {
                ThresholdOperator::GreaterThan => metric_value > threshold.value,
                ThresholdOperator::LessThan => metric_value < threshold.value,
                ThresholdOperator::Equals => (metric_value - threshold.value).abs() < f64::EPSILON,
            };

            let counter_key = format!("{}_{}", trigger_name, threshold_name);
            let mut alert_counters = self.alert_counters.write();

            if threshold_breached {
                let counter = alert_counters.entry(counter_key.clone()).or_insert(0);
                *counter += 1;

                if *counter >= threshold.consecutive_occurrences {
                    // Generate alert
                    let alert = PerformanceAlert {
                        trigger_name: trigger_name.to_string(),
                        alert_type: match threshold_name.as_str() {
                            "slow_execution" => AlertType::SlowExecution,
                            "high_memory" => AlertType::HighMemoryUsage,
                            "high_cpu" => AlertType::PerformanceDegradation,
                            _ => AlertType::PerformanceDegradation,
                        },
                        severity: threshold.severity.clone(),
                        message: format!("{} exceeded threshold: {:.2} > {:.2} ({} consecutive)",
                                       threshold.metric, metric_value, threshold.value, *counter),
                        timestamp: Utc::now(),
                        metrics: HashMap::from([
                            (threshold.metric.clone(), metric_value),
                            ("threshold".to_string(), threshold.value),
                            ("consecutive".to_string(), *counter as f64),
                        ]),
                        recommendations: vec!["Review trigger performance".to_string()],
                    };

                    let mut alerts = self.alerts.write();
                    alerts.push(alert);

                    // Reset counter after alert
                    *counter = 0;
                }
            } else {
                // Reset counter when threshold is no longer breached
                alert_counters.remove(&counter_key);
            }
        }

        Ok(())
    }
}

impl PerformanceMetrics {
    pub fn get_value(&self, metric: &str) -> Option<f64> {
        match metric {
            "execution_time_ms" => Some(self.execution_time_ms),
            "memory_used_mb" => Some(self.memory_used_mb),
            "cpu_utilization" => Some(self.cpu_utilization),
            _ => None,
        }
    }
}

impl Default for TriggerStats {
    fn default() -> Self {
        Self {
            total_executions: 0,
            successful_executions: 0,
            failed_executions: 0,
            avg_execution_time_ms: 0.0,
            max_execution_time_ms: 0.0,
            min_execution_time_ms: f64::INFINITY,
            last_executed: None,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn test_performance_monitor_creation() {
        let monitor = TriggerPerformanceMonitor::new();
        assert!(true); // Passes if created successfully
    }

    #[tokio::test]
    async fn test_trigger_registration() {
        let monitor = TriggerPerformanceMonitor::new();

        monitor.register_trigger("test_trigger").await.unwrap();

        let stats = monitor.get_trigger_stats("test_trigger").await;
        assert_eq!(stats.total_executions, 0);
    }

    #[tokio::test]
    async fn test_performance_recording() {
        let monitor = TriggerPerformanceMonitor::new();

        monitor.register_trigger("test_trigger").await.unwrap();

        let metrics = PerformanceMetrics {
            execution_time_ms: 150.0,
            memory_used_mb: 25.0,
            cpu_utilization: 45.0,
            timestamp: Utc::now(),
        };

        monitor.record_execution("test_trigger", &metrics).await.unwrap();

        let stats = monitor.get_trigger_stats("test_trigger").await;
        assert_eq!(stats.total_executions, 1);
        assert_eq!(stats.avg_execution_time_ms, 150.0);
    }

    #[test]
    fn test_alert_types() {
        assert_eq!(AlertType::SlowExecution, AlertType::SlowExecution);
        assert_ne!(AlertType::HighMemoryUsage, AlertType::CascadeFailure);
    }

    #[test]
    fn test_alert_severity() {
        assert!(AlertSeverity::Low < AlertSeverity::Critical);
        assert!(AlertSeverity::Medium > AlertSeverity::Low);
    }

    #[test]
    fn test_threshold_operators() {
        assert_eq!(ThresholdOperator::GreaterThan, ThresholdOperator::GreaterThan);
        assert_ne!(ThresholdOperator::LessThan, ThresholdOperator::Equals);
    }

    #[test]
    fn test_recommendation_types() {
        assert_eq!(RecommendationType::ChangeExecutionMode, RecommendationType::ChangeExecutionMode);
        assert_ne!(RecommendationType::AddConditions, RecommendationType::OptimizeCode);
    }

    #[test]
    fn test_effort_levels() {
        assert!(EffortLevel::Low < EffortLevel::High);
        assert!(EffortLevel::Medium > EffortLevel::Low);
    }

    #[test]
    fn test_priority_levels() {
        assert!(Priority::Low < Priority::Critical);
        assert!(Priority::Medium > Priority::Low);
    }

    #[test]
    fn test_performance_metrics_get_value() {
        let metrics = PerformanceMetrics {
            execution_time_ms: 150.0,
            memory_used_mb: 25.0,
            cpu_utilization: 45.0,
            timestamp: Utc::now(),
        };

        assert_eq!(metrics.get_value("execution_time_ms"), Some(150.0));
        assert_eq!(metrics.get_value("memory_used_mb"), Some(25.0));
        assert_eq!(metrics.get_value("cpu_utilization"), Some(45.0));
        assert_eq!(metrics.get_value("unknown_metric"), None);
    }

    #[test]
    fn test_trigger_stats_default() {
        let stats = TriggerStats::default();
        assert_eq!(stats.total_executions, 0);
        assert_eq!(stats.successful_executions, 0);
        assert_eq!(stats.failed_executions, 0);
        assert_eq!(stats.avg_execution_time_ms, 0.0);
    }

    #[tokio::test]
    async fn test_metrics_history() {
        let monitor = TriggerPerformanceMonitor::new();

        monitor.register_trigger("test_trigger").await.unwrap();

        let metrics = PerformanceMetrics {
            execution_time_ms: 100.0,
            memory_used_mb: 20.0,
            cpu_utilization: 30.0,
            timestamp: Utc::now(),
        };

        monitor.record_execution("test_trigger", &metrics).await.unwrap();

        let history = monitor.get_metrics_history("test_trigger", 10);
        assert_eq!(history.len(), 1);
        assert_eq!(history[0].execution_time_ms, 100.0);
    }

    #[tokio::test]
    async fn test_performance_thresholds() {
        let mut monitor = TriggerPerformanceMonitor::new();

        // Add custom threshold
        let threshold = PerformanceThreshold {
            metric: "execution_time_ms".to_string(),
            operator: ThresholdOperator::GreaterThan,
            value: 50.0,
            severity: AlertSeverity::Low,
            consecutive_occurrences: 1,
        };

        monitor.add_threshold("custom_threshold".to_string(), threshold);

        monitor.register_trigger("test_trigger").await.unwrap();

        // This should trigger an alert
        let slow_metrics = PerformanceMetrics {
            execution_time_ms: 100.0, // Above threshold
            memory_used_mb: 20.0,
            cpu_utilization: 30.0,
            timestamp: Utc::now(),
        };

        monitor.record_execution("test_trigger", &slow_metrics).await.unwrap();

        let alerts = monitor.get_alerts(Some("test_trigger"), 10).await;
        assert!(!alerts.is_empty());
        assert_eq!(alerts[0].alert_type, AlertType::SlowExecution);
    }

    #[tokio::test]
    async fn test_performance_report() {
        let monitor = TriggerPerformanceMonitor::new();

        monitor.register_trigger("test_trigger").await.unwrap();

        // Add some metrics
        for i in 1..=5 {
            let metrics = PerformanceMetrics {
                execution_time_ms: (i * 100) as f64,
                memory_used_mb: (i * 10) as f64,
                cpu_utilization: (i * 5) as f64,
                timestamp: Utc::now(),
            };
            monitor.record_execution("test_trigger", &metrics).await.unwrap();
        }

        let report = monitor.generate_report("test_trigger").await.unwrap();
        assert_eq!(report.trigger_name, "test_trigger");
        assert_eq!(report.total_executions, 5);
        assert_eq!(report.avg_execution_time_ms, 300.0); // (100+200+300+400+500)/5
    }

    #[tokio::test]
    async fn test_optimization_recommendations() {
        let monitor = TriggerPerformanceMonitor::new();

        let stats = TriggerStats {
            total_executions: 1000,
            successful_executions: 900,
            failed_executions: 100,
            avg_execution_time_ms: 600.0, // Slow
            max_execution_time_ms: 2000.0,
            min_execution_time_ms: 100.0,
            last_executed: Some(Utc::now()),
        };

        let recommendations = monitor.generate_recommendations("test_trigger", &stats).await.unwrap();

        assert!(!recommendations.is_empty());
        // Should have recommendations for slow execution, high variance, and high error rate
        assert!(recommendations.len() >= 2);
    }
}
